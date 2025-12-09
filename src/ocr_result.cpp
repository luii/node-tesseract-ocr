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
