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

#pragma once

#include "monitor.hpp"
#include "utils.hpp"
#include <allheaders.h>
#include <atomic>
#include <cstddef>
#include <cstdint>
#include <exception>
#include <iostream>
#include <memory>
#include <napi.h>
#include <optional>
#include <ostream>
#include <string>
#include <tesseract/baseapi.h>
#include <tesseract/ocrclass.h>
#include <tesseract/publictypes.h>
#include <tesseract/renderer.h>
#include <unordered_map>
#include <variant>
#include <vector>

struct ResultVoid {};

struct ResultBool {
  bool value;
};

struct ResultInt {
  int value;
};

struct ResultDouble {
  double value;
};

struct ResultFloat {
  float value;
};

struct ResultString {
  std::string value;
};

struct ResultBuffer {
  std::vector<uint8_t> value;
};

using ObjectValue = std::variant<bool, int, double, float, std::string,
                                 std::vector<std::string>, std::vector<uint8_t>,
                                 std::vector<int>>;

struct ResultObject {
  std::unordered_map<std::string, ObjectValue> value;
};

using ArrayValue = std::variant<std::vector<int>, std::vector<std::string>>;

struct ResultArray {
  ArrayValue value;
};

using Result =
    std::variant<ResultVoid, ResultBool, ResultInt, ResultDouble, ResultFloat,
                 ResultString, ResultArray, ResultBuffer, ResultObject>;

template <class... Ts> struct match : Ts... {
  using Ts::operator()...;
};

template <class... Ts> match(Ts...) -> match<Ts...>;

template <typename T>
static Napi::Array VectorToNapiArray(Napi::Env env, const std::vector<T> &vec) {
  Napi::Array arr = Napi::Array::New(env, vec.size());
  for (size_t i = 0; i < vec.size(); ++i) {
    arr.Set(static_cast<uint32_t>(i), vec[i]);
  }
  return arr;
}

static Napi::Value ToNapiValue(Napi::Env env, const ObjectValue &v) {
  return std::visit(
      match{
          [&](bool b) -> Napi::Value { return Napi::Boolean::New(env, b); },
          [&](int i) -> Napi::Value { return Napi::Number::New(env, i); },
          [&](double d) -> Napi::Value { return Napi::Number::New(env, d); },
          [&](float f) -> Napi::Value { return Napi::Number::New(env, f); },
          [&](const std::string &s) -> Napi::Value { // String
            return Napi::String::New(env, s);
          },
          [&](const std::vector<uint8_t> &vec) -> Napi::Value { // Buffer
            return Napi::Buffer<uint8_t>::Copy(env, vec.data(), vec.size());
          },
          [&](const std::vector<int> &vec) -> Napi::Value {
            return VectorToNapiArray(env, vec);
          },
          [&](const std::vector<std::string> &vec)
              -> Napi::Value { // string array
            return VectorToNapiArray(env, vec);
          },
      },
      v);
}

inline Napi::Value MatchResult(Napi::Env env, const Result &r) {
  return std::visit(
      match{[&](const ResultVoid &) -> Napi::Value { return env.Undefined(); },
            [&](const ResultBool &v) -> Napi::Value {
              return Napi::Boolean::New(env, v.value);
            },
            [&](const ResultInt &v) -> Napi::Value {
              return Napi::Number::New(env, v.value);
            },
            [&](const ResultDouble &v) -> Napi::Value {
              return Napi::Number::New(env, v.value);
            },
            [&](const ResultFloat &v) -> Napi::Value {
              return Napi::Number::New(env, v.value);
            },
            [&](const ResultString &v) -> Napi::Value {
              return Napi::String::New(env, v.value);
            },
            [&](const ResultBuffer &v) -> Napi::Value {
              return Napi::Buffer<uint8_t>::Copy(env, v.value.data(),
                                                 v.value.size());
            },
            [&](const ResultArray &v) -> Napi::Value {
              return std::visit(
                  [&](const auto &vec) -> Napi::Value {
                    return VectorToNapiArray(env, vec);
                  },
                  v.value);
            },
            [&](const ResultObject &v) -> Napi::Value {
              Napi::Object obj = Napi::Object::New(env);

              for (const auto &[k, val] : v.value) {
                obj.Set(k, ToNapiValue(env, val));
              }
              return obj;
            }},
      r);
}

