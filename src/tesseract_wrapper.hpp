
#include "napi.h"
#include <atomic>
#include <cstdlib>
#include <tesseract/baseapi.h>
#include <tesseract/publictypes.h>
class TesseractWrapper : Napi::ObjectWrap<TesseractWrapper> {

public:
  static Napi::Object GetClass(Napi::Env env, Napi::Object exports);

  TesseractWrapper(const Napi::CallbackInfo &info);
  ~TesseractWrapper();

  Napi::Env Env() const { return _env; }
  tesseract::TessBaseAPI *Api() { return &_api; }

private:
  static Napi::FunctionReference constructor;

  // JS Methods
  Napi::Value InitEngine(const Napi::CallbackInfo &info);
  Napi::Value InitForAnalysePage(const Napi::CallbackInfo &info);
  Napi::Value AnalysePage(const Napi::CallbackInfo &info);
  Napi::Value SetVariable(const Napi::CallbackInfo &info);
  Napi::Value SetImage(const Napi::CallbackInfo &info);
  Napi::Value SetPageMode(const Napi::CallbackInfo &info);
  Napi::Value SetRectangle(const Napi::CallbackInfo &info);
  Napi::Value Recognize(const Napi::CallbackInfo &info);
  Napi::Value GetUTF8Text(const Napi::CallbackInfo &info);
  Napi::Value GetHOCR(const Napi::CallbackInfo &info);
  Napi::Value GetTSV(const Napi::CallbackInfo &info);
  Napi::Value GetALTO(const Napi::CallbackInfo &info);
  Napi::Value Clear(const Napi::CallbackInfo &info);
  Napi::Value End(const Napi::CallbackInfo &info);

  Napi::Env _env;
  tesseract::TessBaseAPI _api;
  std::atomic_bool _busy{false};
  std::atomic_bool _initialized{false};

  tesseract::OcrEngineMode _oem;
  const std::string _dataPath = std::getenv("NODE_TESSERACT_DATAPATH");
  std::string _lang;

  Pix *_pix = nullptr;
};
