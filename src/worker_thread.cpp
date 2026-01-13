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
#include <queue>
#include <stop_token>
#include <thread>
#include <variant>

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
  _main_thread.BlockingCall(p_job, [](Napi::Env env,
                                      Napi::Function /* unused */,
                                      std::shared_ptr<Job> *_job) {
    // break the reference to the underlying job reference
    std::shared_ptr<Job> job = *_job;
    delete _job;

    if (job->error.has_value()) {
      job->deffered.Reject(Napi::Error::New(env, *job->error).Value());
      return;
    }

    if (!job->result.has_value()) {
      job->deffered.Resolve(env.Undefined());
      return;
    }

    job->deffered.Resolve(MatchResult(env, *job->result));
  });
}

void WorkerThread::Run(std::stop_token token) {
  std::shared_ptr<Job> job = nullptr;

  do {
    job = nullptr;
    {
      std::unique_lock<std::mutex> lock(_queue_mutex);

      // wait until queue is not empty anymore, or stop was requested
      _queue_cv.wait(lock, [&] {
        return !_request_queue.empty() || token.stop_requested();
      });
      if (token.stop_requested()) {
        while (!_request_queue.empty()) {
          std::shared_ptr<Job> job = _request_queue.front();
          _request_queue.pop();

          job->error = "Worker stopped accepting new Commands";
          auto *sp_pending_job = new std::shared_ptr<Job>(job);
          MakeCallback(sp_pending_job);
        }
        break;
      }

      if (_request_queue.empty())
        break;

      job = _request_queue.front();
      _request_queue.pop();
    };

    try {
      job->result = std::visit(
          [&](const auto &command) -> Result { return command.invoke(_api); },
          job->command);
    } catch (const std::exception &error) {
      job->error = error.what();
    }

    auto *sp_job = new std::shared_ptr<Job>(job);
    MakeCallback(sp_job);
  } while (!token.stop_requested() && job != nullptr &&
           !std::holds_alternative<CommandEnd>(job->command));
  _main_thread.Release();
};