inline void RequireInitialized(const std::atomic<bool> &initialized,
                               const char *method) {
  if (!initialized.load(std::memory_order_acquire)) {
    throw_runtime("{}: call init(...) first", method);
  }
}

struct CommandVersion {
  Result invoke(tesseract::TessBaseAPI &api) const {
    return ResultString{api.Version()};
  }
};

struct CommandIsInitialized {
  Result invoke(tesseract::TessBaseAPI &,
                const std::atomic<bool> &initialized) const {
    return ResultBool{initialized.load(std::memory_order_acquire)};
  }
};

struct CommandSetInputName {
  std::string input_name;
  Result invoke(tesseract::TessBaseAPI &api) const {
    api.SetInputName(input_name.c_str());
    return ResultVoid{};
  }
};

struct CommandGetInputName {
  Result invoke(tesseract::TessBaseAPI &api) const {
    return ResultString{api.GetInputName()};
  }
};

struct CommandSetInputImage {
  std::vector<uint8_t> bytes;
  Result invoke(tesseract::TessBaseAPI &api,
                const std::atomic<bool> &initialized) const {
    RequireInitialized(initialized, "setInputImage");
    if (bytes.size() == 0) {
      throw_runtime("setInputImage: input buffer is empty");
    }

    Pix *pix = pixReadMem(bytes.data(), bytes.size());
    if (pix == nullptr) {
      throw_runtime("setInputImage: failed to decode image buffer");
    }

    // TessBaseAPI::SetInputImage takes ownership of pix.
    api.SetInputImage(pix);
    return ResultVoid{};
  }
};

struct CommandGetInputImage {
  Result invoke(tesseract::TessBaseAPI &api,
                const std::atomic<bool> &initialized) const {
    RequireInitialized(initialized, "getInputImage");
    Pix *source = api.GetInputImage();

    std::cout << source << std::endl;

    if (source == nullptr) {
      throw_runtime("getInputImage: TessBaseAPI::GetInputImage returned null");
    }

    // GetInputImage has no caller-ownership contract; work on a clone.
    Pix *pix = pixClone(source);
    if (pix == nullptr) {
      throw_runtime("getInputImage: failed to clone source image");
    }

    l_uint32 *data = pixGetData(pix);
    l_int32 wpl = pixGetWpl(pix);
    l_int32 h = pixGetHeight(pix);

    size_t bytecount = wpl * 4 * h;
    const uint8_t *start = reinterpret_cast<const uint8_t *>(data);
    std::vector<uint8_t> buffer(start, start + bytecount);
    pixDestroy(&pix);

    return ResultBuffer{buffer};
  }
};

struct CommandGetSourceYResolution {
  Result invoke(tesseract::TessBaseAPI &api,
                const std::atomic<bool> &initialized) const {
    RequireInitialized(initialized, "getSourceYResolution");
    int source_y_resolution = api.GetSourceYResolution();
    return ResultInt{source_y_resolution};
  }
};

struct CommandGetDataPath {
  Result invoke(tesseract::TessBaseAPI &api,
                const std::atomic<bool> &initialized) const {
    RequireInitialized(initialized, "getDataPath");
    const char *data_path = api.GetDatapath();

    if (data_path == nullptr) {
      throw_runtime("getDataPath: TessBaseAPI::GetDatapath returned null");
    }

    return ResultString{data_path};
  }
};

struct CommandSetOutputName {
  std::string output_name;
  Result invoke(tesseract::TessBaseAPI &api,
                const std::atomic<bool> &initialized) const {
    RequireInitialized(initialized, "setOutputName");
    if (output_name.empty()) {
      throw_runtime("setOutputName: output name is empty");
    }

    api.SetOutputName(output_name.c_str());
    return ResultVoid{};
  }
};

struct CommandClearPersistentCache {
  Result invoke(tesseract::TessBaseAPI &api,
                const std::atomic<bool> &initialized) const {
    RequireInitialized(initialized, "clearPersistentCache");
    api.ClearPersistentCache();
    return ResultVoid{};
  }
};

struct CommandClearAdaptiveClassifier {
  Result invoke(tesseract::TessBaseAPI &api,
                const std::atomic<bool> &initialized) const {
    RequireInitialized(initialized, "clearAdaptiveClassifier");
    api.ClearAdaptiveClassifier();
    return ResultVoid{};
  }
};

