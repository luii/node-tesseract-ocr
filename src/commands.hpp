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
#include <cstddef>
#include <memory>
#include <napi.h>
#include <optional>
#include <stdexcept>
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

using ObjectValue =
    std::variant<bool, int, double, float, std::string,
                 std::vector<std::string>, std::vector<uint8_t>>;

struct ResultObject {
  std::unordered_map<std::string, ObjectValue> value;
};

struct ResultArray {
  std::vector<ObjectValue> value;
};

using Result =
    std::variant<ResultVoid, ResultBool, ResultInt, ResultDouble, ResultFloat,
                 ResultString, ResultArray, ResultBuffer, ResultObject>;

template <class... Ts> struct match : Ts... {
  using Ts::operator()...;
};

template <class... Ts> match(Ts...) -> match<Ts...>;

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
          [&](const std::vector<std::string> &vec) -> Napi::Value { // Array
            Napi::Array arr = Napi::Array::New(env, vec.size());
            for (size_t i = 0; i < vec.size(); ++i) {
              arr.Set(static_cast<uint32_t>(i), vec[i]);
            }
            return arr;
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
              const size_t n = v.value.size();

              if (n > std::numeric_limits<uint32_t>::max()) {
                // return Napi::RangeError::New(
                //     env, "ResultArray too large for JS array");
              }

              Napi::Array array = Napi::Array::New(env, v.value.size());
              for (size_t i = 0; i < v.value.size(); i++) {
                array.Set(static_cast<uint32_t>(i), v.value[i]);
              }
              return array;
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

struct CommandVersion {
  Result invoke(tesseract::TessBaseAPI &api) const {
    return ResultString{api.Version()};
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
  Result invoke(tesseract::TessBaseAPI &api) const {
    if (bytes.size() == 0) {
      throw_runtime("Input Image Buffer is empty");
    }

    Pix *pix = pixReadMem(bytes.data(), bytes.size());
    api.SetInputImage(pix);
    pixDestroy(&pix); // do i need that since SetInputImage takes ownership?
    return ResultVoid{};
  }
};

struct CommandGetInputImage {
  Result invoke(tesseract::TessBaseAPI &api) const {
    Pix *pix = api.GetInputImage();

    if (pix == nullptr) {
      throw_runtime("Could not get Input image");
    }

    l_uint32 *data = pixGetData(pix);
    l_int32 wpl = pixGetWpl(pix);
    l_int32 h = pixGetHeight(pix);

    size_t bytecount = wpl * 4 * h;
    lept_free(&pix);

    std::vector<uint8_t> buffer(data, data + bytecount);

    return ResultBuffer{buffer};
  }
};

struct CommandGetSourceYResolution {
  Result invoke(tesseract::TessBaseAPI &api) const {
    int source_y_resolution = api.GetSourceYResolution();
    return ResultInt{source_y_resolution};
  }
};

struct CommandGetDataPath {
  Result invoke(tesseract::TessBaseAPI &api) const {
    const char *data_path = api.GetDatapath();

    if (data_path == nullptr) {
      throw_runtime("No data path set");
    }

    return ResultString{data_path};
  }
};

struct CommandSetOutputName {
  std::string output_name;
  Result invoke(tesseract::TessBaseAPI &api) const {
    if (output_name.empty()) {
      throw_runtime("Output name is empty or not set");
    }

    api.SetOutputName(output_name.c_str());
    return ResultVoid{};
  }
};

struct CommandClearPersistentCache {
  Result invoke(tesseract::TessBaseAPI &api) const {
    api.ClearPersistentCache();
    return ResultVoid{};
  }
};

struct CommandClearAdaptiveClassifier {
  Result invoke(tesseract::TessBaseAPI &api) const {
    api.ClearAdaptiveClassifier();
    return ResultVoid{};
  }
};

struct CommandGetThresholdedImage {
  Result invoke(tesseract::TessBaseAPI &api) const {
    Pix *pix = api.GetThresholdedImage();

    if (pix == nullptr) {
      throw_runtime("Could not get Input image");
    }

    l_uint32 *data = pixGetData(pix);
    l_int32 wpl = pixGetWpl(pix);
    l_int32 h = pixGetHeight(pix);

    size_t bytecount = wpl * 4 * h;
    lept_free(&pix);

    std::vector<uint8_t> buffer(data, data + bytecount);

    return ResultBuffer{buffer};
  }
};

struct CommandGetThresholdedImageScaleFactor {
  Result invoke(tesseract::TessBaseAPI &api) const {
    int scale_factor = api.GetThresholdedImageScaleFactor();
    return ResultInt{scale_factor};
  }
};

struct EncodedImageBuffer {
  std::vector<uint8_t> bytes;
};

struct ProcessPagesSession {
  std::unique_ptr<tesseract::TessPDFRenderer> renderer;
  std::string output_base;
  std::string filename;
  int timeout_millisec{0};
  int next_page_index{0};
};

struct CommandBeginProcessPages {
  std::string output_base;
  std::string title;
  std::string filename;
  int timeout_millisec{0}; // 0 = unlimited timeout
  bool textonly{false};
  Result invoke(tesseract::TessBaseAPI &api,
                std::optional<ProcessPagesSession> &session) const {
    if (session.has_value()) {
      throw_runtime(
          "beginProcessPages called while a session is already active");
    }
    if (title.empty()) {
      throw_runtime("beginProcessPages: title cannot be empty");
    }

    const char *input_name = api.GetInputName();
    std::string effective_output_base = output_base;
    std::string effective_filename = filename;

    if (effective_output_base.empty()) {
      if (input_name == nullptr || *input_name == '\0') {
        throw_runtime("beginProcessPages: output_base is empty and "
                      "TessBaseAPI::GetInputName() returned null/empty");
      }
      effective_output_base = input_name;
    }

    if (effective_filename.empty()) {
      if (input_name == nullptr || *input_name == '\0') {
        throw_runtime("beginProcessPages: filename is empty and "
                      "TessBaseAPI::GetInputName() returned null/empty");
      }
      effective_filename = input_name;
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
    session->filename = std::move(effective_filename);
    session->timeout_millisec = timeout_millisec;
    session->next_page_index = 0;
    return ResultVoid{};
  }
};

struct CommandAddProcessPage {
  EncodedImageBuffer page;
  Result invoke(tesseract::TessBaseAPI &api,
                std::optional<ProcessPagesSession> &session) const {
    if (!session.has_value()) {
      throw_runtime("addProcessPage called without an active session");
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

    bool success = api.ProcessPage(
        pix, session->next_page_index, session->filename.c_str(), nullptr,
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
                std::optional<ProcessPagesSession> &session) const {
    if (!session.has_value()) {
      throw_runtime("finishProcessPages called without an active session");
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

// struct CommandProcessPages {
//   std::vector<EncodedImageBuffer> buffers;
//   std::string output_base; // actual name of the file that will be outputted
//   std::string title;       // metadata title of the resulting pdf
//   std::string filename;    // metadata filename of the resulting pdf
//   int timeout_millisec{0}; // 0 = unlimited timeout
//   const bool textonly{false};
//   Result invoke(tesseract::TessBaseAPI &api) const {
//     std::string effective_output_base = output_base;
//     std::string effective_filename = filename;
//     const char *input_name = api.GetInputName();
//
//     if (effective_output_base.empty()) {
//       if (input_name == nullptr || *input_name == '\0') {
//         throw_runtime("CommandProcessPages: output_base is empty and "
//                       "GetInputName() returned null/empty");
//       }
//       effective_output_base = input_name;
//     }
//
//     if (effective_filename.empty()) {
//       if (input_name == nullptr || *input_name == '\0') {
//         throw_runtime("CommandProcessPages: filename is empty and "
//                       "GetInputName() returned null/empty");
//       }
//       effective_filename = input_name;
//     }
//
//     if (title.empty()) {
//       throw_runtime("CommandProcessPages: No title set");
//     }
//
//     auto renderer = std::make_unique<tesseract::TessPDFRenderer>(
//         effective_output_base.c_str(), api.GetDatapath(), textonly);
//
//     if (!renderer->happy()) {
//       throw_runtime("CommandProcessPages: TessPDFRenderer is not happy");
//     }
//
//     bool started = renderer->BeginDocument(title.c_str());
//
//     if (!started) {
//       throw_runtime("CommandProcessPages: Failed to begin new document "
//                     "(filename: {}, output_base: {})",
//                     effective_filename, effective_output_base);
//     }
//
//     for (int i = 0; i < static_cast<int>(buffers.size()); ++i) {
//       if (!renderer->happy()) {
//         throw_runtime("CommandProcessPages: Renderer became unhealthy while "
//                       "processing page {}",
//                       i);
//       }
//
//       const EncodedImageBuffer &encoded = buffers[static_cast<size_t>(i)];
//       Pix *pix = pixReadMem(encoded.bytes.data(), encoded.bytes.size());
//
//       if (!pix) {
//         continue;
//       }
//
//       bool succeed = api.ProcessPage(pix, i, effective_filename.c_str(),
//                                      nullptr, timeout_millisec,
//                                      renderer.get());
//
//       pixDestroy(&pix);
//
//       if (!succeed) {
//         continue; // silently continue with the next page?
//       }
//     }
//
//     bool ended = renderer->EndDocument();
//
//     if (!ended) {
//       throw_runtime("CommandProcessPages: Renderer could not end document");
//     }
//
//     std::string output_filepath = effective_output_base + ".pdf";
//     return ResultString{output_filepath};
//   }
// };

struct CommandInit {
  std::string data_path, language;
  tesseract::OcrEngineMode oem{tesseract::OEM_DEFAULT};
  std::vector<std::string> configs_storage;
  std::vector<char *> configs;
  std::vector<std::string> vars_vec;
  std::vector<std::string> vars_values;
  bool set_only_non_debug_params{false};

  Result invoke(tesseract::TessBaseAPI &api) const {
    const std::vector<std::string> *vv = vars_vec.empty() ? nullptr : &vars_vec;
    const std::vector<std::string> *vval =
        vars_values.empty() ? nullptr : &vars_values;

    if ((vv == nullptr) != (vval == nullptr) ||
        (vv && vv->size() != vval->size())) {
      throw std::runtime_error(
          "vars_vec and vars_values must both be set and same length");
    }

    if (api.Init(data_path.empty() ? nullptr : data_path.c_str(),
                 language.empty() ? nullptr : language.c_str(), oem,
                 configs.empty() ? nullptr
                                 : const_cast<char **>(configs.data()),
                 static_cast<int>(configs.size()), vv, vval,
                 set_only_non_debug_params) != 0) {
      throw std::runtime_error("tesseract::TessBaseAPI::Init failed");
    }

    return ResultVoid{};
  }
};

struct CommandInitForAnalysePage {
  Result invoke(tesseract::TessBaseAPI &api) const {
    api.InitForAnalysePage();
    return ResultVoid{};
  }
};

struct CommandAnalyseLayout {
  bool merge_similar_words = false;
  Result invoke(tesseract::TessBaseAPI &api) const {

    tesseract::PageIterator *p_iter = api.AnalyseLayout(merge_similar_words);

    // returns nullptr on error or empty page
    if (p_iter == nullptr) {
      throw std::runtime_error("tesseract::TessBaseAPI::AnalyseLayout failed");
    }

    // Convert PageIterator to a feasible object here
    // For now return void for this complex return value
    return ResultVoid{};
  }
};

struct CommandSetDebugVariable {
  std::string name, value;
  Result invoke(tesseract::TessBaseAPI &api) const {
    if (name.empty()) {
      throw_runtime("SetDebugVariable: variable name is empty");
    } else if (value.empty()) {
      throw_runtime("SetDebugVariable: variable value is empty");
    }
    return ResultBool{api.SetDebugVariable(name.c_str(), value.c_str())};
  }
};

struct CommandSetVariable {
  std::string name, value;
  Result invoke(tesseract::TessBaseAPI &api) const {
    if (name.empty()) {
      throw_runtime("SetVariable: variable name is empty");
    } else if (value.empty()) {
      throw_runtime("SetVariable: variable value is empty");
    }
    return ResultBool{api.SetVariable(name.c_str(), value.c_str())};
  }
};

struct CommandGetIntVariable {
  std::string name;
  Result invoke(tesseract::TessBaseAPI &api) const {
    int value;
    if (!api.GetIntVariable(name.c_str(), &value)) {
      throw_runtime("tesseract::TessBaseAPI::GetIntVariable: Variable '{}' "
                    "was not found",
                    name.c_str());
    }

    return ResultInt{value};
  }
};

struct CommandGetBoolVariable {
  std::string name;
  Result invoke(tesseract::TessBaseAPI &api) const {
    bool value;
    if (!api.GetBoolVariable(name.c_str(), &value)) {
      throw_runtime("tesseract::TessBaseAPI::GetBoolVariable: Variable '{}' "
                    "was not found",
                    name.c_str());
    }
    return ResultBool{value};
  }
};

struct CommandGetDoubleVariable {
  std::string name;
  Result invoke(tesseract::TessBaseAPI &api) const {
    double value;
    if (!api.GetDoubleVariable(name.c_str(), &value)) {
      throw_runtime("tesseract::TessBaseAPI::GetDoubleVariable: Variable '{}' "
                    "was not found",
                    name.c_str());
    }
    return ResultDouble{value};
  }
};

struct CommandGetStringVariable {
  std::string name;
  Result invoke(tesseract::TessBaseAPI &api) const {
    auto value = api.GetStringVariable(name.c_str());
    if (value == nullptr) {
      throw_runtime("tesseract::TessBaseAPI::GetStringVariable: Variable '{}' "
                    "was not found",
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
  Result invoke(tesseract::TessBaseAPI &api) const {
    api.SetImage(bytes.data(), width, height, bytes_per_pixel, bytes_per_line);
    return ResultVoid{};
  }
};

struct CommandSetPageMode {
  tesseract::PageSegMode psm;
  Result invoke(tesseract::TessBaseAPI &api) const {
    if (psm < 0 || psm >= tesseract::PageSegMode::PSM_COUNT) {

      throw_runtime(
          "tesseract::TessBaseAPI::SetPageMode out range; received: {}",
          static_cast<int>(psm));
    }
    api.SetPageSegMode(psm);
    return ResultVoid{};
  }
};

struct CommandSetRectangle {
  int left, top, width, height;
  Result invoke(tesseract::TessBaseAPI &api) const {
    api.SetRectangle(left, top, width, height);
    return ResultVoid{};
  }
};

struct CommandSetSourceResolution {
  int ppi;
  Result invoke(tesseract::TessBaseAPI &api) const {
    api.SetSourceResolution(ppi);
    return ResultVoid{};
  }
};

struct CommandRecognize {
  std::shared_ptr<MonitorContext> monitor_context;
  Result invoke(tesseract::TessBaseAPI &api) const {
    MonitorHandle handle{monitor_context};
    auto *monitor = monitor_context ? &handle.monitor : nullptr;
    if (api.Recognize(monitor) != 0) {
      throw std::runtime_error("tesseract::TessBaseAPI::Recognize failed");
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
  Result invoke(tesseract::TessBaseAPI &api) const {
    int orient_deg;
    float orient_conf;
    const char *script_name;
    float script_conf;

    if (!api.DetectOrientationScript(&orient_deg, &orient_conf, &script_name,
                                     &script_conf)) {
      throw std::runtime_error(
          "tesseract::TessBaseAPI::DetectOrientationScript failed");
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
  Result invoke(tesseract::TessBaseAPI &api) const {
    return ResultInt{api.MeanTextConf()};
  }
};

struct CommandGetPAGEText {
  int page_number;
  std::shared_ptr<MonitorContext> monitor_context;
  Result invoke(tesseract::TessBaseAPI &api) const {
    MonitorHandle handle{monitor_context};
    auto *monitor = monitor_context ? &handle.monitor : nullptr;
    char *page_text = api.GetPAGEText(monitor, page_number);
    if (!page_text) {
      throw_runtime("GetPAGEText returned null");
    }
    std::string text = std::string{page_text};

    delete[] page_text;
    return ResultString(text);
  }
};

struct CommandGetLSTMBoxText {
  int page_number;
  Result invoke(tesseract::TessBaseAPI &api) const {
    char *lstm_box_text = api.GetLSTMBoxText(page_number);
    if (!lstm_box_text) {
      throw_runtime("GetLSTMBoxText returned null");
    }
    std::string text = std::string{lstm_box_text};

    delete[] lstm_box_text;
    return ResultString(text);
  }
};

struct CommandGetBoxText {
  int page_number;
  Result invoke(tesseract::TessBaseAPI &api) const {
    char *box_text = api.GetBoxText(page_number);
    if (!box_text) {
      throw_runtime("GetBoxText returned null");
    }
    std::string text = std::string{box_text};

    delete[] box_text;
    return ResultString(text);
  }
};

struct CommandGetWordStrBoxText {
  int page_number;
  Result invoke(tesseract::TessBaseAPI &api) const {
    char *word_str_box_text = api.GetWordStrBoxText(page_number);
    if (!word_str_box_text) {
      throw_runtime("GetWordStrBoxText returned null");
    }
    std::string text = std::string{word_str_box_text};

    delete[] word_str_box_text;
    return ResultString(text);
  }
};

struct CommandGetOSDText {
  int page_number;
  Result invoke(tesseract::TessBaseAPI &api) const {
    char *ost_text = api.GetOsdText(page_number);
    if (!ost_text) {
      throw_runtime("GetOSDText returned null");
    }
    std::string text = std::string{ost_text};

    delete[] ost_text;
    return ResultString(text);
  }
};

struct CommandAllWordConfidences {
  Result invoke(tesseract::TessBaseAPI &api) const {
    int *all_word_confidences = api.AllWordConfidences();

    std::vector<int> confidences =

        delete[] all_word_confidences;
    return ResultArray{};
  }
};

struct CommandGetUTF8Text {
  Result invoke(tesseract::TessBaseAPI &api) const {
    char *utf8_text = api.GetUTF8Text();
    if (!utf8_text) {
      throw_runtime("GetUTF8Text returned null");
    }
    std::string text = std::string{utf8_text};

    delete[] utf8_text;
    return ResultString{text};
  }
};

struct CommandGetHOCRText {
  int page_number;
  std::shared_ptr<MonitorContext> monitor_context;
  Result invoke(tesseract::TessBaseAPI &api) const {

    MonitorHandle handle{monitor_context};
    auto *monitor = monitor_context ? &handle.monitor : nullptr;
    char *hocr_text = api.GetHOCRText(monitor, page_number);
    if (!hocr_text) {
      throw_runtime("GetHOCRText returned null");
    }

    std::string text = std::string{hocr_text};

    delete[] hocr_text;
    return ResultString{text};
  }
};

struct CommandGetTSVText {
  int page_number;
  Result invoke(tesseract::TessBaseAPI &api) const {
    char *tsv_text = api.GetTSVText(page_number);
    if (!tsv_text) {
      throw_runtime("GetTSVText returned null");
    }
    std::string text = std::string{tsv_text};

    delete[] tsv_text;
    return ResultString{text};
  }
};

struct CommandGetUNLVText {
  Result invoke(tesseract::TessBaseAPI &api) const {
    char *unlv_text = api.GetUNLVText();
    if (!unlv_text) {
      throw_runtime("GetUNLVText returned null");
    }
    std::string text = std::string{unlv_text};
    delete[] unlv_text;
    return ResultString{text};
  }
};

struct CommandGetALTOText {
  int page_number;
  std::shared_ptr<MonitorContext> monitor_context;
  Result invoke(tesseract::TessBaseAPI &api) const {
    MonitorHandle handle{monitor_context};
    auto *monitor = monitor_context ? &handle.monitor : nullptr;
    char *alto_text = api.GetAltoText(monitor, page_number);
    if (!alto_text) {
      throw_runtime("GetALTOText returned null");
    }
    std::string text = std::string{alto_text};
    delete[] alto_text;

    return ResultString{text};
  }
};

struct CommandGetInitLanguages {
  Result invoke(tesseract::TessBaseAPI &api) const {
    const char *p_init_languages = api.GetInitLanguagesAsString();

    if (p_init_languages == nullptr) {
      // TODO: put a better error message here
      // throw std::runtime_error("Uhhhhm");
    }

    std::string init_languages = std::string{p_init_languages};

    return ResultString{init_languages};
  }
};

struct CommandGetLoadedLanguages {
  Result invoke(tesseract::TessBaseAPI &api) const {
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
  Result invoke(tesseract::TessBaseAPI &api) const {
    api.Clear();
    return ResultVoid{};
  }
};

struct CommandEnd {
  Result invoke(tesseract::TessBaseAPI &api) const {
    api.End();
    return ResultVoid{};
  }
};

using Command = std::variant<
    CommandInit, CommandInitForAnalysePage, CommandAnalyseLayout,
    CommandSetVariable, CommandGetIntVariable, CommandGetBoolVariable,
    CommandGetDoubleVariable, CommandGetStringVariable, CommandSetPageMode,
    CommandSetRectangle, CommandSetSourceResolution, CommandSetImage,
    CommandRecognize, CommandDetectOrientationScript, CommandMeanTextConf,
    CommandBeginProcessPages, CommandAddProcessPage, CommandFinishProcessPages,
    CommandAbortProcessPages, CommandGetUTF8Text, CommandGetHOCRText,
    CommandGetTSVText, CommandGetUNLVText, CommandGetALTOText,
    CommandGetInitLanguages, CommandGetLoadedLanguages,
    CommandGetAvailableLanguages, CommandClear, CommandEnd>;

struct Job {
  Command command;
  Napi::Promise::Deferred deffered;

  std::optional<Result> result;
  std::optional<std::string> error;
};
