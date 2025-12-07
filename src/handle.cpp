#include "handle.h"
#include "napi.h"
#include "ocr_worker.h"
#include <tesseract/pageiterator.h>
#include <tesseract/publictypes.h>

Napi::Object Handle::Init(Napi::Env env, Napi::Object exports) {

  Napi::Function funcs =
      DefineClass(env, "Tesseract",
                  {InstanceMethod<&Handle::SetPageMode>(
                       "setPageMode", static_cast<napi_property_attributes>(
                                          napi_writable | napi_configurable)),
                   InstanceMethod<&Handle::Recognize>(
                       "recognize", static_cast<napi_property_attributes>(
                                        napi_writable | napi_configurable))});

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

  api_ = std::make_unique<tesseract::TessBaseAPI>();
  if (skipOcr_) {
    api_->InitForAnalysePage();
  } else {
    if (api_->Init(dataPath_.c_str(), lang_.c_str(), oemMode_)) {
      api_->End();

      Napi::Error::New(env, "Could not initialize Tesseract API")
          .ThrowAsJavaScriptException();
      return;
    }
  }

  api_->SetPageSegMode(static_cast<tesseract::PageSegMode>(psm_));
}

Handle::~Handle() {
  if (api_) {
    api_->End();
  }
}

void Handle::SetPageMode(const Napi::CallbackInfo &info) {
  const Napi::Env env = info.Env();

  if (!api_) {
    Napi::Error::New(env,
                     "Cannot set page mode when api is not available anymore")
        .ThrowAsJavaScriptException();
    return;
  }

  if (info.Length() <= 0 || !info[0].IsNumber()) {
    Napi::TypeError::New(env, "Expected argument at index 0 to be a number")
        .ThrowAsJavaScriptException();
    return;
  }

  tesseract::PageSegMode psm = static_cast<tesseract::PageSegMode>(
      info[0].As<Napi::Number>().Int32Value());

  if (psm < 0 || psm >= tesseract::PSM_COUNT) {
    Napi::TypeError::New(info.Env(), "Unsupported Page Segmentation Mode")
        .ThrowAsJavaScriptException();
    return;
  }

  api_->SetPageSegMode(psm);

  return;
}

Napi::Value Handle::Recognize(const Napi::CallbackInfo &info) {
  const Napi::Env env = info.Env();
  const Napi::Promise::Deferred deffered = Napi::Promise::Deferred::New(env);

  if (!api_) {
    deffered.Reject(
        Napi::Error::New(
            env, "Cannot recognize image with no available tesseract api")
            .Value());
    return deffered.Promise();
  }

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

  // check here for the abort signal

  auto imageBuffer = info[0].As<Napi::Buffer<uint8_t>>();
  auto *pWorker =
      new OCRWorker(env, this, imageBuffer, deffered, progressCallback);

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

tesseract::TessBaseAPI *Handle::Api() { return api_.get(); }

std::mutex &Handle::Mutex() { return mutex_; }
