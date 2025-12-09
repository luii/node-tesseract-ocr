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

#ifndef HANDLE_H
#define HANDLE_H

#include "napi.h"
#include <cstdint>
#include <cstdlib>
#include <memory>
#include <mutex>
#include <string>
#include <tesseract/baseapi.h>
#include <tesseract/ocrclass.h>
#include <tesseract/publictypes.h>

using Napi::CallbackInfo;

class Handle : public Napi::ObjectWrap<Handle> {
public:
  static Napi::Object GetClass(Napi::Env env, Napi::Object exports);
  Handle(const Napi::CallbackInfo &info);
  ~Handle();

  std::unique_ptr<tesseract::TessBaseAPI> CreateApi();
  std::mutex &Mutex();
  tesseract::ETEXT_DESC *Monitor();

private:
  bool skipOcr_ = false;

  std::string dataPath_ = std::getenv("NODE_TESSERACT_DATAPATH");
  std::string lang_ = "eng";
  tesseract::OcrEngineMode oemMode_ = tesseract::OEM_DEFAULT;
  tesseract::PageSegMode psm_ = tesseract::PSM_SINGLE_BLOCK;

  std::unique_ptr<tesseract::TessBaseAPI> api_;
  std::mutex mutex_;

  Napi::Value Recognize(const CallbackInfo &info);
  Napi::Value AnalyzeLayout(const CallbackInfo &info);
};

#endif // HANDLE_H
