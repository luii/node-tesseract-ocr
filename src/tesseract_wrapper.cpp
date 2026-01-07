/*
 * Copyright 2026 Philipp Czarnetzki
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express
 * or implied. See the License for the specific language governing
 * permissions and limitations under the License.
 */

#include "tesseract_wrapper.hpp"
#include <format>
#include <tesseract/publictypes.h>

Napi::Object TesseractWrapper::InitAddon(Napi::Env env, Napi::Object exports) {
  Napi::Function func = DefineClass(
      env, "Tesseract",
      {
          InstanceMethod("init", &TesseractWrapper::Init),
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
    : Napi::ObjectWrap<TesseractWrapper>(info), _env(info.Env()),
      _worker_thread(info.Env()) {
  Napi::Env env = info.Env();

  if (info.Length() == 1 && info[0].IsObject()) {
    auto ctorOptions = info[0].As<Napi::Object>();

    // const Napi::Value skipOcrOption = ctorOptions.Get("skipOcr");
    // if (!skipOcrOption.IsUndefined() && skipOcrOption.IsBoolean()) {
    //   _skipOcr = skipOcrOption.As<Napi::Boolean>().Value();
    // }

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

TesseractWrapper::~TesseractWrapper() { _worker_thread.Terminate(); }

Napi::Value TesseractWrapper::Init(const Napi::CallbackInfo &info) {
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

  return env.Undefined();
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

  if (_api.GetPageSegMode() != psm) {
    _api.SetPageSegMode(psm);
  }

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

Napi::Value TesseractWrapper::GetIntVariable(const Napi::CallbackInfo &info) {
  Napi::Env env = info.Env();

  if (info.Length() <= 0 || info.Length() > 1) {
    Napi::Error::New(env, "GetIntVariable(name): exactly 1 argument required")
        .ThrowAsJavaScriptException();
    return env.Undefined();
  }

  if (!info[0].IsString()) {
    Napi::Error::New(env, "GetIntVariable(name): name must be a string")
        .ThrowAsJavaScriptException();
    return env.Undefined();
  }

  int *value;
  std::string name = info[0].As<Napi::String>().Utf8Value();
  if (!_api.GetIntVariable(name.c_str(), value)) {
    return env.Undefined();
  }

  if (value == nullptr) {
    return env.Undefined();
  }

  return Napi::Number::New(env, *value);
}

Napi::Value TesseractWrapper::GetBoolVariable(const Napi::CallbackInfo &info) {
  Napi::Env env = info.Env();

  if (info.Length() <= 0 || info.Length() > 1) {
    Napi::Error::New(env, "GetBoolVariable(name): exactly 1 argument required")
        .ThrowAsJavaScriptException();
    return env.Undefined();
  }

  if (!info[0].IsString()) {
    Napi::Error::New(env, "GetBoolVariable(name): name must be a string")
        .ThrowAsJavaScriptException();
    return env.Undefined();
  }

  bool *value;
  std::string name = info[0].As<Napi::String>().Utf8Value();
  if (!_api.GetBoolVariable(name.c_str(), value)) {
    return env.Undefined();
  }

  if (value == nullptr) {
    return env.Undefined();
  }

  return Napi::Boolean::New(env, *value);
}

Napi::Value
TesseractWrapper::GetDoubleVariable(const Napi::CallbackInfo &info) {
  Napi::Env env = info.Env();

  if (info.Length() <= 0 || info.Length() > 1) {
    Napi::Error::New(env,
                     "GetDoubleVariable(name): exactly 1 argument required")
        .ThrowAsJavaScriptException();
    return env.Undefined();
  }

  if (!info[0].IsString()) {
    Napi::Error::New(env, "GetDoubleVariable(name): name must be a string")
        .ThrowAsJavaScriptException();
    return env.Undefined();
  }

  double *value;
  std::string name = info[0].As<Napi::String>().Utf8Value();
  if (!_api.GetDoubleVariable(name.c_str(), value)) {
    return env.Undefined();
  }

  if (value == nullptr) {
    return env.Undefined();
  }

  return Napi::Number::New(env, *value);
}

Napi::Value
TesseractWrapper::GetStringVariable(const Napi::CallbackInfo &info) {
  Napi::Env env = info.Env();

  if (info.Length() <= 0 || info.Length() > 1) {
    Napi::Error::New(env,
                     "GetStringVariable(name): exactly 1 argument required")
        .ThrowAsJavaScriptException();
    return env.Undefined();
  }

  if (!info[0].IsString()) {
    Napi::Error::New(env, "GetStringVariable(name): name must be a string")
        .ThrowAsJavaScriptException();
    return env.Undefined();
  }

  std::string name = info[0].As<Napi::String>().Utf8Value();
  const char *value = _api.GetStringVariable(name.c_str());
  if (value == nullptr) {
    return env.Undefined();
  }

  return Napi::String::New(env, value);
}

Napi::Value TesseractWrapper::SetImage(const Napi::CallbackInfo &info) {
  return info.Env().Undefined();
}

Napi::Value TesseractWrapper::SetRectangle(const Napi::CallbackInfo &info) {
  Napi::Env env = info.Env();
  Napi::Promise::Deferred m_deferred = Napi::Promise::Deferred::New(env);

  if (info.Length() != 1 || !info[0].IsObject()) {
    m_deferred.Reject(
        Napi::Error::New(env, "Expected first argument to be a object")
            .Value());
    return m_deferred.Promise();
  }

  if (_api.GetInputImage() == nullptr) {
    m_deferred.Reject(
        Napi::Error::New(env, "SetRectangle needs to be called after SetImage")
            .Value());
    return m_deferred.Promise();
  }

  Napi::Object rectangle = info[0].As<Napi::Object>();

  Napi::Value maybe_left = rectangle.Get("left");
  Napi::Value maybe_top = rectangle.Get("top");
  Napi::Value maybe_width = rectangle.Get("width");
  Napi::Value maybe_height = rectangle.Get("height");

  if (!maybe_left.IsNumber() || !maybe_top.IsNumber() ||
      !maybe_width.IsNumber() || !maybe_height.IsNumber()) {
    m_deferred.Reject(
        Napi::Error::New(env,
                         "SetRectangle: missing property in rectangle object")
            .Value());
    return m_deferred.Promise();
  }

  _api.SetRectangle(maybe_left.As<Napi::Number>().Int32Value(),
                    maybe_top.As<Napi::Number>().Int32Value(),
                    maybe_width.As<Napi::Number>().Int32Value(),
                    maybe_height.As<Napi::Number>().Int32Value());

  m_deferred.Resolve(rectangle);
  return m_deferred.Promise();
}

Napi::Value
TesseractWrapper::SetSourceResolution(const Napi::CallbackInfo &info) {
  Napi::Env env = info.Env();

  if (info.Length() > 1) {
    Napi::Error::New(env, "Expected only one parameter")
        .ThrowAsJavaScriptException();
    return env.Undefined();
  }

  if (!info[0].IsNumber()) {
    Napi::Error::New(env, "Expected ppi to be of type number")
        .ThrowAsJavaScriptException();
    return env.Undefined();
  }

  if (_api.GetInputImage() == nullptr) {
    Napi::Error::New(env,
                     "SetSourceResolution cannot be called before `SetImage`")
        .ThrowAsJavaScriptException();
    return env.Undefined();
  }

  int ppi = info[0].As<Napi::Number>().Int32Value();
  _api.SetSourceResolution(ppi);

  return Napi::Number::New(env, ppi);
}

Napi::Value TesseractWrapper::Recognize(const Napi::CallbackInfo &info) {
  return info.Env().Undefined();
}

Napi::Value
TesseractWrapper::DetectOrientationScript(const Napi::CallbackInfo &info) {
  Napi::Env env = info.Env();

  if (_api.GetInputImage() == nullptr) {
    Napi::Error::New(
        env, "DetectOrientationScript: cannot be called before `SetImage`")
        .ThrowAsJavaScriptException();
    return env.Undefined();
  }

  int orient_deg;
  float orient_conf;
  const char *script_name;
  float script_conf;

  if (!_api.DetectOrientationScript(&orient_deg, &orient_conf, &script_name,
                                    &script_conf)) {
    Napi::Error::New(
        env, "DetectOrientationScript: could not detect orientation or script")
        .ThrowAsJavaScriptException();
    return env.Undefined();
  }

  Napi::Object result = Napi::Object::New(env);

  result.Set("orient_deg", Napi::Number::New(env, orient_deg));
  result.Set("orient_conf", Napi::Number::New(env, orient_conf));
  result.Set("script_name", Napi::String::New(env, script_name));
  result.Set("script_conf", Napi::Number::New(env, script_conf));

  return result;
}

Napi::Value TesseractWrapper::MeanTextConf(const Napi::CallbackInfo &info) {
  Napi::Env env = info.Env();

  // _api.MeanTextConf();

  return env.Undefined();
}

// Napi::Value TesseractWrapper::GetUTF8Text(const Napi::CallbackInfo &info) {
//   return info.Env().Undefined();
// }
//
// Napi::Value TesseractWrapper::GetHOCR(const Napi::CallbackInfo &info) {}
//
// Napi::Value TesseractWrapper::GetTSV(const Napi::CallbackInfo &info) {}
//
// Napi::Value TesseractWrapper::GetALTO(const Napi::CallbackInfo &info) {}
//
// Napi::Value TesseractWrapper::GetUNLV(const Napi::CallbackInfo &info) {}

Napi::Value TesseractWrapper::GetInitLanguages(const Napi::CallbackInfo &info) {
  Napi::Env env = info.Env();
  const char *init_languages = _api.GetInitLanguagesAsString();

  if (init_languages == nullptr) {
    return env.Undefined();
  }

  return Napi::String::New(env, std::string(init_languages));
}

Napi::Value
TesseractWrapper::GetLoadedLanguages(const Napi::CallbackInfo &info) {
  Napi::Env env = info.Env();

  std::vector<std::string> langs;
  _api.GetLoadedLanguagesAsVector(&langs);

  Napi::Array result =
      Napi::Array::New(env, static_cast<uint32_t>(langs.size()));
  for (size_t i = 0; i < langs.size(); ++i) {
    result.Set(static_cast<uint32_t>(i), Napi::String::New(env, langs[i]));
  }

  return result;
}

Napi::Value
TesseractWrapper::GetAvailableLanguages(const Napi::CallbackInfo &info) {
  Napi::Env env = info.Env();

  std::vector<std::string> langs;
  _api.GetAvailableLanguagesAsVector(&langs);

  Napi::Array result =
      Napi::Array::New(env, static_cast<uint32_t>(langs.size()));
  for (size_t i = 0; i < langs.size(); ++i) {
    result.Set(static_cast<uint32_t>(i), Napi::String::New(env, langs[i]));
  }

  return result;
}

Napi::Value TesseractWrapper::Clear(const Napi::CallbackInfo &info) {
  Napi::Env env = info.Env();

  if (!_initialized.load()) {
    Napi::Error::New(env, "Cannot clear an uninitialized Tesseract API")
        .ThrowAsJavaScriptException();
  }

  _api.Clear();

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
