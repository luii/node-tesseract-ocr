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
#include <thread>
#include <utility>
#include <variant>

WorkerThread::WorkerThread(Napi::Env env)
    : _env(env),
      _main_thread(Napi::ThreadSafeFunction::New(
          env, Napi::Function::New(env, [](const Napi::CallbackInfo &) {}),
          "main_thread_callback", 0, 1)) {
  _worker_thread = std::jthread([this] { this->Run(); });
}

WorkerThread::~WorkerThread() {
  Terminate();
  _worker_thread.request_stop();
  _main_thread.Release();
}

void WorkerThread::Terminate() {
  {
    std::scoped_lock<std::mutex> lock(_queue_mutex);
    _stop.store(true);
  }
  _queue_cv.notify_all();
};

template <typename C> Napi::Promise WorkerThread::Enqueue(C &&command) {
  Napi::Promise::Deferred deffered = Napi::Promise::Deferred::New(_env);
  auto job =
      std::make_shared<Job>(Job{Command{std::forward<C>(command)}, deffered});

  {
    std::scoped_lock<std::mutex> lock(_queue_mutex);
    _request_queue.push(job);
  };

  _queue_cv.notify_one();
  return deffered.Promise();
}

void WorkerThread::Run() {
  std::shared_ptr<Job> job;

  do {
    {
      std::unique_lock<std::mutex> lock(_queue_mutex);
      _queue_cv.wait(lock, [&] { return !_request_queue.empty() || _stop; });
      job = _request_queue.front();
      _request_queue.pop();

      if (_stop && _request_queue.empty()) {
        return;
      }
    };

    try {
      job->result = std::visit(
          [&](const auto &command) -> Result { return command.invoke(_api); },
          job->command);
    } catch (const std::exception &error) {
      job->error = error.what();
    }

    auto *sp_job = new std::shared_ptr<Job>(job);
    _main_thread.BlockingCall(sp_job, [](Napi::Env env,
                                         Napi::Function /* unused */,
                                         std::shared_ptr<Job> *p_job) {
      std::shared_ptr<Job> j = *p_job;
      delete p_job;

      if (j->error.has_value()) {
        j->deffered.Reject(Napi::Error::New(env, *j->error).Value());
        return;
      }

      if (!j->result.has_value()) {
        j->deffered.Resolve(env.Undefined());
        return;
      }

      j->deffered.Resolve(MatchResult(env, *j->result));
    });
  } while (!std::holds_alternative<CommandEnd>(job->command));
};
