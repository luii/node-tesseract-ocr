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
                     });
}

OCRResult::OCRResult(const Napi::CallbackInfo &info)
    : Napi::ObjectWrap<OCRResult>(info), handle_(nullptr) {
  if (info.Length() > 0 && info[0].IsExternal()) {
    handle_ = info[0].As<Napi::External<Handle>>().Data();
  }
}

Napi::Object OCRResult::NewInstance(Napi::Env env, Handle *handle) {
  Napi::EscapableHandleScope scope(env);

  Napi::Function ctor = OCRResult::GetClass(env);
  Napi::External<Handle> ext = Napi::External<Handle>::New(env, handle);
  Napi::Object obj = ctor.New({ext});

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
  std::string text;

  {
    std::lock_guard<std::mutex> lock(handle_->Mutex());
    tesseract::TessBaseAPI *api = handle_->Api();

    // if (!handle_->Recognized()) {
    //   throw Napi::Error::New(env, "No recognition result available");
    // }

    char *t = api->GetUTF8Text();
    if (!t) {
      Napi::Error::New(env, "GetUTF8Text failed").ThrowAsJavaScriptException();
    }
    text.assign(t);
    delete[] t;
  }

  return Napi::String::New(env, text);
}

Napi::Value OCRResult::GetHOCR(const Napi::CallbackInfo &info) {
  Napi::Env env = info.Env();
  std::string hocr;

  {
    std::lock_guard<std::mutex> lock(handle_->Mutex());
    tesseract::TessBaseAPI *api = handle_->Api();

    // if (!handle_->Recognized()) {
    //   throw Napi::Error::New(env, "No recognition result available");
    // }

    char *t = api->GetHOCRText(0);
    if (!t) {
      Napi::Error::New(env, "GetHOCRText failed").ThrowAsJavaScriptException();
    }
    hocr.assign(t);
    delete[] t;
  }

  return Napi::String::New(env, hocr);
}

Napi::Value OCRResult::GetTSV(const Napi::CallbackInfo &info) {
  Napi::Env env = info.Env();
  std::string tsv;

  {
    std::lock_guard<std::mutex> lock(handle_->Mutex());
    tesseract::TessBaseAPI *api = handle_->Api();

    // if (!handle_->Recognized()) {
    //   throw Napi::Error::New(env, "No recognition result available");
    // }

    char *t = api->GetTSVText(0);
    if (!t) {
      Napi::Error::New(env, "GetTSVText failed").ThrowAsJavaScriptException();
    }
    tsv.assign(t);
    delete[] t;
  }

  return Napi::String::New(env, tsv);
}
