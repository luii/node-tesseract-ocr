/*
 * Copyright 2026 Philipp Czarnetzki
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

#include "worker_thread.hpp"
#include "commands.hpp"
#include <exception>
#include <memory>
#include <mutex>
#include <optional>
#include <queue>
#include <stop_token>
#include <thread>
#include <type_traits>
#include <variant>
#include <vector>

namespace {

std::string CommandName(const Command &command) {
  return std::visit(
      [](const auto &c) -> std::string {
        using T = std::decay_t<decltype(c)>;
        if constexpr (std::is_same_v<T, CommandVersion>)
          return "version";
        if constexpr (std::is_same_v<T, CommandInit>)
          return "init";
        if constexpr (std::is_same_v<T, CommandInitForAnalysePage>)
          return "initForAnalysePage";
        if constexpr (std::is_same_v<T, CommandSetVariable>)
          return "setVariable";
        if constexpr (std::is_same_v<T, CommandSetDebugVariable>)
          return "setDebugVariable";
        if constexpr (std::is_same_v<T, CommandGetIntVariable>)
          return "getIntVariable";
        if constexpr (std::is_same_v<T, CommandGetBoolVariable>)
          return "getBoolVariable";
        if constexpr (std::is_same_v<T, CommandGetDoubleVariable>)
          return "getDoubleVariable";
        if constexpr (std::is_same_v<T, CommandGetStringVariable>)
          return "getStringVariable";
        if constexpr (std::is_same_v<T, CommandSetInputName>)
          return "setInputName";
        if constexpr (std::is_same_v<T, CommandGetInputName>)
          return "getInputName";
        if constexpr (std::is_same_v<T, CommandSetOutputName>)
          return "setOutputName";
        if constexpr (std::is_same_v<T, CommandGetDataPath>)
          return "getDataPath";
        if constexpr (std::is_same_v<T, CommandSetInputImage>)
          return "setInputImage";
        if constexpr (std::is_same_v<T, CommandGetInputImage>)
          return "getInputImage";
        if constexpr (std::is_same_v<T, CommandSetPageMode>)
          return "setPageMode";
        if constexpr (std::is_same_v<T, CommandSetRectangle>)
          return "setRectangle";
        if constexpr (std::is_same_v<T, CommandSetSourceResolution>)
          return "setSourceResolution";
        if constexpr (std::is_same_v<T, CommandGetSourceYResolution>)
          return "getSourceYResolution";
        if constexpr (std::is_same_v<T, CommandSetImage>)
          return "setImage";
        if constexpr (std::is_same_v<T, CommandGetThresholdedImage>)
          return "getThresholdedImage";
        if constexpr (std::is_same_v<T, CommandGetThresholdedImageScaleFactor>)
          return "getThresholdedImageScaleFactor";
        if constexpr (std::is_same_v<T, CommandRecognize>)
          return "recognize";
        if constexpr (std::is_same_v<T, CommandAnalyseLayout>)
          return "analyseLayout";
        if constexpr (std::is_same_v<T, CommandDetectOrientationScript>)
          return "detectOrientationScript";
        if constexpr (std::is_same_v<T, CommandMeanTextConf>)
          return "meanTextConf";
        if constexpr (std::is_same_v<T, CommandAllWordConfidences>)
          return "allWordConfidences";
        if constexpr (std::is_same_v<T, CommandGetUTF8Text>)
          return "getUTF8Text";
        if constexpr (std::is_same_v<T, CommandGetHOCRText>)
          return "getHOCRText";
        if constexpr (std::is_same_v<T, CommandGetTSVText>)
          return "getTSVText";
        if constexpr (std::is_same_v<T, CommandGetUNLVText>)
          return "getUNLVText";
        if constexpr (std::is_same_v<T, CommandGetALTOText>)
          return "getALTOText";
        if constexpr (std::is_same_v<T, CommandGetPAGEText>)
          return "getPAGEText";
        if constexpr (std::is_same_v<T, CommandGetLSTMBoxText>)
          return "getLSTMBoxText";
        if constexpr (std::is_same_v<T, CommandGetBoxText>)
          return "getBoxText";
        if constexpr (std::is_same_v<T, CommandGetWordStrBoxText>)
          return "getWordStrBoxText";
        if constexpr (std::is_same_v<T, CommandGetOSDText>)
          return "getOSDText";
        if constexpr (std::is_same_v<T, CommandBeginProcessPages>)
          return "beginProcessPages";
        if constexpr (std::is_same_v<T, CommandAddProcessPage>)
          return "addProcessPage";
        if constexpr (std::is_same_v<T, CommandFinishProcessPages>)
          return "finishProcessPages";
        if constexpr (std::is_same_v<T, CommandAbortProcessPages>)
          return "abortProcessPages";
        if constexpr (std::is_same_v<T, CommandGetInitLanguages>)
          return "getInitLanguages";
        if constexpr (std::is_same_v<T, CommandGetLoadedLanguages>)
          return "getLoadedLanguages";
        if constexpr (std::is_same_v<T, CommandGetAvailableLanguages>)
          return "getAvailableLanguages";
        if constexpr (std::is_same_v<T, CommandClearPersistentCache>)
          return "clearPersistentCache";
        if constexpr (std::is_same_v<T, CommandClearAdaptiveClassifier>)
          return "clearAdaptiveClassifier";
        if constexpr (std::is_same_v<T, CommandClear>)
          return "clear";
        if constexpr (std::is_same_v<T, CommandEnd>)
          return "end";
        return "unknown";
      },
      command);
}

} // namespace

WorkerThread::WorkerThread(Napi::Env env)
    : _env(env),
      _main_thread(Napi::ThreadSafeFunction::New(
          env, Napi::Function::New(env, [](const Napi::CallbackInfo &) {}),
          "main_thread_callback", 0, 1)) {
  _worker_thread =
      std::jthread([this](std::stop_token token) { this->Run(token); });
}

WorkerThread::~WorkerThread() {
  _worker_thread.request_stop();
  _queue_cv.notify_all();
  if (_worker_thread.joinable()) {
    _worker_thread.join();
  }
}

void WorkerThread::MakeCallback(std::shared_ptr<Job> *p_job) {
  auto status = _main_thread.NonBlockingCall(
      p_job, [](Napi::Env env, Napi::Function /* unused */,
                std::shared_ptr<Job> *_job) {
        // break the reference to the underlying job reference
        std::shared_ptr<Job> job = *_job;
        delete _job;

        if (job->error.has_value()) {
          Napi::Error error = Napi::Error::New(env, *job->error);
          if (job->error_code.has_value()) {
            error.Set("code", Napi::String::New(env, *job->error_code));
          }
          if (job->error_method.has_value()) {
            error.Set("method", Napi::String::New(env, *job->error_method));
          }
          job->deffered.Reject(error.Value());
          return;
        }

        if (!job->result.has_value()) {
          job->deffered.Resolve(env.Undefined());
          return;
        }

        job->deffered.Resolve(MatchResult(env, *job->result));
      });

  if (status != napi_ok) {
    return;
  }
}

