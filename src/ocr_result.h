#include "handle.h"
#include <napi.h>

class OCRResult : public Napi::ObjectWrap<OCRResult> {

public:
  OCRResult(const Napi::CallbackInfo &info);
  static Napi::Function GetClass(Napi::Env env);
  static Napi::Object NewInstance(Napi::Env env, Handle *handle);

private:
  void Cancel(const CallbackInfo &info);
  Napi::Value GetText(const CallbackInfo &info);
  Napi::Value GetHOCR(const CallbackInfo &info);
  Napi::Value GetTSV(const CallbackInfo &info);

  Handle *handle_;
};
