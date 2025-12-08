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
