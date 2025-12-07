#include "ocr_worker.h"
#include "napi.h"
#include "ocr_result.h"
#include <cstddef>
#include <cstdio>
#include <tesseract/baseapi.h>
#include <tesseract/ocrclass.h>

OCRWorker::OCRWorker(const Napi::Env &env, Handle *handle,
                     Napi::Buffer<uint8_t> buffer,
                     Napi::Promise::Deferred deffered,
                     Napi::Function &progressCallback)
    : Napi::AsyncProgressWorker<ProgressPayload>{handle->Env(), "OCRWorker"},
      handle_(handle), data_(buffer.Data()), length_(buffer.Length()),
      deffered_{deffered} {

  this->progressCallback_.Reset(progressCallback, 1);

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

void OCRWorker::Execute(const ExecutionProgress &executionProgress) {

  Pix *pPix = pixReadMem(data_, length_);

  if (!pPix) {
    SetError("Could not read image from buffer");
    return;
  }

  {
    std::lock_guard<std::mutex> lock(handle_->Mutex());
    tesseract::TessBaseAPI *api = handle_->Api();

    api->SetImage(pPix);
    monitor_->cancel_this = (void *)&executionProgress;

    if (api->Recognize(monitor_) != 0) {
      pixDestroy(&pPix);
      monitor_->cancel_this = nullptr;
      SetError("Recognize failed");
      return;
    }
  }

  monitor_->cancel_this = nullptr;
  pixDestroy(&pPix);
}

void OCRWorker::OnOK() {
  Napi::Env env = Env();
  Napi::HandleScope scope(env);

  this->deffered_.Resolve(OCRResult::NewInstance(env, handle_));
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

  // printf("percent=%i; progress=%i; ocr_alive=%i; top=%i; right=%i; bottom=%i;
  // "
  //        "left=%i;\n",
  //        payload->progress, payload->percent, payload->ocr_alive,
  //        payload->bbox.top, payload->bbox.right, payload->bbox.bottom,
  //        payload->bbox.left);

  if (!this->progressCallback_.IsEmpty()) {
    progressCallback_.Call(Env().Undefined(), {progress});
  }
}
