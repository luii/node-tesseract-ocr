#include "handle.h"
#include <napi.h>

Napi::Object InitAll(Napi::Env env, Napi::Object exports) {
  return Handle::Init(env, exports);
}

NODE_API_MODULE(hello, InitAll)
