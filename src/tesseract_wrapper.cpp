#include "tesseract_wrapper.hpp"
#include "napi.h"
#include <format>
#include <tesseract/publictypes.h>

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

Napi::Value
TesseractWrapper::InitForAnalysePage(const Napi::CallbackInfo &info) {
  Napi::Env env = info.Env();

  bool previous_state = _initialized.exchange(true);

  if (previous_state != false) {
    Napi::Error::New(env, "Tesseract API was initialized")
        .ThrowAsJavaScriptException();
    return env.Undefined();
  } else {
    _api.InitForAnalysePage();
  }

  return env.Undefined();
}

Napi::Value TesseractWrapper::AnalysePage(const Napi::CallbackInfo &info) {
  Napi::Env env = info.Env();

  if (!_initialized.load()) {
    Napi::Error::New(env, "Tesseract API is not initialized yet")
        .ThrowAsJavaScriptException();
  }

  if (_busy.load()) {
    Napi::Error::New(env, "The Tesseract API currently marked busy")
        .ThrowAsJavaScriptException();
  }

  // call worker here
}

Napi::Value TesseractWrapper::SetPageMode(const Napi::CallbackInfo &info) {
  Napi::Env env = info.Env();

  if (info.Length() > 1) {
    Napi::Error::New(env, "Expected only one parameter")
        .ThrowAsJavaScriptException();
    return env.Undefined();
  }

  if (!info[0].IsNumber()) {
    Napi::Error::New(env,
                     "Expected page segmentation mode to be of type number")
        .ThrowAsJavaScriptException();
    return env.Undefined();
  }

  tesseract::PageSegMode psm = static_cast<tesseract::PageSegMode>(
      info[0].As<Napi::Number>().Int32Value());

  if (psm < 0 || psm >= tesseract::PageSegMode::PSM_COUNT) {
    Napi::Error::New(env, "Page Segmentation Mode out of range")
        .ThrowAsJavaScriptException();
    return env.Undefined();
  }

  _api.SetPageSegMode(psm);

  return Napi::Number::New(env, psm);
}

Napi::Value TesseractWrapper::SetVariable(const Napi::CallbackInfo &info) {
  Napi::Env env = info.Env();

  Napi::Promise::Deferred m_deferred = Napi::Promise::Deferred::New(env);

  if (!_initialized.load()) {
    m_deferred.Reject(
        Napi::Error::New(
            env, "Cannot call `SetVariable` to an uninitialized Tesseract API")
            .Value());
    return m_deferred.Promise();
  }

  if (info.Length() != 2) {
    m_deferred.Reject(Napi::Error::New(env, "Expected two arguments").Value());
    return m_deferred.Promise();
  }

  if (!info[0].IsString()) {
    m_deferred.Reject(
        Napi::Error::New(env, "Expected variable name to be a string").Value());
    return m_deferred.Promise();
  }
  if (!info[1].IsString()) {
    return m_deferred.Reject(
               Napi::Error::New(env, "Variable value must be a string")
                   .Value()),
           m_deferred.Promise();
  }

  const std::string variable_name = info[0].As<Napi::String>().Utf8Value();
  const std::string variable_value = info[1].As<Napi::String>().Utf8Value();

  if (!_api.SetVariable(variable_name.c_str(), variable_value.c_str())) {
    m_deferred.Reject(
        Napi::Error::New(env, std::format("Could not set variable: {}",
                                          variable_name.c_str()))
            .Value());
    m_deferred.Promise();
  }

  Napi::Array result = Napi::Array::New(env);
  result.Set(variable_name, variable_value);

  m_deferred.Resolve(result);
  return m_deferred.Promise();
}

Napi::Value TesseractWrapper::SetImage(const Napi::CallbackInfo &info) {}

// can partially apply rectangles, if for some reason
// there is a sparse value (eg undefined), or
// a property is missing from the rectangle definition
// it will sparse them out and only apply the ones that match
Napi::Value TesseractWrapper::SetRectangle(const Napi::CallbackInfo &info) {
  Napi::Env env = info.Env();
  Napi::Promise::Deferred m_deferred = Napi::Promise::Deferred::New(env);

  if (info.Length() != 1 || !info[0].IsArray()) {
    m_deferred.Reject(
        Napi::Error::New(env, "Expected first argument to be a array").Value());
    return m_deferred.Promise();
  }

  if (_api.GetInputImage() == nullptr) {
    m_deferred.Reject(
        Napi::Error::New(env, "SetRectangle needs to be called after SetImage")
            .Value());
    return m_deferred.Promise();
  }

  Napi::Array rectangles = info[0].As<Napi::Array>();
  int rectangle_count = rectangles.Length();

  Napi::Array applied_rectangles = Napi::Array::New(env);
  if (rectangle_count == 0) {
    m_deferred.Resolve(applied_rectangles);
    return m_deferred.Promise();
  }

  for (int i = 0; i < rectangle_count; i++) {
    Napi::Value value = rectangles.Get(i);
    if (!value.IsObject())
      continue;

    Napi::Object rectangle = value.As<Napi::Object>();
    Napi::Value maybe_left = rectangle.Get("left");
    Napi::Value maybe_top = rectangle.Get("top");
    Napi::Value maybe_width = rectangle.Get("width");
    Napi::Value maybe_height = rectangle.Get("height");

    if (!maybe_left.IsNumber() || !maybe_top.IsNumber() ||
        !maybe_width.IsNumber() || !maybe_height.IsNumber())
      continue;

    _api.SetRectangle(maybe_left.As<Napi::Number>().Int32Value(),
                      maybe_top.As<Napi::Number>().Int32Value(),
                      maybe_width.As<Napi::Number>().Int32Value(),
                      maybe_height.As<Napi::Number>().Int32Value());

    applied_rectangles.Set(i, rectangle);
  }

  m_deferred.Resolve(applied_rectangles);
  return m_deferred.Promise();
}

Napi::Value TesseractWrapper::Recognize(const Napi::CallbackInfo &info) {}

Napi::Value TesseractWrapper::GetUTF8Text(const Napi::CallbackInfo &info) {}

Napi::Value TesseractWrapper::GetHOCR(const Napi::CallbackInfo &info) {}

Napi::Value TesseractWrapper::GetTSV(const Napi::CallbackInfo &info) {}

Napi::Value TesseractWrapper::GetALTO(const Napi::CallbackInfo &info) {}

Napi::Value TesseractWrapper::Clear(const Napi::CallbackInfo &info) {
  Napi::Env env = info.Env();

  if (!_initialized.load()) {
    Napi::Error::New(env, "Cannot clear an uninitialized Tesseract API")
        .ThrowAsJavaScriptException();
  }

  _api.End();

  return env.Undefined();
}

Napi::Value TesseractWrapper::End(const Napi::CallbackInfo &info) {
  Napi::Env env = info.Env();

  if (!_initialized.load()) {
    Napi::Error::New(env, "Cannot terminate the internal Tesseract API")
        .ThrowAsJavaScriptException();
  }

  _api.End();

  return env.Undefined();
}
