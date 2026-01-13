
#pragma once

#include <algorithm>
#include <memory>
#include <napi.h>
#include <tesseract/ocrclass.h>

struct ProgressUpdate {
  int progress;
  int percent;
  int ocr_alive;
  int top;
  int right;
  int bottom;
  int left;
};

struct MonitorContext {
  explicit MonitorContext(Napi::ThreadSafeFunction progress_tsfn)
      : js_progress_callback(std::move(progress_tsfn)) {}
  Napi::ThreadSafeFunction js_progress_callback;
};

struct MonitorHandle {
  tesseract::ETEXT_DESC monitor{};
  std::shared_ptr<MonitorContext> monitor_context;

  MonitorHandle(std::shared_ptr<MonitorContext> ctx)
      : monitor_context(std::move(ctx)) {
    if (monitor_context) {
      monitor.cancel_this = monitor_context.get();
      monitor.progress_callback2 = [](tesseract::ETEXT_DESC *monitor, int left,
                                      int right, int top, int bottom) -> bool {
        auto *ctx = static_cast<MonitorContext *>(monitor->cancel_this);
        if (!ctx) {
          return true;
        }

        auto *update = new ProgressUpdate{monitor->count,
                                          monitor->progress,
                                          monitor->ocr_alive,
                                          top,
                                          right,
                                          bottom,
                                          left};

        napi_status status = ctx->js_progress_callback.NonBlockingCall(
            update, [](Napi::Env env, Napi::Function js_cb, ProgressUpdate *v) {
              Napi::Object info = Napi::Object::New(env);
              info.Set("progress", Napi::Number::New(env, v->progress));
              info.Set("percent", Napi::Number::New(env, v->percent));
              info.Set("ocrAlive", Napi::Number::New(env, v->ocr_alive));
              info.Set("top", Napi::Number::New(env, v->top));
              info.Set("right", Napi::Number::New(env, v->right));
              info.Set("bottom", Napi::Number::New(env, v->bottom));
              info.Set("left", Napi::Number::New(env, v->left));
              js_cb.Call({info});
              delete v;
            });

        if (status != napi_ok) {
          delete update;
        }

        return true;
      };
    }
  }

  MonitorHandle(const MonitorHandle &) = delete;
  MonitorHandle &operator=(const MonitorHandle &) = delete;
  MonitorHandle(MonitorHandle &&) = default;
  MonitorHandle &operator=(MonitorHandle &&) = default;

  ~MonitorHandle() {
    if (monitor_context) {
      monitor_context->js_progress_callback.Release();
    }
  }
};