struct CommandGetThresholdedImage {
  Result invoke(tesseract::TessBaseAPI &api,
                const std::atomic<bool> &initialized) const {
    RequireInitialized(initialized, "getThresholdedImage");
    Pix *pix = api.GetThresholdedImage();

    if (pix == nullptr) {
      throw_runtime("getThresholdedImage: TessBaseAPI::GetThresholdedImage "
                    "returned null");
    }

    l_uint32 *data = pixGetData(pix);
    l_int32 wpl = pixGetWpl(pix);
    l_int32 h = pixGetHeight(pix);

    size_t bytecount = wpl * 4 * h;
    const uint8_t *start = reinterpret_cast<const uint8_t *>(data);
    std::vector<uint8_t> buffer(start, start + bytecount);
    pixDestroy(&pix);

    return ResultBuffer{buffer};
  }
};

struct CommandGetThresholdedImageScaleFactor {
  Result invoke(tesseract::TessBaseAPI &api,
                const std::atomic<bool> &initialized) const {
    RequireInitialized(initialized, "getThresholdedImageScaleFactor");
    int scale_factor = api.GetThresholdedImageScaleFactor();
    return ResultInt{scale_factor};
  }
};

struct CommandInit {
  std::string data_path, language;
  tesseract::OcrEngineMode oem{tesseract::OEM_DEFAULT};
  std::vector<std::string> configs_storage;
  std::vector<char *> configs;
  std::vector<std::string> vars_vec;
  std::vector<std::string> vars_values;
  bool set_only_non_debug_params{false};

  Result invoke(tesseract::TessBaseAPI &api,
                std::atomic<bool> &initialized) const {
    const std::vector<std::string> *vv = vars_vec.empty() ? nullptr : &vars_vec;
    const std::vector<std::string> *vval =
        vars_values.empty() ? nullptr : &vars_values;

    if ((vv == nullptr) != (vval == nullptr) ||
        (vv && vv->size() != vval->size())) {
      throw_runtime(
          "init: vars_vec and vars_values must either both be empty or have "
          "the same length");
    }

    if (api.Init(data_path.empty() ? nullptr : data_path.c_str(),
                 language.empty() ? nullptr : language.c_str(), oem,
                 configs.empty() ? nullptr
                                 : const_cast<char **>(configs.data()),
                 static_cast<int>(configs.size()), vv, vval,
                 set_only_non_debug_params) != 0) {
      throw_runtime("init: TessBaseAPI::Init returned non-zero status");
    }

    initialized.store(true, std::memory_order_release);
    return ResultVoid{};
  }
};

struct CommandInitForAnalysePage {
  Result invoke(tesseract::TessBaseAPI &api,
                const std::atomic<bool> &initialized) const {
    RequireInitialized(initialized, "initForAnalysePage");
    api.InitForAnalysePage();
    return ResultVoid{};
  }
};

struct CommandAnalyseLayout {
  bool merge_similar_words = false;
  Result invoke(tesseract::TessBaseAPI &api,
                const std::atomic<bool> &initialized) const {
    RequireInitialized(initialized, "analyseLayout");

    tesseract::PageIterator *p_iter = api.AnalyseLayout(merge_similar_words);

    // returns nullptr on error or empty page
    if (p_iter == nullptr) {
      throw_runtime("analyseLayout: TessBaseAPI::AnalyseLayout returned null");
    }

    // Convert PageIterator to a feasible object here
    // For now return void for this complex return value
    return ResultVoid{};
  }
};

struct EncodedImageBuffer {
  std::vector<uint8_t> bytes;
};

struct ProcessPagesSession {
  std::unique_ptr<tesseract::TessPDFRenderer> renderer;
  std::string output_base;
  int timeout_millisec{0};
  bool textonly{false};
  int next_page_index{0};
};

