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
  OCRWorker(const Napi::Env &env, Handle *handle, Napi::Buffer<uint8_t> buffer,
            Napi::Promise::Deferred deffered, Napi::Function &progressCallback);

protected:
  void Execute(const ExecutionProgress &executionProgress) override;

  void OnOK() override;
  void OnError(const Napi::Error &error) override;
  void OnProgress(const ProgressPayload *payload, size_t count) override;

private:
  Handle *handle_;
  uint8_t *data_;
  size_t length_;
  Napi::Promise::Deferred deffered_;
  tesseract::ETEXT_DESC *monitor_ = new tesseract::ETEXT_DESC();
  Napi::FunctionReference progressCallback_;
};
