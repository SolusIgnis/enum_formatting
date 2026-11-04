// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <beman/enum_formatting/config.hpp>
#include <catch2/catch_all.hpp>

#if BEMAN_ENUM_FORMATTING_USE_MODULES()
import std;
import beman.enum_formatting;
#else
#include <beman/enum_formatting/utility.hpp>

#include <cstdint>
#include <meta>
#endif

TEST_CASE("is_enumerated_value validates if a value corresponds to a named enumerator",
          "[unit][enum_formatting][is_enumerated_value]")
{
    using beman::enum_formatting::is_enumerated_value;

    SECTION("unsigned enumerators with aliases")
    {
        enum class color : std::uint32_t {
            red   = 0xFF0000,
            gules = 0xFF0000,
            green = 0x00FF00,
            vert  = 0x00FF00,
            blue  = 0x0000FF,
        };

        auto my_red1   = color::red;
        auto my_red2   = color::gules;
        auto my_white  = static_cast<color>(0x000000);
        auto my_blue   = static_cast<color>(0x0000FF);
        auto my_orange = static_cast<color>(0xFFA500);
        auto my_black  = static_cast<color>(0xFFFFFF);
        CHECK(is_enumerated_value(color::green));
        CHECK(is_enumerated_value(color::vert));
        CHECK(is_enumerated_value(my_red1));
        CHECK(is_enumerated_value(my_red2));
        CHECK(is_enumerated_value(my_blue));
        CHECK_FALSE(is_enumerated_value(my_white));
        CHECK_FALSE(is_enumerated_value(my_orange));
        CHECK_FALSE(is_enumerated_value(my_black));
    }

    SECTION("signed enumerators")
    {
        enum class signed_value : std::int8_t {
            negative = -1,
            zero     = 0,
            positive = 1,
        };

        CHECK(is_enumerated_value(signed_value::negative));
        CHECK_FALSE(is_enumerated_value(static_cast<signed_value>(-2)));
    }
}