struct CommandBeginProcessPages {
  std::string output_base;
  std::string title;
  int timeout_millisec{0}; // 0 = unlimited timeout
  bool textonly{false};
  Result invoke(tesseract::TessBaseAPI &api,
                std::optional<ProcessPagesSession> &session,
                const std::atomic<bool> &initialized) const {
    RequireInitialized(initialized, "beginProcessPages");
    if (session.has_value()) {
      throw_runtime(
          "beginProcessPages called while a session is already active");
    }
    if (title.empty()) {
      throw_runtime("beginProcessPages: title cannot be empty");
    }

    const char *input_name = api.GetInputName();
    std::string effective_output_base = output_base;
    if (effective_output_base.empty()) {
      if (input_name == nullptr || *input_name == '\0') {
        throw_runtime("beginProcessPages: output_base is empty and "
                      "TessBaseAPI::GetInputName() returned null/empty");
      }
      effective_output_base = input_name;
    }

    auto renderer = std::make_unique<tesseract::TessPDFRenderer>(
        effective_output_base.c_str(), api.GetDatapath(), textonly);
    if (!renderer->happy()) {
      throw_runtime("beginProcessPages: renderer is not healthy");
    }
    if (!renderer->BeginDocument(title.c_str())) {
      throw_runtime("beginProcessPages: could not begin document");
    }

    session.emplace();
    session->renderer = std::move(renderer);
    session->output_base = std::move(effective_output_base);
    session->timeout_millisec = timeout_millisec;
    session->textonly = textonly;
    session->next_page_index = 0;
    return ResultVoid{};
  }
};

struct CommandAddProcessPage {
  EncodedImageBuffer page;
  std::string filename;
  Result invoke(tesseract::TessBaseAPI &api,
                std::optional<ProcessPagesSession> &session,
                const std::atomic<bool> &initialized) const {
    RequireInitialized(initialized, "addProcessPage");
    if (!session.has_value()) {
      throw_runtime("addProcessPage: called without an active session");
    }
    if (!session->renderer->happy()) {
      throw_runtime("addProcessPage: renderer is not healthy");
    }
    if (page.bytes.empty()) {
      throw_runtime("addProcessPage: buffer is empty");
    }

    Pix *pix = pixReadMem(page.bytes.data(), page.bytes.size());
    if (pix == nullptr) {
      throw_runtime("addProcessPage: failed to decode image buffer");
    }

    if (pixGetColormap(pix) != nullptr) {
      Pix *no_cmap = pixRemoveColormap(pix, REMOVE_CMAP_BASED_ON_SRC);
      if (no_cmap == nullptr) {
        pixDestroy(&pix);
        throw_runtime("addProcessPage: failed to remove image colormap");
      }
      if (no_cmap != pix) {
        pixDestroy(&pix);
        pix = no_cmap;
      }
    }

    if (pixGetSpp(pix) == 4) {
      Pix *no_alpha = pixRemoveAlpha(pix);
      if (no_alpha == nullptr) {
        pixDestroy(&pix);
        throw_runtime("addProcessPage: failed to remove alpha channel");
      }
      if (no_alpha != pix) {
        pixDestroy(&pix);
        pix = no_alpha;
      }
    }

    const int depth = pixGetDepth(pix);
    if (depth > 0 && depth < 8) {
      Pix *normalized = pixConvertTo8(pix, false);
      if (normalized == nullptr) {
        pixDestroy(&pix);
        throw_runtime(
            "addProcessPage: failed to normalize low-bit-depth image");
      }
      if (normalized != pix) {
        pixDestroy(&pix);
        pix = normalized;
      }
    }

    const int x_res = pixGetXRes(pix);
    const int y_res = pixGetYRes(pix);
    if (x_res <= 0 || y_res <= 0) {
      pixSetResolution(pix, 300, 300);
    }

    const char *effective_filename =
        filename.empty() ? nullptr : filename.c_str();

    bool success = api.ProcessPage(
        pix, session->next_page_index, effective_filename, nullptr,
        session->timeout_millisec, session->renderer.get());
    pixDestroy(&pix);

    if (!success) {
      throw_runtime("addProcessPage: ProcessPage failed at page {}",
                    session->next_page_index);
    }

    session->next_page_index++;
    return ResultVoid{};
  }
};

struct CommandFinishProcessPages {
  Result invoke(tesseract::TessBaseAPI &,
                std::optional<ProcessPagesSession> &session,
                const std::atomic<bool> &initialized) const {
    RequireInitialized(initialized, "finishProcessPages");
    if (!session.has_value()) {
      throw_runtime("finishProcessPages: called without an active session");
    }
    if (!session->renderer->happy()) {
      throw_runtime("finishProcessPages: renderer is not healthy");
    }
    if (!session->renderer->EndDocument()) {
      throw_runtime("finishProcessPages: could not finalize document");
    }

    std::string output_filepath = session->output_base + ".pdf";
    session.reset();
    return ResultString{std::move(output_filepath)};
  }
};

