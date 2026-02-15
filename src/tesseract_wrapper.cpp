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
#include <iostream>
#include <leptonica/allheaders.h>
#include <string>
#include <tesseract/publictypes.h>

Napi::FunctionReference TesseractWrapper::constructor;

namespace {

Napi::Value RejectWithError(Napi::Env env, Napi::Error error, const char *code,
                            const std::string &message, const char *method) {
  error.Set("code", Napi::String::New(env, code));
  error.Set("method", Napi::String::New(env, method));

  Napi::Promise::Deferred deferred = Napi::Promise::Deferred::New(env);
  deferred.Reject(error.Value());
  return deferred.Promise();
}

Napi::Value RejectError(Napi::Env env, const std::string &message,
                        const char *method) {
  return RejectWithError(env, Napi::Error::New(env, message),
                         "ERR_TESSERACT_RUNTIME", message, method);
}

Napi::Value RejectTypeError(Napi::Env env, const std::string &message,
                            const char *method) {
  return RejectWithError(env, Napi::TypeError::New(env, message),
                         "ERR_INVALID_ARGUMENT", message, method);
}

Napi::Value RejectRangeError(Napi::Env env, const std::string &message,
                             const char *method) {
  return RejectWithError(env, Napi::RangeError::New(env, message),
                         "ERR_OUT_OF_RANGE", message, method);
}

bool HasArg(const Napi::CallbackInfo &info, size_t index) {
  return info.Length() > index && !info[index].IsUndefined();
}

} // namespace

