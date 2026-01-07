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

#include "utils.hpp"
#include <napi.h>
#include <optional>
#include <stdexcept>
#include <string>
#include <tesseract/baseapi.h>
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

struct ResultString {
  std::string value;
};

struct ResultObject { /*object_type value */
};

struct ResultArray {
  std::vector<std::string> value;
};

using Result = std::variant<ResultVoid, ResultBool, ResultInt, ResultDouble,
                            ResultString, ResultArray>;
// ResultObject, ResultArray

template <class... Ts> struct match : Ts... {
  using Ts::operator()...;
};

static Napi::Value MatchResult(Napi::Env env, const Result &r) {
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
            }},

      // [&](const ResultObject &v) -> Napi::Value {
      //   return Napi::Object::New(env, v.value);
      // }},
      r);
}

struct CommandInit {
  std::string data_path, language;
  Result invoke(tesseract::TessBaseAPI &api) const {
    if (api.Init(data_path.empty() ? nullptr : data_path.c_str(),
                 language.empty() ? nullptr : language.c_str()) != 0) {
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

    // returns nullptr on error or empty page
    if (api.AnalyseLayout(merge_similar_words) == nullptr) {
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
          "tesseract::TessBaseAPI::GetIntVariable: '{}' was not found",
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
      throw_runtime(
          "tesseract::TessBaseAPI::GetBoolVariable: '{}' was not found",
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
      throw_runtime(
          "tesseract::TessBaseAPI::GetDoubleVariable: '{}' was not found",
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
      throw_runtime(
          "tesseract::TessBaseAPI::GetStringVariable: '{}' was not found",
          name.c_str());
    }

    return ResultString{value};
  }
};

struct CommandSetPageMode {
  tesseract::PageSegMode psm;
  Result invoke(tesseract::TessBaseAPI &api) const {
    if (!psm || psm < 0 || psm >= tesseract::PSM_COUNT) {
      throw std::runtime_error("tesseract::TessBaseAPI::SetPageMode out range");
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

  Result invoke(tesseract::TessBaseAPI &api) const {
    // api.Recognize();
    return ResultVoid{};
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
    std::string text = std::string{utf8_text};

    delete[] utf8_text;
    return ResultString{text};
  }
};

struct CommandGetHOCR {
  int page_number;
  Result invoke(tesseract::TessBaseAPI &api) const {

    // support monitor here
    char *hocr_text = api.GetHOCRText(page_number);
    std::string text = std::string{hocr_text};

    delete[] hocr_text;
    return ResultString{text};
  }
};

struct GommandGetTSVText {
  int page_number;
  Result invoke(tesseract::TessBaseAPI &api) const {
    // support monitor
    char *tsv_text = api.GetTSVText(page_number);
    std::string text = std::string{tsv_text};

    delete[] tsv_text;
    return ResultString{text};
  }
};

struct CommandGetUNLVText {
  Result invoke(tesseract::TessBaseAPI &api) const {
    char *unlv_text = api.GetUNLVText();
    std::string text = std::string{unlv_text};
    delete[] unlv_text;
    return ResultString{unlv_text};
  }
};

struct CommandGetALTO {
  int page_number;
  Result invoke(tesseract::TessBaseAPI &api) const {
    // support monitor here;
    char *alto_text = api.GetAltoText(page_number);
    std::string text = std::string{alto_text};
    delete[] alto_text;

    return ResultString{text};
  }
};

struct CommandGetInitLanguages {
  Result invoke(tesseract::TessBaseAPI &api) const {
    const char *p_init_languages = api.GetInitLanguagesAsString();
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
    CommandSetRectangle, CommandSetSourceResolution, CommandRecognize,
    CommandMeanTextConf, CommandGetUTF8Text, CommandGetHOCR, GommandGetTSVText,
    CommandGetUNLVText, CommandGetALTO, CommandGetInitLanguages,
    CommandGetLoadedLanguages, CommandGetAvailableLanguages, CommandClear,
    CommandEnd>;

struct Job {
  Command command;
  Napi::Promise::Deferred deffered;

  std::optional<Result> result;
  std::optional<std::string> error;
};