struct CommandAbortProcessPages {
  std::string reason;
  Result invoke(tesseract::TessBaseAPI &,
                std::optional<ProcessPagesSession> &session) const {
    session.reset();
    return ResultVoid{};
  }
};

struct CommandGetProcessPagesStatus {
  Result invoke(tesseract::TessBaseAPI &,
                std::optional<ProcessPagesSession> &session) const {
    if (!session.has_value()) {
      return ResultObject{{
          {"active", false},
          {"healthy", false},
          {"processedPages", 0},
          {"nextPageIndex", 0},
          {"outputBase", std::string{}},
          {"timeoutMillisec", 0},
          {"textonly", false},
      }};
    }

    return ResultObject{{
        {"active", true},
        {"healthy", session->renderer->happy()},
        {"processedPages", session->next_page_index},
        {"nextPageIndex", session->next_page_index},
        {"outputBase", session->output_base},
        {"timeoutMillisec", session->timeout_millisec},
        {"textonly", session->textonly},
    }};
  }
};

struct CommandSetDebugVariable {
  std::string name, value;
  Result invoke(tesseract::TessBaseAPI &api,
                const std::atomic<bool> &initialized) const {
    RequireInitialized(initialized, "setDebugVariable");
    if (name.empty()) {
      throw_runtime("setDebugVariable: variable name is empty");
    } else if (value.empty()) {
      throw_runtime("setDebugVariable: variable value is empty");
    }
    return ResultBool{api.SetDebugVariable(name.c_str(), value.c_str())};
  }
};

struct CommandSetVariable {
  std::string name, value;
  Result invoke(tesseract::TessBaseAPI &api,
                const std::atomic<bool> &initialized) const {
    RequireInitialized(initialized, "setVariable");
    if (name.empty()) {
      throw_runtime("setVariable: variable name is empty");
    } else if (value.empty()) {
      throw_runtime("setVariable: variable value is empty");
    }
    return ResultBool{api.SetVariable(name.c_str(), value.c_str())};
  }
};

struct CommandGetIntVariable {
  std::string name;
  Result invoke(tesseract::TessBaseAPI &api,
                const std::atomic<bool> &initialized) const {
    RequireInitialized(initialized, "getIntVariable");
    int value;
    if (!api.GetIntVariable(name.c_str(), &value)) {
      throw_runtime("getIntVariable: variable '{}' was not found",
                    name.c_str());
    }

    return ResultInt{value};
  }
};

struct CommandGetBoolVariable {
  std::string name;
  Result invoke(tesseract::TessBaseAPI &api,
                const std::atomic<bool> &initialized) const {
    RequireInitialized(initialized, "getBoolVariable");
    bool value;
    if (!api.GetBoolVariable(name.c_str(), &value)) {
      throw_runtime("getBoolVariable: variable '{}' was not found",
                    name.c_str());
    }
    return ResultBool{value};
  }
};

struct CommandGetDoubleVariable {
  std::string name;
  Result invoke(tesseract::TessBaseAPI &api,
                const std::atomic<bool> &initialized) const {
    RequireInitialized(initialized, "getDoubleVariable");
    double value;
    if (!api.GetDoubleVariable(name.c_str(), &value)) {
      throw_runtime("getDoubleVariable: variable '{}' was not found",
                    name.c_str());
    }
    return ResultDouble{value};
  }
};

struct CommandGetStringVariable {
  std::string name;
  Result invoke(tesseract::TessBaseAPI &api,
                const std::atomic<bool> &initialized) const {
    RequireInitialized(initialized, "getStringVariable");
    auto value = api.GetStringVariable(name.c_str());
    if (value == nullptr) {
      throw_runtime("getStringVariable: variable '{}' was not found",
                    name.c_str());
    }
    return ResultString{value};
  }
};

