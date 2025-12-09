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

#ifndef OCRWORKER_H
#define OCRWORKER_H

#include "handle.h"
#include "leptonica/allheaders.h"
#include "napi.h"
#include <cstddef>
#include <memory>
#include <tesseract/baseapi.h>
#include <tesseract/ocrclass.h>

struct ProgressPayload {
  int percent;
  int progress;
  int ocr_alive;
  int top;
  int right;
  int bottom;
  int left;
};

class OCRWorker : public Napi::AsyncProgressWorker<ProgressPayload> {
public:
  OCRWorker(Handle *handle, Napi::Object handleObject,
            Napi::Buffer<uint8_t> buffer, Napi::Promise::Deferred deffered,
            Napi::Function &progressCallback);
  ~OCRWorker();

protected:
  void Execute(const ExecutionProgress &executionProgress) override;

  void OnOK() override;
  void OnError(const Napi::Error &error) override;
  void OnProgress(const ProgressPayload *payload, size_t count) override;

private:
  Handle *handle_;
  Napi::Reference<Napi::Object> handleRef_;
  uint8_t *data_;
  size_t length_;
  Napi::Reference<Napi::Buffer<uint8_t>> bufferRef_;
  Napi::Promise::Deferred deffered_;
  tesseract::ETEXT_DESC *monitor_ = nullptr;
  Napi::FunctionReference progressCallback_;
  std::string resultText_;
  std::string resultHOCR_;
  std::string resultTSV_;
  std::string resultALTO_;
};

#endif // OCRWORKER_H
