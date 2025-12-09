/*
 * Copyright 2025 Philipp Czarnetzki
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

#ifndef OCRRESULT_H
#define OCRRESULT_H

#include "handle.h"
#include <napi.h>

class OCRResult : public Napi::ObjectWrap<OCRResult> {

public:
  OCRResult(const Napi::CallbackInfo &info);
  static Napi::Function GetClass(Napi::Env env);
  static Napi::Object NewInstance(Napi::Env env, const std::string &text,
                                  const std::string &hocr,
                                  const std::string &tsv,
                                  const std::string &alto);

private:
  void Cancel(const CallbackInfo &info);
  Napi::Value GetText(const CallbackInfo &info);
  Napi::Value GetHOCR(const CallbackInfo &info);
  Napi::Value GetTSV(const CallbackInfo &info);
  Napi::Value GetALTO(const CallbackInfo &info);

  Handle *handle_;
  std::string text_;
  std::string hocr_;
  std::string tsv_;
  std::string alto_;
};

#endif // OCRRESULT_H