struct CommandSetImage {
  std::vector<uint8_t> bytes;
  int width = 0;
  int height = 0;
  int bytes_per_pixel = 0; // bpp/8
  int bytes_per_line = 0;
  Result invoke(tesseract::TessBaseAPI &api,
                const std::atomic<bool> &initialized) const {
    RequireInitialized(initialized, "setImage");
    api.SetImage(bytes.data(), width, height, bytes_per_pixel, bytes_per_line);
    return ResultVoid{};
  }
};

struct CommandSetPageMode {
  tesseract::PageSegMode psm;
  Result invoke(tesseract::TessBaseAPI &api,
                const std::atomic<bool> &initialized) const {
    RequireInitialized(initialized, "setPageMode");
    if (psm < 0 || psm >= tesseract::PageSegMode::PSM_COUNT) {

      throw_runtime("setPageMode: page segmentation mode is out of range; "
                    "received {}",
                    static_cast<int>(psm));
    }
    api.SetPageSegMode(psm);
    return ResultVoid{};
  }
};

struct CommandSetRectangle {
  int left, top, width, height;
  Result invoke(tesseract::TessBaseAPI &api,
                const std::atomic<bool> &initialized) const {
    RequireInitialized(initialized, "setRectangle");
    api.SetRectangle(left, top, width, height);
    return ResultVoid{};
  }
};

struct CommandSetSourceResolution {
  int ppi;
  Result invoke(tesseract::TessBaseAPI &api,
                const std::atomic<bool> &initialized) const {
    RequireInitialized(initialized, "setSourceResolution");
    api.SetSourceResolution(ppi);
    return ResultVoid{};
  }
};

struct CommandRecognize {
  std::shared_ptr<MonitorContext> monitor_context;
  Result invoke(tesseract::TessBaseAPI &api,
                const std::atomic<bool> &initialized) const {
    RequireInitialized(initialized, "recognize");
    MonitorHandle handle{monitor_context};
    auto *monitor = monitor_context ? &handle.monitor : nullptr;
    if (api.Recognize(monitor) != 0) {
      throw_runtime(
          "recognize: TessBaseAPI::Recognize returned non-zero status");
    }
    return ResultVoid{};
  }
};

// struct CommandGetIterator {
//   Result invoke(tesseract::TessBaseAPI &api) const {
//     api.GetIterator();
//   }
// };

struct CommandDetectOrientationScript {
  Result invoke(tesseract::TessBaseAPI &api,
                const std::atomic<bool> &initialized) const {
    RequireInitialized(initialized, "detectOrientationScript");
    int orient_deg;
    float orient_conf;
    const char *script_name;
    float script_conf;

    if (!api.DetectOrientationScript(&orient_deg, &orient_conf, &script_name,
                                     &script_conf)) {
      throw_runtime(
          "detectOrientationScript: TessBaseAPI::DetectOrientationScript "
          "returned false");
    }

    return ResultObject{{
        {"orientationDegrees", orient_deg},
        {"orientationConfidence", orient_conf},
        {"scriptName", script_name},
        {"scriptConfidence", script_conf},
    }};
  }
};

struct CommandMeanTextConf {
  Result invoke(tesseract::TessBaseAPI &api,
                const std::atomic<bool> &initialized) const {
    RequireInitialized(initialized, "meanTextConf");
    return ResultInt{api.MeanTextConf()};
  }
};

struct CommandGetPAGEText {
  int page_number;
  std::shared_ptr<MonitorContext> monitor_context;
  Result invoke(tesseract::TessBaseAPI &api,
                const std::atomic<bool> &initialized) const {
    RequireInitialized(initialized, "getPAGEText");
    MonitorHandle handle{monitor_context};
    auto *monitor = monitor_context ? &handle.monitor : nullptr;
    char *page_text = api.GetPAGEText(monitor, page_number);
    if (!page_text) {
      throw_runtime("getPAGEText: TessBaseAPI::GetPAGEText returned null");
    }
    std::string text = std::string{page_text};

    delete[] page_text;
    return ResultString(text);
  }
};

struct CommandGetLSTMBoxText {
  int page_number;
  Result invoke(tesseract::TessBaseAPI &api,
                const std::atomic<bool> &initialized) const {
    RequireInitialized(initialized, "getLSTMBoxText");
    char *lstm_box_text = api.GetLSTMBoxText(page_number);
    if (!lstm_box_text) {
      throw_runtime(
          "getLSTMBoxText: TessBaseAPI::GetLSTMBoxText returned null");
    }
    std::string text = std::string{lstm_box_text};

    delete[] lstm_box_text;
    return ResultString(text);
  }
};

