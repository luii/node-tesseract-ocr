#pragma once

#include <format>
template <typename... Args>
[[noreturn]] void throw_runtime(std::format_string<Args...> fmt,
                                Args &&...args) {
  throw std::runtime_error(std::format(fmt, std::forward<Args>(args)...));
}
