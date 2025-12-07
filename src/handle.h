#ifndef HANDLE_H
#define HANDLE_H

#include "napi.h"
#include <cstdint>
#include <memory>
#include <mutex>
#include <string>
#include <tesseract/baseapi.h>
#include <tesseract/ocrclass.h>
#include <tesseract/publictypes.h>

using Napi::CallbackInfo;

class Handle : public Napi::ObjectWrap<Handle> {
public:
  static Napi::Object Init(Napi::Env env, Napi::Object exports);
  Handle(const Napi::CallbackInfo &info);
  ~Handle();

  tesseract::TessBaseAPI *Api();
  std::mutex &Mutex();
  tesseract::ETEXT_DESC *Monitor();

private:
  bool skipOcr_ = false;

  // TODO: replace this with an evn variable, safer to use; the user should not
  // be able to inject some weird path into it
  std::string dataPath_ = "/usr/share/tesseract-ocr/5/tessdata";
  std::string lang_ = "eng";
  tesseract::OcrEngineMode oemMode_ = tesseract::OEM_DEFAULT;
  tesseract::PageSegMode psm_ = tesseract::PSM_SINGLE_BLOCK;

  std::unique_ptr<tesseract::TessBaseAPI> api_;
  std::mutex mutex_;

  void SetPageMode(const CallbackInfo &info);
  Napi::Value Recognize(const CallbackInfo &info);

  Napi::Value AnalyzeLayout(const CallbackInfo &info);
};

#endif // HANDLE_H