struct CommandGetBoxText {
  int page_number;
  Result invoke(tesseract::TessBaseAPI &api,
                const std::atomic<bool> &initialized) const {
    RequireInitialized(initialized, "getBoxText");
    char *box_text = api.GetBoxText(page_number);
    if (!box_text) {
      throw_runtime("getBoxText: TessBaseAPI::GetBoxText returned null");
    }
    std::string text = std::string{box_text};

    delete[] box_text;
    return ResultString(text);
  }
};

struct CommandGetWordStrBoxText {
  int page_number;
  Result invoke(tesseract::TessBaseAPI &api,
                const std::atomic<bool> &initialized) const {
    RequireInitialized(initialized, "getWordStrBoxText");
    char *word_str_box_text = api.GetWordStrBoxText(page_number);
    if (!word_str_box_text) {
      throw_runtime(
          "getWordStrBoxText: TessBaseAPI::GetWordStrBoxText returned null");
    }
    std::string text = std::string{word_str_box_text};

    delete[] word_str_box_text;
    return ResultString(text);
  }
};

struct CommandGetOSDText {
  int page_number;
  Result invoke(tesseract::TessBaseAPI &api,
                const std::atomic<bool> &initialized) const {
    RequireInitialized(initialized, "getOSDText");
    char *ost_text = api.GetOsdText(page_number);
    if (!ost_text) {
      throw_runtime("getOSDText: TessBaseAPI::GetOsdText returned null");
    }
    std::string text = std::string{ost_text};

    delete[] ost_text;
    return ResultString(text);
  }
};

struct CommandAllWordConfidences {
  Result invoke(tesseract::TessBaseAPI &api,
                const std::atomic<bool> &initialized) const {
    RequireInitialized(initialized, "allWordConfidences");
    int *all_word_confidences = api.AllWordConfidences();

    std::vector<int> confidences;
    if (all_word_confidences != nullptr) {
      for (int i = 0; all_word_confidences[i] != -1; ++i) {
        confidences.push_back(all_word_confidences[i]);
      }
    }
    delete[] all_word_confidences;
    return ResultArray{confidences};
  }
};

struct CommandGetUTF8Text {
  Result invoke(tesseract::TessBaseAPI &api,
                const std::atomic<bool> &initialized) const {
    RequireInitialized(initialized, "getUTF8Text");
    char *utf8_text = api.GetUTF8Text();
    if (!utf8_text) {
      throw_runtime("getUTF8Text: TessBaseAPI::GetUTF8Text returned null");
    }
    std::string text = std::string{utf8_text};

    delete[] utf8_text;
    return ResultString{text};
  }
};

struct CommandGetHOCRText {
  int page_number;
  std::shared_ptr<MonitorContext> monitor_context;
  Result invoke(tesseract::TessBaseAPI &api,
                const std::atomic<bool> &initialized) const {
    RequireInitialized(initialized, "getHOCRText");

    MonitorHandle handle{monitor_context};
    auto *monitor = monitor_context ? &handle.monitor : nullptr;
    char *hocr_text = api.GetHOCRText(monitor, page_number);
    if (!hocr_text) {
      throw_runtime("getHOCRText: TessBaseAPI::GetHOCRText returned null");
    }

    std::string text = std::string{hocr_text};

    delete[] hocr_text;
    return ResultString{text};
  }
};

struct CommandGetTSVText {
  int page_number;
  Result invoke(tesseract::TessBaseAPI &api,
                const std::atomic<bool> &initialized) const {
    RequireInitialized(initialized, "getTSVText");
    char *tsv_text = api.GetTSVText(page_number);
    if (!tsv_text) {
      throw_runtime("getTSVText: TessBaseAPI::GetTSVText returned null");
    }
    std::string text = std::string{tsv_text};

    delete[] tsv_text;
    return ResultString{text};
  }
};

struct CommandGetUNLVText {
  Result invoke(tesseract::TessBaseAPI &api,
                const std::atomic<bool> &initialized) const {
    RequireInitialized(initialized, "getUNLVText");
    char *unlv_text = api.GetUNLVText();
    if (!unlv_text) {
      throw_runtime("getUNLVText: TessBaseAPI::GetUNLVText returned null");
    }
    std::string text = std::string{unlv_text};
    delete[] unlv_text;
    return ResultString{text};
  }
};