Napi::Object TesseractWrapper::InitAddon(Napi::Env env, Napi::Object exports) {
  Napi::Function func = DefineClass(
      env, "Tesseract",
      {
          InstanceMethod("version", &TesseractWrapper::Version),
          InstanceMethod("isInitialized", &TesseractWrapper::IsInitialized),
          InstanceMethod("setInputName", &TesseractWrapper::SetInputName),
          InstanceMethod("getInputName", &TesseractWrapper::GetInputName),
          InstanceMethod("setInputImage", &TesseractWrapper::SetInputImage),
          InstanceMethod("getInputImage", &TesseractWrapper::GetInputImage),
          InstanceMethod("getSourceYResolution",
                         &TesseractWrapper::GetSourceYResolution),
          InstanceMethod("getDataPath", &TesseractWrapper::GetDataPath),
          InstanceMethod("setOutputName", &TesseractWrapper::SetOutputName),
          InstanceMethod("clearPersistentCache",
                         &TesseractWrapper::ClearPersistentCache),
          InstanceMethod("clearAdaptiveClassifier",
                         &TesseractWrapper::ClearAdaptiveClassifier),
          InstanceMethod("getThresholdedImage",
                         &TesseractWrapper::GetThresholdedImage),
          InstanceMethod("getThresholdedImageScaleFactor",
                         &TesseractWrapper::GetThresholdedImageScaleFactor),
          InstanceMethod("init", &TesseractWrapper::Init),
          InstanceMethod("initForAnalysePage",
                         &TesseractWrapper::InitForAnalysePage),
          InstanceMethod("analyseLayout", &TesseractWrapper::AnalyseLayout),
          InstanceMethod("beginProcessPages",
                         &TesseractWrapper::BeginProcessPages),
          InstanceMethod("addProcessPage", &TesseractWrapper::AddProcessPage),
          InstanceMethod("finishProcessPages",
                         &TesseractWrapper::FinishProcessPages),
          InstanceMethod("abortProcessPages",
                         &TesseractWrapper::AbortProcessPages),
          InstanceMethod("getProcessPagesStatus",
                         &TesseractWrapper::GetProcessPagesStatus),
          InstanceMethod("setDebugVariable",
                         &TesseractWrapper::SetDebugVariable),
          InstanceMethod("setVariable", &TesseractWrapper::SetVariable),
          InstanceMethod("getIntVariable", &TesseractWrapper::GetIntVariable),
          InstanceMethod("getBoolVariable", &TesseractWrapper::GetBoolVariable),
          InstanceMethod("getDoubleVariable",
                         &TesseractWrapper::GetDoubleVariable),
          InstanceMethod("getStringVariable",
                         &TesseractWrapper::GetStringVariable),
          InstanceMethod("setImage", &TesseractWrapper::SetImage),
          // InstanceMethod("printVariables",
          // &TesseractWrapper::PrintVariables),
          InstanceMethod("setPageMode", &TesseractWrapper::SetPageMode),
          InstanceMethod("setRectangle", &TesseractWrapper::SetRectangle),
          InstanceMethod("setSourceResolution",
                         &TesseractWrapper::SetSourceResolution),
          InstanceMethod("recognize", &TesseractWrapper::Recognize),
          InstanceMethod("detectOrientationScript",
                         &TesseractWrapper::DetectOrientationScript),
          InstanceMethod("meanTextConf", &TesseractWrapper::MeanTextConf),
          InstanceMethod("allWordConfidences",
                         &TesseractWrapper::AllWordConfidences),
          InstanceMethod("getPAGEText", &TesseractWrapper::GetPAGEText),
          InstanceMethod("getLSTMBoxText", &TesseractWrapper::GetLSTMBoxText),
          InstanceMethod("getBoxText", &TesseractWrapper::GetBoxText),
          InstanceMethod("getWordStrBoxText",
                         &TesseractWrapper::GetWordStrBoxText),
          InstanceMethod("getOSDText", &TesseractWrapper::getOSDText),
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

Napi::Value TesseractWrapper::Version(const Napi::CallbackInfo &info) {
  return _worker_thread.Enqueue(CommandVersion{});
}

Napi::Value TesseractWrapper::IsInitialized(const Napi::CallbackInfo &info) {
  Napi::Env env = info.Env();

  if (info.Length() > 0) {
    return RejectTypeError(env, "isInitialized(): expected no arguments",
                           "isInitialized");
  }

  return _worker_thread.Enqueue(CommandIsInitialized{});
}

Napi::Value TesseractWrapper::SetInputName(const Napi::CallbackInfo &info) {
  Napi::Env env = info.Env();

  if (info.Length() > 1) {
    return RejectTypeError(
        env, "setInputName(inputName?): expected at most 1 argument",
        "setInputName");
  }

  CommandSetInputName command{};
  if (HasArg(info, 0)) {
    if (!info[0].IsString()) {
      return RejectTypeError(
          env, "setInputName(inputName?): inputName must be a string",
          "setInputName");
    }

    command.input_name = info[0].As<Napi::String>().Utf8Value();
  }

  return _worker_thread.Enqueue(command);
}

Napi::Value TesseractWrapper::GetInputName(const Napi::CallbackInfo &info) {
  return _worker_thread.Enqueue(CommandGetInputName{});
}

Napi::Value TesseractWrapper::SetInputImage(const Napi::CallbackInfo &info) {
  Napi::Env env = info.Env();

  if (info.Length() > 1) {
    return RejectTypeError(
        env, "setInputImage(buffer?): expected at most 1 argument",
        "setInputImage");
  }

  CommandSetInputImage command{};
  if (HasArg(info, 0)) {
    if (!info[0].IsBuffer()) {
      return RejectTypeError(env,
                             "setInputImage(buffer?): buffer must be a Buffer",
                             "setInputImage");
    }

    Napi::Buffer<uint8_t> image_buffer = info[0].As<Napi::Buffer<uint8_t>>();
    const uint8_t *data = image_buffer.Data();
    const size_t length = image_buffer.Length();

    if (length == 0) {
      return RejectTypeError(env, "setInputImage(buffer?): buffer is empty",
                             "setInputImage");
    }

    command.bytes.assign(data, data + length);
  }

  return _worker_thread.Enqueue(command);
}

Napi::Value TesseractWrapper::GetInputImage(const Napi::CallbackInfo &info) {
  Napi::Env env = info.Env();

  if (info.Length() > 0) {
    return RejectTypeError(env, "getInputImage(): expected no arguments",
                           "getInputImage");
  }

  return _worker_thread.Enqueue(CommandGetInputImage{});
}

Napi::Value
TesseractWrapper::GetSourceYResolution(const Napi::CallbackInfo &info) {
  Napi::Env env = info.Env();

  if (info.Length() > 0) {
    return RejectTypeError(env, "getSourceYResolution(): expected no arguments",
                           "getSourceYResolution");
  }

  return _worker_thread.Enqueue(CommandGetSourceYResolution{});
}

Napi::Value TesseractWrapper::GetDataPath(const Napi::CallbackInfo &info) {
  Napi::Env env = info.Env();

  if (info.Length() > 0) {
    return RejectTypeError(env, "getDataPath(): expected no arguments",
                           "getDataPath");
  }

  return _worker_thread.Enqueue(CommandGetDataPath{});
}

Napi::Value TesseractWrapper::SetOutputName(const Napi::CallbackInfo &info) {
  Napi::Env env = info.Env();
  CommandSetOutputName command{};

  if (info.Length() != 1 || !info[0].IsString()) {
    return RejectTypeError(
        env, "setOutputName(outputName): outputName must be a string",
        "setOutputName");
  }

  command.output_name = info[0].As<Napi::String>().Utf8Value();
  return _worker_thread.Enqueue(command);
}

Napi::Value
TesseractWrapper::ClearPersistentCache(const Napi::CallbackInfo &info) {
  Napi::Env env = info.Env();

  if (info.Length() > 0) {
    return RejectTypeError(env, "clearPersistentCache(): expected no arguments",
                           "clearPersistentCache");
  }

  return _worker_thread.Enqueue(CommandClearPersistentCache{});
}

Napi::Value
TesseractWrapper::ClearAdaptiveClassifier(const Napi::CallbackInfo &info) {
  Napi::Env env = info.Env();

  if (info.Length() > 0) {
    return RejectTypeError(env,
                           "clearAdaptiveClassifier(): expected no arguments",
                           "clearAdaptiveClassifier");
  }

  return _worker_thread.Enqueue(CommandClearAdaptiveClassifier{});
}

Napi::Value
TesseractWrapper::GetThresholdedImage(const Napi::CallbackInfo &info) {
  Napi::Env env = info.Env();

  if (info.Length() > 0) {
    return RejectTypeError(env, "getThresholdedImage(): expected no arguments",
                           "getThresholdedImage");
  }

  return _worker_thread.Enqueue(CommandGetThresholdedImage{});
}

Napi::Value TesseractWrapper::GetThresholdedImageScaleFactor(
    const Napi::CallbackInfo &info) {
  Napi::Env env = info.Env();

  if (info.Length() > 0) {
    return RejectTypeError(
        env, "getThresholdedImageScaleFactor(): expected no arguments",
        "getThresholdedImageScaleFactor");
  }

  return _worker_thread.Enqueue(CommandGetThresholdedImageScaleFactor{});
}

Napi::Value TesseractWrapper::Init(const Napi::CallbackInfo &info) {
  Napi::Env env = info.Env();

  if (info.Length() != 1 || !info[0].IsObject()) {
    return RejectTypeError(
        env, "init(options): required argument at index 0 must be an object",
        "init");
  }

  auto options = info[0].As<Napi::Object>();
  CommandInit command{};

  const Napi::Value dataPathOption = options.Get("dataPath");
  if (!dataPathOption.IsUndefined()) {
    if (!dataPathOption.IsString()) {
      return RejectTypeError(
          env, "init(options): options.dataPath must be a string", "init");
    }

    Napi::String dataPath = dataPathOption.As<Napi::String>();
    command.data_path = dataPath.Utf8Value();
  }

  const Napi::Value langsOption = options.Get("langs");
  if (!langsOption.IsUndefined()) {
    if (!langsOption.IsArray()) {
      return RejectTypeError(
          env, "init(options): options.langs must be an array of strings",
          "init");
    }

    Napi::Array languages = langsOption.As<Napi::Array>();
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
      return RejectTypeError(env, "init(options): options.oem must be a number",
                             "init");
    }
    tesseract::OcrEngineMode oem = static_cast<tesseract::OcrEngineMode>(
        engineModeOption.As<Napi::Number>().Int32Value());

    if (oem < 0 || oem >= tesseract::OEM_COUNT) {
      return RejectRangeError(
          env, "init(options): options.oem is out of supported range", "init");
    }

    command.oem = oem;
  }

  const Napi::Value set_only_non_debug_params =
      options.Get("setOnlyNonDebugParams");
  if (!set_only_non_debug_params.IsUndefined()) {
    if (!set_only_non_debug_params.IsBoolean()) {
      return RejectTypeError(
          env, "init(options): options.setOnlyNonDebugParams must be a boolean",
          "init");
    }

    command.set_only_non_debug_params =
        set_only_non_debug_params.As<Napi::Boolean>().Value();
  }

  const Napi::Value v = options.Get("configs");
  if (!v.IsUndefined()) {
    if (!v.IsArray()) {
      return RejectTypeError(
          env, "init(options): options.configs must be an array of strings",
          "init");
    }

    Napi::Array arr = v.As<Napi::Array>();
    const uint32_t len = arr.Length();

    command.configs_storage.reserve(len);
    command.configs.reserve(len);

    for (uint32_t i = 0; i < len; ++i) {
      Napi::Value item = arr.Get(i);
      if (!item.IsString()) {
        return RejectTypeError(
            env, "init(options): options.configs must contain only strings",
            "init");
      }
      command.configs_storage.emplace_back(item.As<Napi::String>().Utf8Value());
    }

    // after storage is stable, build char* array
    for (auto &s : command.configs_storage) {
      command.configs.push_back(const_cast<char *>(s.c_str()));
    }
  }

  const Napi::Value varsOption = options.Get("vars");

  if (!varsOption.IsUndefined()) {
    if (!varsOption.IsObject()) {
      return RejectTypeError(
          env, "init(options): options.vars must be an object", "init");
    }

    // Napi::Array a = vars_vec.As<Napi::Array>();
    // Napi::Array b = vars_values.As<Napi::Array>();
    Napi::Object vars = varsOption.As<Napi::Object>();
    Napi::Array variable_names = vars.GetPropertyNames();

    const uint32_t length = variable_names.Length();
    command.vars_vec.reserve(length);
    command.vars_values.reserve(length);

    for (uint32_t i = 0; i < length; ++i) {
      Napi::Value variable_value = vars.Get(variable_names.Get(i));
      if (!variable_names.Get(i).IsString() || !variable_value.IsString()) {
        return RejectTypeError(
            env, "init(options): options.vars must contain only strings",
            "init");
      }
      command.vars_vec.emplace_back(
          variable_names.Get(i).As<Napi::String>().Utf8Value());
      command.vars_values.emplace_back(
          variable_value.As<Napi::String>().Utf8Value());
    }
  }

  return _worker_thread.Enqueue(command);
}

Napi::Value
TesseractWrapper::InitForAnalysePage(const Napi::CallbackInfo &info) {
  return _worker_thread.Enqueue(CommandInitForAnalysePage{});
}

Napi::Value TesseractWrapper::AnalyseLayout(const Napi::CallbackInfo &info) {
  Napi::Env env = info.Env();
  CommandAnalyseLayout command{};

  if (info.Length() > 1) {
    return RejectTypeError(
        env, "analyseLayout(mergeSimilarWords?): expected at most 1 argument",
        "analyseLayout");
  }

  if (HasArg(info, 0)) {
    if (!info[0].IsBoolean()) {
      return RejectTypeError(env,
                             "analyseLayout(mergeSimilarWords?): "
                             "mergeSimilarWords must be a boolean",
                             "analyseLayout");
    }

    command.merge_similar_words = info[0].As<Napi::Boolean>().Value();
  }

  return _worker_thread.Enqueue(command);
}

Napi::Value
TesseractWrapper::BeginProcessPages(const Napi::CallbackInfo &info) {
  Napi::Env env = info.Env();
  CommandBeginProcessPages command{};

  if (info.Length() != 1 || !info[0].IsObject()) {
    return RejectTypeError(
        env, "beginProcessPages(options): options must be an object",
        "beginProcessPages");
  }

  Napi::Object options = info[0].As<Napi::Object>();

  Napi::Value output_base = options.Get("outputBase");
  if (!output_base.IsUndefined()) {
    if (!output_base.IsString()) {
      return RejectTypeError(
          env,
          "beginProcessPages(options): options.outputBase must be a string",
          "beginProcessPages");
    }
    command.output_base = output_base.As<Napi::String>().Utf8Value();
  }

  Napi::Value title = options.Get("title");
  if (title.IsUndefined() || !title.IsString()) {
    return RejectTypeError(env,
                           "beginProcessPages(options): options.title is "
                           "required and must be a string",
                           "beginProcessPages");
  }
  command.title = title.As<Napi::String>().Utf8Value();

  Napi::Value timeout = options.Get("timeout");
  if (!timeout.IsUndefined()) {
    if (!timeout.IsNumber()) {
      return RejectTypeError(
          env, "beginProcessPages(options): options.timeout must be a number",
          "beginProcessPages");
    }
    command.timeout_millisec = timeout.As<Napi::Number>().Int32Value();
  }

  Napi::Value textonly = options.Get("textonly");
  if (!textonly.IsUndefined()) {
    if (!textonly.IsBoolean()) {
      return RejectTypeError(
          env, "beginProcessPages(options): options.textonly must be a boolean",
          "beginProcessPages");
    }
    command.textonly = textonly.As<Napi::Boolean>().Value();
  }

  return _worker_thread.Enqueue(std::move(command));
}

Napi::Value TesseractWrapper::AddProcessPage(const Napi::CallbackInfo &info) {
  Napi::Env env = info.Env();
  CommandAddProcessPage command{};

  if (info.Length() != 1 || !info[0].IsObject()) {
    return RejectTypeError(env,
                           "addProcessPage(options): options must be an object",
                           "addProcessPage");
  }

  Napi::Object options = info[0].As<Napi::Object>();

  Napi::Value buffer_value = options.Get("buffer");
  if (!buffer_value.IsBuffer()) {
    return RejectTypeError(
        env, "addProcessPage(options): options.buffer must be a Buffer",
        "addProcessPage");
  }

  Napi::Buffer<uint8_t> page_buffer = buffer_value.As<Napi::Buffer<uint8_t>>();
  const size_t length = page_buffer.Length();
  if (length == 0) {
    return RejectTypeError(env,
                           "addProcessPage(options): options.buffer is empty",
                           "addProcessPage");
  }

  Napi::Value filename_value = options.Get("filename");
  if (!filename_value.IsUndefined() && !filename_value.IsNull()) {
    if (!filename_value.IsString()) {
      return RejectTypeError(
          env, "addProcessPage(options): options.filename must be a string",
          "addProcessPage");
    }
    command.filename = filename_value.As<Napi::String>().Utf8Value();
  }

  Napi::Value progress_callback_value = options.Get("progressCallback");
  if (!progress_callback_value.IsUndefined() &&
      !progress_callback_value.IsNull()) {
    if (!progress_callback_value.IsFunction()) {
      return RejectTypeError(env,
                             "addProcessPage(options): "
                             "options.progressCallback must be a function",
                             "addProcessPage");
    }

    Napi::Function progress_callback =
        progress_callback_value.As<Napi::Function>();
    Napi::ThreadSafeFunction progress_tsfn = Napi::ThreadSafeFunction::New(
        env, progress_callback, "tesseract_progress_callback", 0, 1);
    command.monitor_context =
        std::make_shared<MonitorContext>(std::move(progress_tsfn));
  }

  command.page.bytes.resize(length);
  std::memcpy(command.page.bytes.data(), page_buffer.Data(), length);

  return _worker_thread.Enqueue(std::move(command));
}

Napi::Value
TesseractWrapper::FinishProcessPages(const Napi::CallbackInfo &info) {
  return _worker_thread.Enqueue(CommandFinishProcessPages{});
}

Napi::Value
TesseractWrapper::AbortProcessPages(const Napi::CallbackInfo &info) {
  CommandAbortProcessPages command{};
  if (info.Length() > 0 && info[0].IsString()) {
    command.reason = info[0].As<Napi::String>().Utf8Value();
  }
  return _worker_thread.Enqueue(std::move(command));
}

Napi::Value
TesseractWrapper::GetProcessPagesStatus(const Napi::CallbackInfo &info) {
  Napi::Env env = info.Env();

  if (info.Length() > 0) {
    return RejectTypeError(env,
                           "getProcessPagesStatus(): expected no arguments",
                           "getProcessPagesStatus");
  }

  return _worker_thread.Enqueue(CommandGetProcessPagesStatus{});
}

Napi::Value TesseractWrapper::SetDebugVariable(const Napi::CallbackInfo &info) {
  Napi::Env env = info.Env();
  CommandSetDebugVariable command{};

  if (info.Length() != 2) {
    return RejectTypeError(
        env, "setDebugVariable(name, value): expected exactly 2 arguments",
        "setDebugVariable");
  }

  if (!info[0].IsString()) {
    return RejectTypeError(
        env, "setDebugVariable(name, value): name must be a string",
        "setDebugVariable");
  }
  if (!info[1].IsString()) {
    return RejectTypeError(
        env, "setDebugVariable(name, value): value must be a string",
        "setDebugVariable");
  }

  command.name = info[0].As<Napi::String>().Utf8Value();
  command.value = info[1].As<Napi::String>().Utf8Value();

  return _worker_thread.Enqueue(command);
}

Napi::Value TesseractWrapper::SetVariable(const Napi::CallbackInfo &info) {
  Napi::Env env = info.Env();
  CommandSetVariable command{};

  if (info.Length() != 2) {
    return RejectTypeError(
        env, "setVariable(name, value): expected exactly 2 arguments",
        "setVariable");
  }

  if (!info[0].IsString()) {
    return RejectTypeError(
        env, "setVariable(name, value): name must be a string", "setVariable");
  }
  if (!info[1].IsString()) {
    return RejectTypeError(
        env, "setVariable(name, value): value must be a string", "setVariable");
  }

  const std::string variable_name = info[0].As<Napi::String>().Utf8Value();
  const std::string variable_value = info[1].As<Napi::String>().Utf8Value();

  command.name = variable_name;
  command.value = variable_value;

  return _worker_thread.Enqueue(command);
}

Napi::Value TesseractWrapper::GetIntVariable(const Napi::CallbackInfo &info) {
  Napi::Env env = info.Env();
  CommandGetIntVariable command{};

  if (info.Length() != 1) {
    return RejectTypeError(env,
                           "getIntVariable(name): expected exactly 1 argument",
                           "getIntVariable");
  }

  if (!info[0].IsString()) {
    return RejectTypeError(env, "getIntVariable(name): name must be a string",
                           "getIntVariable");
  }

  std::string name = info[0].As<Napi::String>().Utf8Value();
  command.name = name;

  return _worker_thread.Enqueue(command);
}

Napi::Value TesseractWrapper::GetBoolVariable(const Napi::CallbackInfo &info) {
  Napi::Env env = info.Env();
  CommandGetBoolVariable command{};

  if (info.Length() != 1) {
    return RejectTypeError(env,
                           "getBoolVariable(name): expected exactly 1 argument",
                           "getBoolVariable");
  }

  if (!info[0].IsString()) {
    return RejectTypeError(env, "getBoolVariable(name): name must be a string",
                           "getBoolVariable");
  }

  std::string name = info[0].As<Napi::String>().Utf8Value();
  command.name = name;

  return _worker_thread.Enqueue(command);
}

Napi::Value
TesseractWrapper::GetDoubleVariable(const Napi::CallbackInfo &info) {
  Napi::Env env = info.Env();
  CommandGetDoubleVariable command{};

  if (info.Length() != 1) {
    return RejectTypeError(
        env, "getDoubleVariable(name): expected exactly 1 argument",
        "getDoubleVariable");
  }

  if (!info[0].IsString()) {
    return RejectTypeError(env,
                           "getDoubleVariable(name): name must be a string",
                           "getDoubleVariable");
  }

  std::string name = info[0].As<Napi::String>().Utf8Value();
  command.name = name;

  return _worker_thread.Enqueue(command);
}

Napi::Value
TesseractWrapper::GetStringVariable(const Napi::CallbackInfo &info) {
  Napi::Env env = info.Env();
  CommandGetStringVariable command{};

  if (info.Length() != 1) {
    return RejectTypeError(
        env, "getStringVariable(name): expected exactly 1 argument",
        "getStringVariable");
  }

  if (!info[0].IsString()) {
    return RejectTypeError(env,
                           "getStringVariable(name): name must be a string",
                           "getStringVariable");
  }

  std::string name = info[0].As<Napi::String>().Utf8Value();
  command.name = name;

  return _worker_thread.Enqueue(command);
}

Napi::Value TesseractWrapper::SetImage(const Napi::CallbackInfo &info) {
  Napi::Env env = info.Env();
  CommandSetImage command{};

  if (info.Length() < 1 || !info[0].IsBuffer()) {
    return RejectTypeError(env, "setImage(buffer): buffer argument is required",
                           "setImage");
  }

  Napi::Buffer<uint8_t> image_buffer = info[0].As<Napi::Buffer<uint8_t>>();
  const uint8_t *data = image_buffer.Data();
  const size_t length = image_buffer.Length();

  if (length == 0) {
    return RejectTypeError(env, "setImage(buffer): buffer is empty",
                           "setImage");
  }

  Pix *pix = pixReadMem(data, length);
  if (!pix) {
    return RejectError(env, "setImage(buffer): failed to decode image buffer",
                       "setImage");
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
    return RejectError(env, "setImage(buffer): invalid decoded image data",
                       "setImage");
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

Napi::Value TesseractWrapper::SetPageMode(const Napi::CallbackInfo &info) {
  Napi::Env env = info.Env();
  CommandSetPageMode command{};

  if (info.Length() > 1) {
    return RejectTypeError(
        env, "setPageMode(psm?): expected at most 1 argument", "setPageMode");
  }

  if (HasArg(info, 0)) {
    if (!info[0].IsNumber()) {
      return RejectTypeError(env, "setPageMode(psm?): psm must be a number",
                             "setPageMode");
    }

    tesseract::PageSegMode psm = static_cast<tesseract::PageSegMode>(
        info[0].As<Napi::Number>().Int32Value());

    if (psm < 0 || psm >= tesseract::PageSegMode::PSM_COUNT) {
      return RejectRangeError(env, "setPageMode(psm?): psm is out of range",
                              "setPageMode");
    }

    command.psm = psm;
  }

  return _worker_thread.Enqueue(command);
}

Napi::Value TesseractWrapper::SetRectangle(const Napi::CallbackInfo &info) {
  Napi::Env env = info.Env();
  CommandSetRectangle command{};

  if (info.Length() != 1 || !info[0].IsObject()) {
    return RejectTypeError(
        env, "setRectangle(rectangle): rectangle must be an object",
        "setRectangle");
  }

  Napi::Object rectangle = info[0].As<Napi::Object>();

  Napi::Value maybe_left = rectangle.Get("left");
  Napi::Value maybe_top = rectangle.Get("top");
  Napi::Value maybe_width = rectangle.Get("width");
  Napi::Value maybe_height = rectangle.Get("height");

  if (!maybe_left.IsNumber() || !maybe_top.IsNumber() ||
      !maybe_width.IsNumber() || !maybe_height.IsNumber()) {
    return RejectTypeError(env,
                           "setRectangle(rectangle): "
                           "rectangle.left/top/width/height must be numbers",
                           "setRectangle");
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
  CommandSetSourceResolution command{};

  if (info.Length() != 1) {
    return RejectTypeError(
        env, "setSourceResolution(ppi): expected exactly 1 argument",
        "setSourceResolution");
  }

  if (!info[0].IsNumber()) {
    return RejectTypeError(env,
                           "setSourceResolution(ppi): ppi must be a number",
                           "setSourceResolution");
  }

  int ppi = info[0].As<Napi::Number>().Int32Value();

  command.ppi = ppi;

  return _worker_thread.Enqueue(command);
}

Napi::Value TesseractWrapper::Recognize(const Napi::CallbackInfo &info) {
  Napi::Env env = info.Env();
  CommandRecognize command{};

  if (HasArg(info, 0)) {
    if (!info[0].IsFunction()) {
      return RejectTypeError(
          env,
          "recognize(progressCallback?): progressCallback must be a function",
          "recognize");
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

Napi::Value
TesseractWrapper::AllWordConfidences(const Napi::CallbackInfo &info) {
  return _worker_thread.Enqueue(CommandAllWordConfidences{});
}

Napi::Value TesseractWrapper::GetPAGEText(const Napi::CallbackInfo &info) {
  Napi::Env env = info.Env();
  CommandGetPAGEText command{};
  command.page_number = 0;

  if (HasArg(info, 0)) {
    if (!info[0].IsFunction()) {
      return RejectTypeError(env,
                             "getPAGEText(progressCallback?, pageNumber?): "
                             "progressCallback must be a function",
                             "getPAGEText");
    }

    Napi::Function progress_callback = info[0].As<Napi::Function>();
    Napi::ThreadSafeFunction progress_tsfn = Napi::ThreadSafeFunction::New(
        env, progress_callback, "tesseract_progress_callback", 0, 1);

    command.monitor_context =
        std::make_shared<MonitorContext>(std::move(progress_tsfn));
  }

  if (HasArg(info, 1)) {
    if (!info[1].IsNumber()) {
      return RejectTypeError(env,
                             "getPAGEText(progressCallback?, pageNumber?): "
                             "pageNumber must be a number",
                             "getPAGEText");
    }

    command.page_number = info[1].As<Napi::Number>().Int32Value();
  }

  return _worker_thread.Enqueue(command);
}

Napi::Value TesseractWrapper::GetLSTMBoxText(const Napi::CallbackInfo &info) {
  Napi::Env env = info.Env();
  CommandGetLSTMBoxText command{};
  command.page_number = 0;

  if (HasArg(info, 0)) {
    if (!info[0].IsNumber()) {
      return RejectTypeError(
          env, "getLSTMBoxText(pageNumber?): pageNumber must be a number",
          "getLSTMBoxText");
    }
    command.page_number = info[0].As<Napi::Number>().Int32Value();
  }

  return _worker_thread.Enqueue(command);
}

Napi::Value TesseractWrapper::GetBoxText(const Napi::CallbackInfo &info) {
  Napi::Env env = info.Env();
  CommandGetBoxText command{};
  command.page_number = 0;

  if (HasArg(info, 0)) {
    if (!info[0].IsNumber()) {
      return RejectTypeError(
          env, "getBoxText(pageNumber?): pageNumber must be a number",
          "getBoxText");
    }
    command.page_number = info[0].As<Napi::Number>().Int32Value();
  }

  return _worker_thread.Enqueue(command);
}

Napi::Value
TesseractWrapper::GetWordStrBoxText(const Napi::CallbackInfo &info) {
  Napi::Env env = info.Env();
  CommandGetWordStrBoxText command{};
  command.page_number = 0;

  if (HasArg(info, 0)) {
    if (!info[0].IsNumber()) {
      return RejectTypeError(
          env, "getWordStrBoxText(pageNumber?): pageNumber must be a number",
          "getWordStrBoxText");
    }
    command.page_number = info[0].As<Napi::Number>().Int32Value();
  }

  return _worker_thread.Enqueue(command);
}

Napi::Value TesseractWrapper::getOSDText(const Napi::CallbackInfo &info) {
  Napi::Env env = info.Env();
  CommandGetOSDText command{};
  command.page_number = 0;

  if (HasArg(info, 0)) {
    if (!info[0].IsNumber()) {
      return RejectTypeError(
          env, "getOSDText(pageNumber?): pageNumber must be a number",
          "getOSDText");
    }
    command.page_number = info[0].As<Napi::Number>().Int32Value();
  }

  return _worker_thread.Enqueue(command);
}

Napi::Value TesseractWrapper::GetUTF8Text(const Napi::CallbackInfo &info) {
  return _worker_thread.Enqueue(CommandGetUTF8Text{});
}

Napi::Value TesseractWrapper::GetHOCRText(const Napi::CallbackInfo &info) {
  Napi::Env env = info.Env();
  CommandGetHOCRText command{};

  if (HasArg(info, 0)) {
    if (!info[0].IsFunction()) {
      return RejectTypeError(env,
                             "getHOCRText(progressCallback?, pageNumber?): "
                             "progressCallback must be a function",
                             "getHOCRText");
    }

    Napi::Function progress_callback = info[0].As<Napi::Function>();
    Napi::ThreadSafeFunction progress_tsfn = Napi::ThreadSafeFunction::New(
        env, progress_callback, "tesseract_progress_callback", 0, 1);

    command.monitor_context =
        std::make_shared<MonitorContext>(std::move(progress_tsfn));
  }

  if (HasArg(info, 1)) {
    if (!info[1].IsNumber()) {
      return RejectTypeError(env,
                             "getHOCRText(progressCallback?, pageNumber?): "
                             "pageNumber must be a number",
                             "getHOCRText");
    }

    int32_t page_number = info[1].As<Napi::Number>().Int32Value();
    command.page_number = page_number;
  }

  return _worker_thread.Enqueue(command);
}

Napi::Value TesseractWrapper::GetTSVText(const Napi::CallbackInfo &info) {
  Napi::Env env = info.Env();
  CommandGetTSVText command{};

  if (HasArg(info, 0)) {
    if (!info[0].IsNumber()) {
      return RejectTypeError(
          env, "getTSVText(pageNumber?): pageNumber must be a number",
          "getTSVText");
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
  CommandGetALTOText command{};

  if (HasArg(info, 0)) {
    if (!info[0].IsNumber()) {
      return RejectTypeError(
          env, "getALTOText(pageNumber?): pageNumber must be a number",
          "getALTOText");
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
