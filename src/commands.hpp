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
#include <memory>
#include <napi.h>
#include <optional>
#include <stdexcept>
#include <string>
#include <tesseract/baseapi.h>
#include <tesseract/ocrclass.h>
#include <tesseract/publictypes.h>
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

using ObjectValue = std::variant<bool, int, double, float, std::string,
                                 std::vector<std::string>>;

struct ResultObject {
  std::unordered_map<std::string, ObjectValue> value;
};

struct ResultArray {
  std::vector<std::string> value;
};

using Result =
    std::variant<ResultVoid, ResultBool, ResultInt, ResultDouble, ResultFloat,
                 ResultString, ResultArray, ResultObject>;

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
          [&](const std::string &s) -> Napi::Value {
            return Napi::String::New(env, s);
          },
          [&](const std::vector<std::string> &vec) -> Napi::Value {
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

struct CommandSetVariable {
  std::string name, value;
  Result invoke(tesseract::TessBaseAPI &api) const {
    return ResultBool{api.SetVariable(name.c_str(), value.c_str())};
  }
};

struct CommandGetIntVariable {
  std::string name;
  Result invoke(tesseract::TessBaseAPI &api) const {
    int value;
    if (!api.GetIntVariable(name.c_str(), &value)) {
      throw_runtime(
          "tesseract::TessBaseAPI::GetIntVariable: Variable '{}' was not found",
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

// struct CommandPrintVariables {
//   Result invoke(tesseract::TessBaseAPI &api) const {
// api.PrintVariables(FILE *fp);
//   }
// };

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
    CommandGetUTF8Text, CommandGetHOCRText, CommandGetTSVText,
    CommandGetUNLVText, CommandGetALTOText, CommandGetInitLanguages,
    CommandGetLoadedLanguages, CommandGetAvailableLanguages, CommandClear,
    CommandEnd>;

struct Job {
  Command command;
  Napi::Promise::Deferred deffered;

  std::optional<Result> result;
  std::optional<std::string> error;
};
