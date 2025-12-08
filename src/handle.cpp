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

#include "handle.h"
#include "napi.h"
#include "ocr_worker.h"
#include <tesseract/pageiterator.h>
#include <tesseract/publictypes.h>

Napi::Object Handle::GetClass(Napi::Env env, Napi::Object exports) {
  Napi::Function funcs = DefineClass(
      env, "Tesseract", {InstanceMethod("recognize", &Handle::Recognize)});

  Napi::FunctionReference *constructor = new Napi::FunctionReference();

  *constructor = Napi::Persistent(funcs);
  exports.Set("Tesseract", funcs);
  env.SetInstanceData<Napi::FunctionReference>(constructor);

  return exports;
};

Handle::Handle(const Napi::CallbackInfo &info)
    : Napi::ObjectWrap<Handle>(info) {

  Napi::Env env = info.Env();

  if (info.Length() == 1 && info[0].IsObject()) {
    auto ctorOptions = info[0].As<Napi::Object>();

    const Napi::Value skipOcrOption = ctorOptions.Get("skipOcr");
    if (!skipOcrOption.IsUndefined() && skipOcrOption.IsBoolean()) {
      skipOcr_ = skipOcrOption.As<Napi::Boolean>().Value();
    }

    const Napi::Value dataPathOption = ctorOptions.Get("dataPath");
    if (!dataPathOption.IsUndefined() && dataPathOption.IsString()) {
      dataPath_ = dataPathOption.As<Napi::String>().Utf8Value();
    }

    const Napi::Value langOption = ctorOptions.Get("lang");
    if (!langOption.IsUndefined() && langOption.IsString()) {
      lang_ = langOption.As<Napi::String>().Utf8Value();
    }

    const Napi::Value engineModeOption = ctorOptions.Get("engineMode");
    if (!engineModeOption.IsUndefined() && engineModeOption.IsNumber()) {
      oemMode_ = static_cast<tesseract::OcrEngineMode>(
          engineModeOption.As<Napi::Number>().Int32Value());
    }

    const Napi::Value psmOption = ctorOptions.Get("psm");
    if (!psmOption.IsUndefined() && psmOption.IsNumber()) {
      psm_ = static_cast<tesseract::PageSegMode>(
          psmOption.As<Napi::Number>().Int32Value());
    }

    if (oemMode_ < 0 || oemMode_ >= tesseract::OEM_COUNT) {
      Napi::TypeError::New(env, "Unsupported OCR Engine Mode")
          .ThrowAsJavaScriptException();
      return;
    }

    if (psm_ < 0 || psm_ >= tesseract::PSM_COUNT) {
      Napi::TypeError::New(env, "Unsupported Page Segmentation Mode")
          .ThrowAsJavaScriptException();
      return;
    }
  }
}

Handle::~Handle() {}

std::unique_ptr<tesseract::TessBaseAPI> Handle::CreateApi() {
  auto api = std::make_unique<tesseract::TessBaseAPI>();
  if (skipOcr_) {
    api->InitForAnalysePage();
  } else {
    if (api->Init(dataPath_.c_str(), lang_.c_str(), oemMode_) == -1) {
      api->End();
      return nullptr;
    }
  }

  api->SetPageSegMode(static_cast<tesseract::PageSegMode>(psm_));
  return api;
}

Napi::Value Handle::Recognize(const Napi::CallbackInfo &info) {
  const Napi::Env env = info.Env();
  const Napi::Promise::Deferred deffered = Napi::Promise::Deferred::New(env);

  if (skipOcr_) {
    deffered.Reject(Napi::Error::New(env, "OCR not available when handle was "
                                          "created with `skipOcr` turned on")
                        .Value());
    return deffered.Promise();
  }

  if (info.Length() <= 0 || !info[0].IsBuffer()) {
    deffered.Reject(
        Napi::TypeError::New(env, "Expected argument at index 0 to be a Buffer")
            .Value());
    return deffered.Promise();
  }

  Napi::Function progressCallback = Napi::Function();
  if (info.Length() == 2 && info[1].IsObject()) {
    const Napi::Object recognizeOptions = info[1].As<Napi::Object>();
    const Napi::Value progressChangedOption =
        recognizeOptions.Get("progressChanged");
    if (!progressChangedOption.IsUndefined() &&
        progressChangedOption.IsFunction()) {
      progressCallback = progressChangedOption.As<Napi::Function>();
    }
  }

  auto imageBuffer = info[0].As<Napi::Buffer<uint8_t>>();
  auto *pWorker = new OCRWorker(this, info.This().As<Napi::Object>(),
                                imageBuffer, deffered, progressCallback);

  pWorker->Queue();

  return deffered.Promise();
}

// Napi::Value Handle::AnalyzeLayout(const Napi::CallbackInfo &info) {
//   const Napi::Env env = info.Env();
//   ArgParser args(info);
//
//   const Napi::Promise::Deferred deffered = Napi::Promise::Deferred::New(env);
//
//   if (!skipOcr_) {
//     deffered.Reject(
//         Napi::Error::New(
//             env, "Page analysis not available unless `skipOcr` is turned on")
//             .Value());
//
//     return deffered.Promise();
//   }
//
//   if (info.Length() < 1 || !info[0].IsBoolean()) {
//     deffered.Reject(Napi::TypeError::New(
//                         info.Env(), "Expected first argument to be a
//                         boolean") .Value());
//     return deffered.Promise();
//   }
//
//   bool merge_similar_words = info[0].As<Napi::Boolean>().Value();
//
//   api_->SetImage();
//
//   tesseract::PageIterator *iterator =
//   api_->AnalyseLayout(merge_similar_words); return;
// }

std::mutex &Handle::Mutex() { return mutex_; }
