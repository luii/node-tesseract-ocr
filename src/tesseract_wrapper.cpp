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
#include "commands.hpp"
#include "worker_thread.hpp"
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <leptonica/allheaders.h>
#include <string>
#include <tesseract/publictypes.h>

Napi::FunctionReference TesseractWrapper::constructor;

Napi::Object TesseractWrapper::InitAddon(Napi::Env env, Napi::Object exports) {
  Napi::Function func = DefineClass(
      env, "Tesseract",
      {
          InstanceMethod("init", &TesseractWrapper::Init),
          InstanceMethod("initForAnalysePage",
                         &TesseractWrapper::InitForAnalysePage),
          InstanceMethod("analysePage", &TesseractWrapper::AnalysePage),
          InstanceMethod("setVariable", &TesseractWrapper::SetVariable),
          InstanceMethod("getIntVariable", &TesseractWrapper::GetIntVariable),
          InstanceMethod("getBoolVariable", &TesseractWrapper::GetBoolVariable),
          InstanceMethod("getDoubleVariable",
                         &TesseractWrapper::GetDoubleVariable),
          InstanceMethod("getStringVariable",
                         &TesseractWrapper::GetStringVariable),
          // InstanceMethod("printVariables",
          // &TesseractWrapper::PrintVariables),
          InstanceMethod("setImage", &TesseractWrapper::SetImage),
          InstanceMethod("setPageMode", &TesseractWrapper::SetPageMode),
          InstanceMethod("setRectangle", &TesseractWrapper::SetRectangle),
          InstanceMethod("setSourceResolution",
                         &TesseractWrapper::SetSourceResolution),
          InstanceMethod("recognize", &TesseractWrapper::Recognize),
          InstanceMethod("detectOrientationScript",
                         &TesseractWrapper::DetectOrientationScript),
          InstanceMethod("meanTextConf", &TesseractWrapper::MeanTextConf),
          InstanceMethod("getUTF8Text", &TesseractWrapper::GetUTF8Text),
          InstanceMethod("getHOCRText", &TesseractWrapper::GetHOCRText),
          InstanceMethod("getTSVText", &TesseractWrapper::GetTSVText),
          InstanceMethod("getUNLVText", &TesseractWrapper::GetUNLVText),
          InstanceMethod("getALTOText", &TesseractWrapper::GetALTOText),
          InstanceMethod("getInitLanguages",
                         &TesseractWrapper::GetInitLanguages),
          InstanceMethod("getLoadedLanguages",
                         &TesseractWrapper::GetLoadedLanguages),
          InstanceMethod("getAvailableLanguages",
                         &TesseractWrapper::GetAvailableLanguages),
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
      _worker_thread(info.Env()) {}

TesseractWrapper::~TesseractWrapper() {}

Napi::Value TesseractWrapper::Init(const Napi::CallbackInfo &info) {
  Napi::Env env = info.Env();
  Napi::Promise::Deferred deferred = Napi::Promise::Deferred::New(env);

  if (info.Length() != 1 || !info[0].IsObject()) {
    deferred.Reject(
        Napi::TypeError::New(
            env, "Expected required argument at index 0 to be of type object")
            .Value());
    return deferred.Promise();
  }

  auto options = info[0].As<Napi::Object>();
  CommandInit command{};

  const Napi::Value langOption = options.Get("lang");
  if (!langOption.IsUndefined()) {
    if (!langOption.IsArray()) {
      deferred.Reject(
          Napi::TypeError::New(env, "Option 'lang' must be a array of strings")
              .Value());
      return deferred.Promise();
    }

    Napi::Array languages = langOption.As<Napi::Array>();
    std::string language;

    for (uint32_t i = 0; i < languages.Length(); ++i) {
      if (!languages.Get(i).IsString())
        continue;
      if (!language.empty())
        language += "+";
      language += languages.Get(i).As<Napi::String>().Utf8Value();
    }

    command.language = language;
  }

  const Napi::Value engineModeOption = options.Get("oem");
  if (!engineModeOption.IsUndefined()) {
    if (!engineModeOption.IsNumber()) {
      deferred.Reject(
          Napi::TypeError::New(env, "Option 'oem' must be of type number")
              .Value());
      return deferred.Promise();
    }
    tesseract::OcrEngineMode oem = static_cast<tesseract::OcrEngineMode>(
        engineModeOption.As<Napi::Number>().Int32Value());

    if (oem < 0 || oem >= tesseract::OEM_COUNT) {
      deferred.Reject(
          Napi::TypeError::New(env, "Unsupported OCR Engine Mode").Value());
      return deferred.Promise();
    }

    command.oem = oem;
  }

  const Napi::Value set_only_non_debug_params =
      options.Get("setOnlyNonDebugParams");
  if (!set_only_non_debug_params.IsUndefined()) {
    if (!set_only_non_debug_params.IsBoolean()) {
      deferred.Reject(
          Napi::TypeError::New(
              env, "Option 'setOnlyNonDebugParams' must be of type boolean")
              .Value());
      return deferred.Promise();
    }

    command.set_only_non_debug_params =
        set_only_non_debug_params.As<Napi::Boolean>().Value();
  }

  const Napi::Value v = options.Get("configs");
  if (!v.IsUndefined()) {
    if (!v.IsArray()) {
      deferred.Reject(Napi::TypeError::New(
                          env, "Option 'configs' must be an array of strings")
                          .Value());
      return deferred.Promise();
    }

    Napi::Array arr = v.As<Napi::Array>();
    const uint32_t len = arr.Length();

    command.configs_storage.reserve(len);
    command.configs.reserve(len);

    for (uint32_t i = 0; i < len; ++i) {
      Napi::Value item = arr.Get(i);
      if (!item.IsString()) {
        deferred.Reject(Napi::TypeError::New(
                            env, "Option 'configs' must contain only strings")
                            .Value());
        return deferred.Promise();
      }
      command.configs_storage.emplace_back(item.As<Napi::String>().Utf8Value());
    }

    // after storage is stable, build char* array
    for (auto &s : command.configs_storage) {
      command.configs.push_back(const_cast<char *>(s.c_str()));
    }
  }

  const Napi::Value vars_vec = options.Get("varsVec");
  const Napi::Value vars_values = options.Get("varsValues");

  const bool has_vars_vec = !vars_vec.IsUndefined();
  const bool has_vars_values = !vars_values.IsUndefined();

  if (has_vars_vec != has_vars_values) {
    deferred.Reject(
        Napi::TypeError::New(
            env, "Options 'varsVec' and 'varsValues' must be provided together")
            .Value());
    return deferred.Promise();
  }

  if (has_vars_vec) {
    if (!vars_vec.IsArray() || !vars_values.IsArray()) {
      deferred.Reject(
          Napi::Error::New(
              env,
              "Options 'varsVec' and 'varsValues' must be arrays of strings")
              .Value());
      return deferred.Promise();
    }

    Napi::Array a = vars_vec.As<Napi::Array>();
    Napi::Array b = vars_values.As<Napi::Array>();

    if (a.Length() != b.Length()) {
      deferred.Reject(
          Napi::TypeError::New(
              env, "'varsVec' and 'varsValues' must have the same length")
              .Value());
      return deferred.Promise();
    }

    const uint32_t len = a.Length();
    command.vars_vec.reserve(len);
    command.vars_values.reserve(len);

    for (uint32_t i = 0; i < len; ++i) {
      Napi::Value k = a.Get(i);
      Napi::Value val = b.Get(i);
      if (!k.IsString() || !val.IsString()) {
        deferred.Reject(
            Napi::TypeError::New(
                env, "'varsVec' and 'varsValues' must contain only strings")
                .Value());
        return deferred.Promise();
      }
      command.vars_vec.emplace_back(k.As<Napi::String>().Utf8Value());
      command.vars_values.emplace_back(val.As<Napi::String>().Utf8Value());
    }
  }

  return _worker_thread.Enqueue(command);
}

Napi::Value
TesseractWrapper::InitForAnalysePage(const Napi::CallbackInfo &info) {
  return _worker_thread.Enqueue(CommandInitForAnalysePage{});
}

Napi::Value TesseractWrapper::AnalysePage(const Napi::CallbackInfo &info) {
  Napi::Env env = info.Env();
  Napi::Promise::Deferred deferred = Napi::Promise::Deferred::New(env);
  CommandAnalyseLayout command{};

  if (info.Length() > 1) {
    deferred.Reject(Napi::Error::New(env, "Too many arguments").Value());
    return deferred.Promise();
  }

  if (!info[0].IsUndefined()) {
    if (!info[0].IsBoolean()) {
      deferred.Reject(
          Napi::Error::New(env, "Expected argument to be of type boolean")
              .Value());
      return deferred.Promise();
    }

    command.merge_similar_words = info[0].As<Napi::Boolean>().Value();
  }

  return _worker_thread.Enqueue(command);
}

Napi::Value TesseractWrapper::SetPageMode(const Napi::CallbackInfo &info) {
  Napi::Env env = info.Env();
  Napi::Promise::Deferred deferred = Napi::Promise::Deferred::New(env);
  CommandSetPageMode command{};

  if (info.Length() > 1) {
    deferred.Reject(
        Napi::Error::New(env, "Expected only one parameter").Value());
    return deferred.Promise();
  }

  if (!info[0].IsUndefined()) {
    if (!info[0].IsNumber()) {
      deferred.Reject(
          Napi::Error::New(
              env, "Expected page segmentation mode to be of type number")
              .Value());
      return deferred.Promise();
    }

    tesseract::PageSegMode psm = static_cast<tesseract::PageSegMode>(
        info[0].As<Napi::Number>().Int32Value());

    if (psm < 0 || psm >= tesseract::PageSegMode::PSM_COUNT) {
      deferred.Reject(
          Napi::Error::New(env, "Page Segmentation Mode out of range").Value());
      return deferred.Promise();
    }

    command.psm = psm;
  }

  return _worker_thread.Enqueue(command);
}

Napi::Value TesseractWrapper::SetVariable(const Napi::CallbackInfo &info) {
  Napi::Env env = info.Env();
  CommandSetVariable command{};

  Napi::Promise::Deferred deferred = Napi::Promise::Deferred::New(env);

  if (info.Length() != 2) {
    deferred.Reject(Napi::Error::New(env, "Expected two arguments").Value());
    return deferred.Promise();
  }

  if (!info[0].IsString()) {
    deferred.Reject(
        Napi::Error::New(env, "Expected variable name to be a string").Value());
    return deferred.Promise();
  }
  if (!info[1].IsString()) {
    return deferred.Reject(
               Napi::Error::New(env, "Variable value must be a string")
                   .Value()),
           deferred.Promise();
  }

  const std::string variable_name = info[0].As<Napi::String>().Utf8Value();
  const std::string variable_value = info[1].As<Napi::String>().Utf8Value();

  command.name = variable_name;
  command.value = variable_value;

  return _worker_thread.Enqueue(command);
}

Napi::Value TesseractWrapper::GetIntVariable(const Napi::CallbackInfo &info) {
  Napi::Env env = info.Env();
  Napi::Promise::Deferred deferred = Napi::Promise::Deferred::New(env);
  CommandGetIntVariable command{};

  if (info.Length() != 1) {
    deferred.Reject(
        Napi::Error::New(env,
                         "GetIntVariable(name): exactly 1 argument required")
            .Value());
    return deferred.Promise();
  }

  if (!info[0].IsString()) {
    deferred.Reject(
        Napi::Error::New(env, "GetIntVariable(name): name must be a string")
            .Value());
    return deferred.Promise();
  }

  std::string name = info[0].As<Napi::String>().Utf8Value();
  command.name = name;

  return _worker_thread.Enqueue(command);
}

Napi::Value TesseractWrapper::GetBoolVariable(const Napi::CallbackInfo &info) {
  Napi::Env env = info.Env();
  Napi::Promise::Deferred deferred = Napi::Promise::Deferred::New(env);
  CommandGetBoolVariable command{};

  if (info.Length() != 1) {
    deferred.Reject(
        Napi::Error::New(env,
                         "GetIntVariable(name): exactly 1 argument required")
            .Value());
    return deferred.Promise();
  }

  if (!info[0].IsString()) {
    deferred.Reject(
        Napi::Error::New(env, "GetIntVariable(name): name must be a string")
            .Value());
    return deferred.Promise();
  }

  std::string name = info[0].As<Napi::String>().Utf8Value();
  command.name = name;

  return _worker_thread.Enqueue(command);
}

Napi::Value
TesseractWrapper::GetDoubleVariable(const Napi::CallbackInfo &info) {
  Napi::Env env = info.Env();
  Napi::Promise::Deferred deferred = Napi::Promise::Deferred::New(env);
  CommandGetDoubleVariable command{};

  if (info.Length() != 1) {
    deferred.Reject(
        Napi::Error::New(env,
                         "GetIntVariable(name): exactly 1 argument required")
            .Value());
    return deferred.Promise();
  }

  if (!info[0].IsString()) {
    deferred.Reject(
        Napi::Error::New(env, "GetIntVariable(name): name must be a string")
            .Value());
    return deferred.Promise();
  }

  std::string name = info[0].As<Napi::String>().Utf8Value();
  command.name = name;

  return _worker_thread.Enqueue(command);
}

Napi::Value
TesseractWrapper::GetStringVariable(const Napi::CallbackInfo &info) {
  Napi::Env env = info.Env();
  Napi::Promise::Deferred deferred = Napi::Promise::Deferred::New(env);
  CommandGetStringVariable command{};

  if (info.Length() != 1) {
    deferred.Reject(
        Napi::Error::New(env,
                         "GetIntVariable(name): exactly 1 argument required")
            .Value());
    return deferred.Promise();
  }

  if (!info[0].IsString()) {
    deferred.Reject(
        Napi::Error::New(env, "GetIntVariable(name): name must be a string")
            .Value());
    return deferred.Promise();
  }

  std::string name = info[0].As<Napi::String>().Utf8Value();
  command.name = name;

  return _worker_thread.Enqueue(command);
}

Napi::Value TesseractWrapper::SetImage(const Napi::CallbackInfo &info) {
  Napi::Env env = info.Env();
  Napi::Promise::Deferred deferred = Napi::Promise::Deferred::New(env);
  CommandSetImage command{};

  if (info.Length() < 1 || !info[0].IsBuffer()) {
    deferred.Reject(
        Napi::Error::New(env, "SetImage(buffer): buffer required").Value());
    return deferred.Promise();
  }

  Napi::Buffer<uint8_t> image_buffer = info[0].As<Napi::Buffer<uint8_t>>();
  const uint8_t *data = image_buffer.Data();
  const size_t length = image_buffer.Length();

  if (length == 0) {
    deferred.Reject(Napi::Error::New(env, "SetImage: buffer is empty").Value());
    return deferred.Promise();
  }

  Pix *pix = pixReadMem(data, length);
  if (!pix) {
    deferred.Reject(
        Napi::Error::New(env, "SetImage: failed to decode image buffer")
            .Value());
    return deferred.Promise();
  }

  Pix *normalized = pix;
  if (pixGetColormap(normalized)) {
    Pix *no_cmap = pixRemoveColormap(normalized, REMOVE_CMAP_BASED_ON_SRC);
    if (no_cmap) {
      if (normalized != pix) {
        pixDestroy(&normalized);
      }
      normalized = no_cmap;
    }
  }

  const int depth = pixGetDepth(normalized);
  if (depth != 8 && depth != 32) {
    Pix *converted = pixConvertTo8(normalized, 0);
    if (converted) {
      if (normalized != pix) {
        pixDestroy(&normalized);
      }
      normalized = converted;
    }
  }

  const int width = pixGetWidth(normalized);
  const int height = pixGetHeight(normalized);
  const int wpl = pixGetWpl(normalized);
  const int bytes_per_line = wpl * 4;
  const int bytes_per_pixel = (pixGetDepth(normalized) == 32) ? 4 : 1;
  const size_t total_bytes =
      static_cast<size_t>(bytes_per_line) * static_cast<size_t>(height);

  if (width <= 0 || height <= 0 || bytes_per_line <= 0 || total_bytes == 0) {
    if (normalized != pix) {
      pixDestroy(&normalized);
    }
    pixDestroy(&pix);
    deferred.Reject(
        Napi::Error::New(env, "SetImage: invalid decoded image data").Value());
    return deferred.Promise();
  }

  command.width = width;
  command.height = height;
  command.bytes_per_pixel = bytes_per_pixel;
  command.bytes_per_line = bytes_per_line;
  command.bytes.resize(total_bytes);
  std::memcpy(command.bytes.data(), pixGetData(normalized), total_bytes);

  if (normalized != pix) {
    pixDestroy(&normalized);
  }
  pixDestroy(&pix);

  return _worker_thread.Enqueue(command);
}

Napi::Value TesseractWrapper::SetRectangle(const Napi::CallbackInfo &info) {
  Napi::Env env = info.Env();
  Napi::Promise::Deferred deferred = Napi::Promise::Deferred::New(env);
  CommandSetRectangle command{};

  if (info.Length() != 1 || !info[0].IsObject()) {
    deferred.Reject(
        Napi::Error::New(env, "Expected first argument to be a object")
            .Value());
    return deferred.Promise();
  }

  Napi::Object rectangle = info[0].As<Napi::Object>();

  Napi::Value maybe_left = rectangle.Get("left");
  Napi::Value maybe_top = rectangle.Get("top");
  Napi::Value maybe_width = rectangle.Get("width");
  Napi::Value maybe_height = rectangle.Get("height");

  if (!maybe_left.IsNumber() || !maybe_top.IsNumber() ||
      !maybe_width.IsNumber() || !maybe_height.IsNumber()) {
    deferred.Reject(
        Napi::Error::New(env,
                         "SetRectangle: missing property in rectangle object")
            .Value());
    return deferred.Promise();
  }

  int32_t left = maybe_left.As<Napi::Number>().Int32Value();
  int32_t top = maybe_top.As<Napi::Number>().Int32Value();
  int32_t width = maybe_width.As<Napi::Number>().Int32Value();
  int32_t height = maybe_height.As<Napi::Number>().Int32Value();

  command.left = left;
  command.top = top;
  command.width = width;
  command.height = height;

  return _worker_thread.Enqueue(command);
}

Napi::Value
TesseractWrapper::SetSourceResolution(const Napi::CallbackInfo &info) {
  Napi::Env env = info.Env();
  Napi::Promise::Deferred deferred = Napi::Promise::Deferred::New(env);
  CommandSetSourceResolution command{};

  if (info.Length() > 1) {
    deferred.Reject(
        Napi::Error::New(env, "Expected only one parameter").Value());
    return deferred.Promise();
  }

  if (!info[0].IsNumber()) {
    deferred.Reject(
        Napi::Error::New(env, "Expected ppi to be of type number").Value());
    return deferred.Promise();
  }

  int ppi = info[0].As<Napi::Number>().Int32Value();

  command.ppi = ppi;

  return _worker_thread.Enqueue(command);
}

Napi::Value TesseractWrapper::Recognize(const Napi::CallbackInfo &info) {
  Napi::Env env = info.Env();
  Napi::Promise::Deferred deferred = Napi::Promise::Deferred::New(env);
  CommandRecognize command{};

  if (!info[0].IsUndefined()) {
    if (!info[0].IsFunction()) {
      deferred.Reject(
          Napi::Error::New(env,
                           "Expected progress callback to be of type function")
              .Value());
      return deferred.Promise();
    }

    Napi::Function progress_callback = info[0].As<Napi::Function>();
    Napi::ThreadSafeFunction progress_tsfn = Napi::ThreadSafeFunction::New(
        env, progress_callback, "tesseract_progress_callback", 0, 1);

    command.monitor_context =
        std::make_shared<MonitorContext>(std::move(progress_tsfn));
  }

  return _worker_thread.Enqueue(command);
}

Napi::Value
TesseractWrapper::DetectOrientationScript(const Napi::CallbackInfo &info) {
  return _worker_thread.Enqueue(CommandDetectOrientationScript{});
}

Napi::Value TesseractWrapper::MeanTextConf(const Napi::CallbackInfo &info) {
  return _worker_thread.Enqueue(CommandMeanTextConf{});
}

Napi::Value TesseractWrapper::GetUTF8Text(const Napi::CallbackInfo &info) {
  return _worker_thread.Enqueue(CommandGetUTF8Text{});
}

Napi::Value TesseractWrapper::GetHOCRText(const Napi::CallbackInfo &info) {
  Napi::Env env = info.Env();
  Napi::Promise::Deferred deferred = Napi::Promise::Deferred::New(env);
  CommandGetHOCRText command{};

  if (!info[0].IsUndefined()) {
    if (!info[0].IsFunction()) {
      deferred.Reject(
          Napi::Error::New(env,
                           "Expected progress callback to be of type function")
              .Value());
      return deferred.Promise();
    }

    Napi::Function progress_callback = info[0].As<Napi::Function>();
    Napi::ThreadSafeFunction progress_tsfn = Napi::ThreadSafeFunction::New(
        env, progress_callback, "tesseract_progress_callback", 0, 1);

    command.monitor_context =
        std::make_shared<MonitorContext>(std::move(progress_tsfn));
  }

  if (!info[1].IsUndefined()) {
    if (!info[1].IsNumber()) {
      deferred.Reject(
          Napi::Error::New(env, "Expected page_number to be of type number")
              .Value());
      return deferred.Promise();
    }

    int32_t page_number = info[1].As<Napi::Number>().Int32Value();
    command.page_number = page_number;
  }

  return _worker_thread.Enqueue(command);
}

Napi::Value TesseractWrapper::GetTSVText(const Napi::CallbackInfo &info) {
  Napi::Env env = info.Env();
  Napi::Promise::Deferred deferred = Napi::Promise::Deferred::New(env);
  CommandGetTSVText command{};

  if (!info[0].IsUndefined()) {
    if (!info[0].IsNumber()) {
      deferred.Reject(
          Napi::Error::New(env, "Expected page_number to be of type number")
              .Value());
      return deferred.Promise();
    }

    int32_t page_number = info[0].As<Napi::Number>().Int32Value();
    command.page_number = page_number;
  }

  return _worker_thread.Enqueue(command);
}

Napi::Value TesseractWrapper::GetUNLVText(const Napi::CallbackInfo &info) {
  return _worker_thread.Enqueue(CommandGetUNLVText{});
}

Napi::Value TesseractWrapper::GetALTOText(const Napi::CallbackInfo &info) {
  Napi::Env env = info.Env();
  Napi::Promise::Deferred deferred = Napi::Promise::Deferred::New(env);
  CommandGetALTOText command{};

  if (!info[0].IsUndefined()) {
    if (!info[0].IsNumber()) {
      deferred.Reject(
          Napi::Error::New(env, "Expected page_number to be of type number")
              .Value());
      return deferred.Promise();
    }

    int32_t page_number = info[0].As<Napi::Number>().Int32Value();
    command.page_number = page_number;
  }

  return _worker_thread.Enqueue(command);
}

Napi::Value TesseractWrapper::GetInitLanguages(const Napi::CallbackInfo &info) {
  return _worker_thread.Enqueue(CommandGetInitLanguages{});
}

Napi::Value
TesseractWrapper::GetLoadedLanguages(const Napi::CallbackInfo &info) {
  return _worker_thread.Enqueue(CommandGetLoadedLanguages{});
}

Napi::Value
TesseractWrapper::GetAvailableLanguages(const Napi::CallbackInfo &info) {
  return _worker_thread.Enqueue(CommandGetAvailableLanguages{});
}

Napi::Value TesseractWrapper::Clear(const Napi::CallbackInfo &info) {
  return _worker_thread.Enqueue(CommandClear{});
}

Napi::Value TesseractWrapper::End(const Napi::CallbackInfo &info) {
  return _worker_thread.Enqueue(CommandEnd{});
}
