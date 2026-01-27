#include "commands.hpp"
#include "third_party/catch2/catch_amalgamated.hpp"
#include "utils.hpp"
#include <stdexcept>
#include <utility>

TEST_CASE("throw_runtime formats message") {
  REQUIRE_THROWS_WITH(throw_runtime("hello {}", "world"), "hello world");
}

TEST_CASE("CommandInit rejects mismatched vars vectors") {
  tesseract::TessBaseAPI api;
  CommandInit cmd;
  cmd.vars_vec = {"foo"};

  REQUIRE_THROWS_WITH(
      cmd.invoke(api),
      "vars_vec and vars_values must both be set and same length");
}
