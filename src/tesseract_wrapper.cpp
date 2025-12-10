#include "tesseract_wrapper.hpp"
#include "napi.h"

Napi::Object TesseractWrapper::GetClass(Napi::Env env, Napi::Object exports) {
  Napi::Function func = DefineClass(
      env, "Tesseract",
      {
          InstanceMethod("init", &TesseractWrapper::InitEngine),
          InstanceMethod("setVariable", &TesseractWrapper::SetVariable),
          InstanceMethod("setImage", &TesseractWrapper::SetImage),
          InstanceMethod("recognize", &TesseractWrapper::Recognize),
          InstanceMethod("getUTF8Text", &TesseractWrapper::GetUTF8Text),
          InstanceMethod("getHOCR", &TesseractWrapper::GetUTF8Text),
          InstanceMethod("getTSV", &TesseractWrapper::GetUTF8Text),
          InstanceMethod("getALTO", &TesseractWrapper::GetALTO),
          InstanceMethod("clear", &TesseractWrapper::Clear),
          InstanceMethod("end", &TesseractWrapper::End),
      });

  constructor = Napi::Persistent(func);
  constructor.SuppressDestruct();
  exports.Set("Tesseract", func);
  return exports;
}

TesseractWrapper::TesseractWrapper(const Napi::CallbackInfo &info)
    : Napi::ObjectWrap<TesseractWrapper>(info), _env(info.Env()) {
  Napi::Env env = info.Env();

  if (info.Length() == 1 && info[0].IsObject()) {
    auto ctorOptions = info[0].As<Napi::Object>();

    // const Napi::Value skipOcrOption = ctorOptions.Get("skipOcr");
    // if (!skipOcrOption.IsUndefined() && skipOcrOption.IsBoolean()) {
    //   _skipOcr = skipOcrOption.As<Napi::Boolean>().Value();
    // }

    const Napi::Value dataPathOption = ctorOptions.Get("dataPath");
    if (!dataPathOption.IsUndefined() && dataPathOption.IsString()) {
      _dataPath = dataPathOption.As<Napi::String>().Utf8Value();
    }

    const Napi::Value langOption = ctorOptions.Get("lang");
    if (!langOption.IsUndefined() && langOption.IsString()) {
      _lang = langOption.As<Napi::String>().Utf8Value();
    }

    const Napi::Value engineModeOption = ctorOptions.Get("engineMode");
    if (!engineModeOption.IsUndefined() && engineModeOption.IsNumber()) {
      _oem = static_cast<tesseract::OcrEngineMode>(
          engineModeOption.As<Napi::Number>().Int32Value());
    }

    // const Napi::Value psmOption = ctorOptions.Get("psm");
    // if (!psmOption.IsUndefined() && psmOption.IsNumber()) {
    //   _psm = static_cast<tesseract::PageSegMode>(
    //       psmOption.As<Napi::Number>().Int32Value());
    // }

    if (_oem < 0 || _oem >= tesseract::OEM_COUNT) {
      Napi::TypeError::New(env, "Unsupported OCR Engine Mode")
          .ThrowAsJavaScriptException();
      return;
    }

    // if (psm_ < 0 || psm_ >= tesseract::PSM_COUNT) {
    //   Napi::TypeError::New(env, "Unsupported Page Segmentation Mode")
    //       .ThrowAsJavaScriptException();
    //   return;
    // }
  }
}

TesseractWrapper::~TesseractWrapper() {
  if (_initialized.load()) {
    _api.End();
  }
}

Napi::Value TesseractWrapper::InitEngine(const Napi::CallbackInfo &info) {
  Napi::Env env = info.Env();

  Napi::Promise::Deferred m_deffered = Napi::Promise::Deferred::New(env);

  if (_initialized.load()) {
    m_deffered.Reject(
        Napi::Error::New(env, "Tesseract API was already initialized").Value());
    return m_deffered.Promise();
  }

  const char *dataPath = _dataPath.empty() ? nullptr : _dataPath.c_str();
  const char *lang = _lang.empty() ? "eng" : _lang.c_str();

  // char **configs, int configs_size, const std::vector<std::string> *vars_vec,
  // const std::vector<std::string> *vars_values, bool set_only_non_debug_params
  if (_api.Init(dataPath, lang, _oem) != 0) {
    m_deffered.Reject(
        Napi::Error::New(env, "Failed to initialize Tesseract API").Value());
    return m_deffered.Promise();
  }

  _initialized.store(true);
  m_deffered.Resolve(env.Undefined());
  return m_deffered.Promise();
}

Napi::Value TesseractWrapper::SetVariable(const Napi::CallbackInfo &info) {}

Napi::Value TesseractWrapper::SetImage(const Napi::CallbackInfo &info) {}

Napi::Value TesseractWrapper::Recognize(const Napi::CallbackInfo &info) {}

Napi::Value TesseractWrapper::GetUTF8Text(const Napi::CallbackInfo &info) {}

Napi::Value TesseractWrapper::GetHOCR(const Napi::CallbackInfo &info) {}

Napi::Value TesseractWrapper::GetTSV(const Napi::CallbackInfo &info) {}

Napi::Value TesseractWrapper::GetALTO(const Napi::CallbackInfo &info) {}

Napi::Value TesseractWrapper::Clear(const Napi::CallbackInfo &info) {}

Napi::Value TesseractWrapper::End(const Napi::CallbackInfo &info) {}
