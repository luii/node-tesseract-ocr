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

#include "worker_thread.hpp"
#include <napi.h>
#include <tesseract/baseapi.h>
#include <tesseract/publictypes.h>

class TesseractWrapper : public Napi::ObjectWrap<TesseractWrapper> {

public:
  static Napi::Object InitAddon(Napi::Env env, Napi::Object exports);

  explicit TesseractWrapper(const Napi::CallbackInfo &info);
  ~TesseractWrapper() override;

  Napi::Env Env() const { return _env; }

private:
  static Napi::FunctionReference constructor;

  // JS Methods
  Napi::Value Init(const Napi::CallbackInfo &info);
  Napi::Value InitForAnalysePage(const Napi::CallbackInfo &info);
  Napi::Value AnalysePage(const Napi::CallbackInfo &info);
  Napi::Value SetVariable(const Napi::CallbackInfo &info);
  Napi::Value GetVariable(const Napi::CallbackInfo &info);
  Napi::Value GetIntVariable(const Napi::CallbackInfo &info);
  Napi::Value GetBoolVariable(const Napi::CallbackInfo &info);
  Napi::Value GetDoubleVariable(const Napi::CallbackInfo &info);
  Napi::Value GetStringVariable(const Napi::CallbackInfo &info);
  Napi::Value PrintVariables(const Napi::CallbackInfo &info);
  Napi::Value SetImage(const Napi::CallbackInfo &info);
  Napi::Value SetPageMode(const Napi::CallbackInfo &info);
  Napi::Value SetRectangle(const Napi::CallbackInfo &info);
  Napi::Value SetSourceResolution(const Napi::CallbackInfo &info);
  Napi::Value Recognize(const Napi::CallbackInfo &info);
  Napi::Value DetectOrientationScript(const Napi::CallbackInfo &info);
  Napi::Value MeanTextConf(const Napi::CallbackInfo &info);
  Napi::Value GetUTF8Text(const Napi::CallbackInfo &info);
  Napi::Value GetHOCRText(const Napi::CallbackInfo &info);
  Napi::Value GetTSVText(const Napi::CallbackInfo &info);
  Napi::Value GetUNLVText(const Napi::CallbackInfo &info);
  Napi::Value GetALTOText(const Napi::CallbackInfo &info);
  Napi::Value GetInitLanguages(const Napi::CallbackInfo &info);
  Napi::Value GetLoadedLanguages(const Napi::CallbackInfo &info);
  Napi::Value GetAvailableLanguages(const Napi::CallbackInfo &info);
  Napi::Value Clear(const Napi::CallbackInfo &info);
  Napi::Value End(const Napi::CallbackInfo &info);

  Napi::Env _env;
  WorkerThread _worker_thread;

  const std::string _data_path = std::getenv("NODE_TESSERACT_DATAPATH");
};
