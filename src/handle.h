/*
 * node-tesseract-ocr
 * Copyright (C) 2025  Philipp Czarnetzki
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
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
