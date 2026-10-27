// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <beman/enum_formatting/config.hpp>
#include <catch2/catch_all.hpp>
#include <beman/enum_formatting/todo.hpp>

TEST_CASE("todo", "[enum_formatting::todo]") {
    const bool todo = true;
    CHECK(todo);
}
