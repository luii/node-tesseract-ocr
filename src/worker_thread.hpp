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
#include <tesseract/baseapi.h>
#include <thread>

class WorkerThread {
public:
  explicit WorkerThread(Napi::Env env);
  ~WorkerThread();

  template <typename C> Napi::Promise Enqueue(C &&command);
  void Terminate();

private:
  void Run();

private:
  Napi::Env _env;
  Napi::ThreadSafeFunction _main_thread;

  std::atomic<bool> _stop{false};
  std::mutex _queue_mutex;
  std::condition_variable _queue_cv;
  std::queue<std::shared_ptr<Job>> _request_queue;

  tesseract::TessBaseAPI _api;

  std::jthread _worker_thread;
};
