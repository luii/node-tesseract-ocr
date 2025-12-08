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

#include "ocr_result.h"
#include "napi.h"
#include <mutex>
#include <tesseract/baseapi.h>
#include <tesseract/ocrclass.h>

Napi::Function OCRResult::GetClass(Napi::Env env) {
  return DefineClass(env, "OCRResult",
                     {
                         InstanceMethod("getText", &OCRResult::GetText),
                         InstanceMethod("getHOCR", &OCRResult::GetHOCR),
                         InstanceMethod("getTSV", &OCRResult::GetTSV),
                         InstanceMethod("getALTO", &OCRResult::GetALTO),
                     });
}

OCRResult::OCRResult(const Napi::CallbackInfo &info)
    : Napi::ObjectWrap<OCRResult>(info), handle_(nullptr) {
  // Expect constructor args: text, hocr, tsv (all strings)
  if (info.Length() >= 1 && info[0].IsString()) {
    text_ = info[0].As<Napi::String>().Utf8Value();
  }
  if (info.Length() >= 2 && info[1].IsString()) {
    hocr_ = info[1].As<Napi::String>().Utf8Value();
  }
  if (info.Length() >= 3 && info[2].IsString()) {
    tsv_ = info[2].As<Napi::String>().Utf8Value();
  }
  if (info.Length() >= 4 && info[3].IsString()) {
    alto_ = info[3].As<Napi::String>().Utf8Value();
  }
}

Napi::Object OCRResult::NewInstance(Napi::Env env, const std::string &text,
                                    const std::string &hocr,
                                    const std::string &tsv,
                                    const std::string &alto) {
  Napi::EscapableHandleScope scope(env);

  Napi::Function ctor = OCRResult::GetClass(env);
  Napi::Object obj =
      ctor.New({Napi::String::New(env, text), Napi::String::New(env, hocr),
                Napi::String::New(env, tsv), Napi::String::New(env, alto)});

  return scope.Escape(obj).As<Napi::Object>();
}

void OCRResult::Cancel(const Napi::CallbackInfo &info) {
  // Napi::Env env = info.Env();

  {
    std::lock_guard<std::mutex> lock(handle_->Mutex());
    tesseract::ETEXT_DESC *monitor = handle_->Monitor();

    monitor->cancel = [](void *should_cancel, int wordcount) -> bool {
      return true;
    };
    monitor->cancel_this = (void *)true;
    monitor->cancel(monitor->cancel_this, monitor->count);
  }

  return;
}

Napi::Value OCRResult::GetText(const Napi::CallbackInfo &info) {
  Napi::Env env = info.Env();
  return Napi::String::New(env, text_);
}

Napi::Value OCRResult::GetHOCR(const Napi::CallbackInfo &info) {
  Napi::Env env = info.Env();
  return Napi::String::New(env, hocr_);
}

Napi::Value OCRResult::GetTSV(const Napi::CallbackInfo &info) {
  Napi::Env env = info.Env();
  return Napi::String::New(env, tsv_);
}

Napi::Value OCRResult::GetALTO(const Napi::CallbackInfo &info) {
  Napi::Env env = info.Env();
  return Napi::String::New(env, alto_);
}
