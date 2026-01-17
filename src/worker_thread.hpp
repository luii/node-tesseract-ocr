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

#pragma once

#include "commands.hpp"
#include <atomic>
#include <condition_variable>
#include <memory>
#include <napi.h>
#include <queue>
#include <stop_token>
#include <tesseract/baseapi.h>
#include <thread>
#include <variant>

class WorkerThread {
public:
  explicit WorkerThread(Napi::Env env);
  ~WorkerThread();

  template <typename C> Napi::Promise Enqueue(C &&command);

private:
  void Run(std::stop_token token);
  void MakeCallback(std::shared_ptr<Job> *job);

private:
  Napi::Env _env;
  Napi::ThreadSafeFunction _main_thread;

  // for graceful shutdown of the worker
  std::atomic<bool> _closing{false};
  std::mutex _queue_mutex;
  std::condition_variable _queue_cv;
  std::queue<std::shared_ptr<Job>> _request_queue;

  tesseract::TessBaseAPI _api;

  std::jthread _worker_thread;
};

template <typename C> Napi::Promise WorkerThread::Enqueue(C &&command) {
  Napi::Promise::Deferred deferred = Napi::Promise::Deferred::New(_env);
  auto job = std::make_shared<Job>(Job{Command{std::forward<C>(command)},
                                       deferred, std::nullopt, std::nullopt});

  {
    std::scoped_lock<std::mutex> lock(_queue_mutex);
    std::stop_token token = _worker_thread.get_stop_token();

    if (_closing.load() || token.stop_requested()) {
      deferred.Reject(Napi::Error::New(_env, "Worker is closing").Value());
      return deferred.Promise();
    }

    _request_queue.push(job);

    if (std::holds_alternative<CommandEnd>(job->command)) {
      _closing.store(true);
    }
  };

  _queue_cv.notify_one();
  return deferred.Promise();
}