void WorkerThread::Run(std::stop_token token) {
  std::optional<ProcessPagesSession> process_pages_session;

  auto drain_queue = [&](std::vector<std::shared_ptr<Job>> &pending_jobs) {
    while (!_request_queue.empty()) {
      pending_jobs.push_back(_request_queue.front());
      _request_queue.pop();
    }
  };
  auto reject_jobs =
      [&](const char *message,
          const std::vector<std::shared_ptr<Job>> &pending_jobs) {
        for (const auto &pending_job : pending_jobs) {
          pending_job->error = message;
          pending_job->error_code = "ERR_WORKER_STOPPED";
          pending_job->error_method = CommandName(pending_job->command);
          auto *sp_pending_job = new std::shared_ptr<Job>(pending_job);
          MakeCallback(sp_pending_job);
        }
      };

  while (true) {
    std::shared_ptr<Job> job;
    {
      std::unique_lock<std::mutex> lock(_queue_mutex);
      std::vector<std::shared_ptr<Job>> pending_jobs;

      // wait until queue is not empty anymore, or stop was requested
      _queue_cv.wait(lock, [&] {
        return !_request_queue.empty() || token.stop_requested();
      });
      if (token.stop_requested()) {
        drain_queue(pending_jobs);
        lock.unlock();
        reject_jobs("Worker stopped accepting new Commands", pending_jobs);
        break;
      }

      // if (_request_queue.empty()) {
      //   break;
      // }

      job = _request_queue.front();
      _request_queue.pop();
    };

    try {
      job->result = std::visit(
          [&](const auto &command) -> Result {
            if constexpr (requires {
                            command.invoke(_api, process_pages_session);
                          }) {
              return command.invoke(_api, process_pages_session);
            } else {
              return command.invoke(_api);
            }
          },
          job->command);
    } catch (const std::exception &error) {
      job->error = error.what();
      job->error_code = "ERR_TESSERACT_RUNTIME";
      job->error_method = CommandName(job->command);
    } catch (...) {
      job->error = "Something unexpected happened";
      job->error_code = "ERR_TESSERACT_RUNTIME";
      job->error_method = CommandName(job->command);
    }

    auto *sp_job = new std::shared_ptr<Job>(job);
    MakeCallback(sp_job);

    if (token.stop_requested() ||
        std::holds_alternative<CommandEnd>(job->command)) {
      std::vector<std::shared_ptr<Job>> pending_jobs;
      {
        std::unique_lock<std::mutex> lock(_queue_mutex);
        drain_queue(pending_jobs);
      }
      reject_jobs("Worker stopped accepting new Commands", pending_jobs);
      break;
    }
  };

  _api.End();
  _main_thread.Release();
};
