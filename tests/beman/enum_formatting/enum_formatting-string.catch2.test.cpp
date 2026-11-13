// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <beman/enum_formatting/config.hpp>
#include <catch2/catch_all.hpp>

#if BEMAN_ENUM_FORMATTING_USE_MODULES()
import std;
import beman.enum_formatting;
#else
    #include <beman/enum_formatting/string.hpp>

    #include <cstdint>
    #include <meta>
    #include <string>
#endif

template<typename T>
concept has_to_string = requires(T value) { beman::enum_formatting::to_string(value); };

TEST_CASE("to_string is only available for scoped enumerations", "[unit][enum_formatting][to_string]")
{
    enum class scoped_enumeration {
    };

    enum unscoped_enumeration {
    };

    CHECK(has_to_string<scoped_enumeration>);
    CHECK_FALSE(has_to_string<unscoped_enumeration>);
}

TEST_CASE("to_string returns the name of a named enumerator", "[unit][enum_formatting][to_string]")
{
    using beman::enum_formatting::to_string;

    enum class color : std::uint8_t {
        red,
        green,
        blue,
    };

    //Check both during constant evaluation and runtime execution.
    STATIC_CHECK(to_string(color::red) == "red");
    CHECK(to_string(color::red) == "red");
    STATIC_CHECK(to_string(color::green) == "green");
    CHECK(to_string(color::green) == "green");
    STATIC_CHECK(to_string(color::blue) == "blue");
    CHECK(to_string(color::blue) == "blue");
}

TEST_CASE("to_string enumerator name selection", "[unit][enum_formatting][to_string]")
{
    using beman::enum_formatting::canonical;
    using beman::enum_formatting::to_string;

    enum class color : std::uint32_t {
        red                  = 0xFF0000,
        green                = 0x00FF00,
        blue                 = 0x0000FF,
        gules[[= canonical]] = 0xFF0000,
        vert                 = 0x00FF00,
        azure[[= canonical]] = 0x0000FF,
        azul[[= canonical]]  = 0x0000FF,
    };

    SECTION("to_string selects the first canonical enumerator alias when designated")
    {
        CHECK(to_string(color::red) == "gules");
        CHECK(to_string(color::gules) == "gules");
        CHECK(to_string(static_cast<color>(0xFF0000)) == "gules");
        CHECK(to_string(color::blue) == "azure");
        CHECK(to_string(color::azure) == "azure");
        CHECK(to_string(color::azul) == "azure");
        CHECK(to_string(static_cast<color>(0x0000FF)) == "azure");
    }
    SECTION("to_string selects the first declared enumerator alias when none is canonical")
    {
        CHECK(to_string(color::green) == "green");
        CHECK(to_string(color::vert) == "green");
        CHECK(to_string(static_cast<color>(0x00FF00)) == "green");
    }
}

TEST_CASE(
    "to_string falls back to integers qualified with the enumeration name for unenumerated values",
    "[unit][enum_formatting][to_string]"
)
{
    using beman::enum_formatting::to_string;

    enum class rgb_color : std::uint32_t {
        red   = 0xFF0000,
        green = 0x00FF00,
        blue  = 0x0000FF,
    };

    auto my_red    = static_cast<rgb_color>(16711680); //0xFF0000
    auto my_white  = static_cast<rgb_color>(0);        //0x000000
    auto my_blue   = static_cast<rgb_color>(255);      //0x0000FF
    auto my_orange = static_cast<rgb_color>(16753920); //0xFFA500
    auto my_black  = static_cast<rgb_color>(16777215); //0xFFFFFF

    CHECK(to_string(my_red) == "red");
    CHECK(to_string(my_white) == "rgb_color::0");
    CHECK(to_string(my_blue) == "blue");
    CHECK(to_string(my_orange) == "rgb_color::16753920");
    CHECK(to_string(my_black) == "rgb_color::16777215");

    enum class direction : std::int8_t {
        left    = -1,
        forward = 0,
        right   = 1,
    };

    CHECK(to_string(static_cast<direction>(-42)) == "direction::-42");
}
