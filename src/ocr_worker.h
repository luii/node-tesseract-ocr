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