struct CommandGetALTOText {
  int page_number;
  std::shared_ptr<MonitorContext> monitor_context;
  Result invoke(tesseract::TessBaseAPI &api,
                const std::atomic<bool> &initialized) const {
    RequireInitialized(initialized, "getALTOText");
    MonitorHandle handle{monitor_context};
    auto *monitor = monitor_context ? &handle.monitor : nullptr;
    char *alto_text = api.GetAltoText(monitor, page_number);
    if (!alto_text) {
      throw_runtime("getALTOText: TessBaseAPI::GetAltoText returned null");
    }
    std::string text = std::string{alto_text};
    delete[] alto_text;

    return ResultString{text};
  }
};

struct CommandGetInitLanguages {
  Result invoke(tesseract::TessBaseAPI &api,
                const std::atomic<bool> &initialized) const {
    RequireInitialized(initialized, "getInitLanguages");
    const char *p_init_languages = api.GetInitLanguagesAsString();

    if (p_init_languages == nullptr) {
      throw_runtime("getInitLanguages: TessBaseAPI::GetInitLanguagesAsString "
                    "returned null; call init(...) first with at least one "
                    "valid language");
    }

    std::string init_languages = std::string{p_init_languages};

    return ResultString{init_languages};
  }
};

struct CommandGetLoadedLanguages {
  Result invoke(tesseract::TessBaseAPI &api,
                const std::atomic<bool> &initialized) const {
    RequireInitialized(initialized, "getLoadedLanguages");
    std::vector<std::string> langs;
    api.GetLoadedLanguagesAsVector(&langs);
    return ResultArray{langs};
  }
};

struct CommandGetAvailableLanguages {
  Result invoke(tesseract::TessBaseAPI &api) const {
    std::vector<std::string> langs;
    api.GetAvailableLanguagesAsVector(&langs);
    return ResultArray{langs};
  }
};

struct CommandClear {
  Result invoke(tesseract::TessBaseAPI &api,
                const std::atomic<bool> &initialized) const {
    RequireInitialized(initialized, "clear");
    api.Clear();
    return ResultVoid{};
  }
};

struct CommandEnd {
  Result invoke(tesseract::TessBaseAPI &api,
                std::atomic<bool> &initialized) const {
    api.End();
    initialized.store(false, std::memory_order_release);
    return ResultVoid{};
  }
};

using Command = std::variant<
    CommandVersion, CommandIsInitialized, CommandInit,
    CommandInitForAnalysePage, CommandSetVariable, CommandSetDebugVariable,
    CommandGetIntVariable, CommandGetBoolVariable, CommandGetDoubleVariable,
    CommandGetStringVariable, CommandSetInputName, CommandGetInputName,
    CommandSetOutputName, CommandGetDataPath, CommandSetInputImage,
    CommandGetInputImage, CommandSetPageMode, CommandSetRectangle,
    CommandSetSourceResolution, CommandGetSourceYResolution, CommandSetImage,
    CommandGetThresholdedImage, CommandGetThresholdedImageScaleFactor,
    CommandRecognize, CommandAnalyseLayout, CommandDetectOrientationScript,
    CommandMeanTextConf, CommandAllWordConfidences, CommandGetUTF8Text,
    CommandGetHOCRText, CommandGetTSVText, CommandGetUNLVText,
    CommandGetALTOText, CommandGetPAGEText, CommandGetLSTMBoxText,
    CommandGetBoxText, CommandGetWordStrBoxText, CommandGetOSDText,
    CommandBeginProcessPages, CommandAddProcessPage, CommandFinishProcessPages,
    CommandAbortProcessPages, CommandGetProcessPagesStatus,
    CommandGetInitLanguages, CommandGetLoadedLanguages,
    CommandGetAvailableLanguages, CommandClearPersistentCache,
    CommandClearAdaptiveClassifier, CommandClear, CommandEnd>;

struct Job {
  Command command;
  Napi::Promise::Deferred deffered;

  std::optional<Result> result;
  std::optional<std::string> error;
  std::optional<std::string> error_code;
  std::optional<std::string> error_method;
};
