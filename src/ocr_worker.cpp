/*
 * node-tesseract-ocr
 * Copyright (C) 2025  Philipp Czarnetzki
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "ocr_worker.h"
#include "napi.h"
#include "ocr_result.h"
#include <cstddef>
#include <cstdio>
#include <tesseract/baseapi.h>
#include <tesseract/ocrclass.h>

OCRWorker::OCRWorker(Handle *handle, Napi::Object handleObject,
                     Napi::Buffer<uint8_t> buffer,
                     Napi::Promise::Deferred deffered,
                     Napi::Function &progressCallback)
    : Napi::AsyncProgressWorker<ProgressPayload>{handle->Env(), "OCRWorker"},
      handle_(handle), data_(buffer.Data()), length_(buffer.Length()),
      deffered_{deffered} {

  this->progressCallback_.Reset(progressCallback, 1);

  // Hold a persistent reference to the JS Buffer.
  // So its memory isn't freed while the async worker is running.
  this->bufferRef_.Reset(buffer, 1);
  // Hold a persistent reference to the JS Handle object.
  // So it isn't GC'd while this worker is active.
  this->handleRef_.Reset(handleObject, 1);

  // allocate and zero-init the monitor to avoid uninitialized fields
  monitor_ = new tesseract::ETEXT_DESC();

  monitor_->progress_callback2 = [](tesseract::ETEXT_DESC *monitor, int left,
                                    int right, int top, int bottom) -> bool {
    ProgressPayload payload{monitor->more_to_come,
                            monitor->progress,
                            monitor->ocr_alive,
                            top,
                            right,
                            bottom,
                            left};
    auto *executionProgress =
        static_cast<OCRWorker::ExecutionProgress *>(monitor->cancel_this);
    if (executionProgress == nullptr) {
      return false;
    }
    executionProgress->Send(&payload, 1);
    return false;
  };
  monitor_->ocr_alive = 1;
};

OCRWorker::~OCRWorker() {
  // release the persistent buffer reference
  if (!this->bufferRef_.IsEmpty()) {
    this->bufferRef_.Reset();
  }

  // free monitor
  if (monitor_) {
    delete monitor_;
    monitor_ = nullptr;
  }
  if (!this->handleRef_.IsEmpty()) {
    this->handleRef_.Reset();
  }
}

void OCRWorker::Execute(const ExecutionProgress &executionProgress) {

  Pix *pPix = pixReadMem(data_, length_);

  if (!pPix) {
    SetError("Could not read image from buffer");
    return;
  }

  // determine image depth and normalize to 8-bit grayscale (if needed)
  int depth = pixGetDepth(pPix);
  if (depth != 8) {
    Pix *pGray = pixConvertTo8(pPix, 0); /* 0 = no colormap */
    if (pGray) {
      pixDestroy(&pPix);
      pPix = pGray;
    }
  }

  std::unique_ptr<tesseract::TessBaseAPI> localApi;
  {
    std::lock_guard<std::mutex> lock(handle_->Mutex());

    // Create a thread-local TessBaseAPI to avoid sharing across threads;
    localApi = handle_->CreateApi();

    if (!localApi) {
      pixDestroy(&pPix);
      SetError("Could not initialize thread-local Tesseract API");
      return;
    }

    localApi->SetImage(pPix);
    monitor_->cancel_this = (void *)&executionProgress;

    if (localApi->Recognize(monitor_) != 0) {
      pixDestroy(&pPix);
      monitor_->cancel_this = nullptr;
      SetError("Recognize failed");
      return;
    }
  }

  // retrieve results from the local API
  {
    char *t = nullptr;
    t = localApi->GetUTF8Text();
    if (t) {
      resultText_.assign(t);
      delete[] t;
    }

    t = localApi->GetHOCRText(0);
    if (t) {
      resultHOCR_.assign(t);
      delete[] t;
    }

    t = localApi->GetTSVText(0);
    if (t) {
      resultTSV_.assign(t);
      delete[] t;
    }

    t = localApi->GetAltoText(0);
    if (t) {
      resultALTO_.assign(t);
      delete[] t;
    }
  }

  monitor_->cancel_this = nullptr;
  pixDestroy(&pPix);
}

void OCRWorker::OnOK() {
  Napi::Env env = Env();
  Napi::HandleScope scope(env);

  Napi::Object resultObj = OCRResult::NewInstance(env, resultText_, resultHOCR_,
                                                  resultTSV_, resultALTO_);
  // Attach a reference to the Handle JS object on the result.
  // So the Handle stays alive as long as the OCRResult is reachable from JS.
  if (!handleRef_.IsEmpty()) {
    resultObj.Set("_handle", handleRef_.Value());
  }

  this->deffered_.Resolve(resultObj);
}

void OCRWorker::OnError(const Napi::Error &error) {
  Napi::HandleScope scope(Env());
  this->deffered_.Reject(error.Value());
}

void OCRWorker::OnProgress(const ProgressPayload *payload, size_t count) {
  Napi::HandleScope scope(Env());
  Napi::Object progress = Napi::Object::New(Env());

  progress.Set("percent", payload->percent);
  progress.Set("progress", payload->progress);
  progress.Set("ocr_alive", payload->ocr_alive);
  progress.Set("top", payload->top);
  progress.Set("right", payload->right);
  progress.Set("bottom", payload->bottom);
  progress.Set("left", payload->left);

  if (!this->progressCallback_.IsEmpty()) {
    progressCallback_.Call(Env().Undefined(), {progress});
  }
}
