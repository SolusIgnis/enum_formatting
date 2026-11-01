// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <beman/enum_formatting/config.hpp>
#include <catch2/catch_all.hpp>

#if BEMAN_ENUM_FORMATTING_USE_MODULES()
import std;
import beman.enum_formatting;
#else
#include <beman/enum_formatting/annotations.hpp>

#include <concepts>
#include <meta>
#include <string_view>
#include <type_traits>
#endif

TEST_CASE("formattable_with_default construction", "[unit][enum_formatting][annotations][formattable_with_default]")
{
    using beman::enum_formatting::formattable_with_default;

    SECTION("construction requires a bounded character array")
    {
        CHECK(std::constructible_from<formattable_with_default, decltype("")>);
        CHECK(std::constructible_from<formattable_with_default, decltype("s:>20.40")>);
        CHECK(std::constructible_from<formattable_with_default, decltype("i:08b")>);
        CHECK(std::constructible_from<formattable_with_default, decltype("The quick brown fox jumps over the lazy dog. ABCDEFGHIJKLMNOPQRSTUVWXYZ abcdefghijklmnopqrstuvwxyz 0123456789")>);
        CHECK_FALSE(std::constructible_from<formattable_with_default, const char*>);
        CHECK_FALSE(std::constructible_from<formattable_with_default, char*>);
    }

    SECTION("default construction")
    {
        constexpr formattable_with_default annotation{};

        REQUIRE(annotation.please_use_other_door != nullptr);
        CHECK(std::string_view{annotation.please_use_other_door}.empty());
    }

    SECTION("construction with a format specification")
    {
        constexpr formattable_with_default annotation{"i:#04X"};

        REQUIRE(annotation.please_use_other_door[0] == 'i');
        REQUIRE(annotation.please_use_other_door[1] == ':');
        REQUIRE(annotation.please_use_other_door[2] == '#');
        REQUIRE(annotation.please_use_other_door[3] == '0');
        REQUIRE(annotation.please_use_other_door[4] == '4');
        REQUIRE(annotation.please_use_other_door[5] == 'X');
        REQUIRE(annotation.please_use_other_door[6] == '\0');
    }

    SECTION("construction with an implausibly long string")
    {
        constexpr char expected[] = "The quick brown fox jumps over the lazy dog. ABCDEFGHIJKLMNOPQRSTUVWXYZ abcdefghijklmnopqrstuvwxyz 0123456789";
        constexpr formattable_with_default annotation{expected};

        CHECK(std::string_view{annotation.please_use_other_door} == std::string_view{expected});
    }
}

TEST_CASE("formattable_with_default std::string_view implicit conversion yields format specification", "[unit][enum_formatting][annotations][formattable_with_default]") {
    using beman::enum_formatting::formattable_with_default;

    constexpr char expected[] = "#010b";
    constexpr auto annotation = formattable_with_default{expected};

    constexpr std::string_view specification = annotation;

    CHECK(specification == std::string_view{expected});
}

TEST_CASE("formattable_with_default format specification has static storage duration", "[unit][enum_formatting][annotations][formattable_with_default]") {
    using beman::enum_formatting::formattable_with_default;

    auto make_format_specification = [] consteval {
        constexpr char specification_array[] = "#010b";
        return formattable_with_default{specification_array};
    };
    
    constexpr std::string_view specification = make_format_specification();
   
    STATIC_CHECK(specification == "#010b"); //STATIC_CHECK employs constant evaluation to fail the check if this access is Undefined Behavior.
    CHECK(specification == "#010b");
}

TEST_CASE("formattable annotation object is formattable_with_default(\"\")", "[unit][enum_formatting][annotations][formattable]") {
    using beman::enum_formatting::formattable_with_default;
    using beman::enum_formatting::formattable;

    CHECK(std::same_as<
        std::remove_cv_t<decltype(formattable)>,
        formattable_with_default
    >);
    CHECK(std::string_view{formattable}.empty());
}

TEST_CASE("formattable_with_default compares equality by format specification equivalence", "[unit][enum_formatting][annotations][formattable_with_default][formattable]")
{
    using beman::enum_formatting::formattable_with_default;
    using beman::enum_formatting::formattable;

    SECTION("formattable_with_default is equality comparable")
    {
        CHECK(std::equality_comparable<formattable_with_default>);
    }

    SECTION("equality compares format specification strings")
    {
        constexpr auto stringy_format_a = formattable_with_default{"s:>20"};
        constexpr auto stringy_format_b = formattable_with_default{"s:>20"};
        constexpr auto stringy_format_c = formattable_with_default{">20"};
        constexpr auto default_format_a = formattable_with_default{""};
        constexpr auto default_format_b = formattable_with_default{":"};
        
        CHECK(stringy_format_a == stringy_format_b);
        CHECK(stringy_format_a != stringy_format_c);
        CHECK(stringy_format_b != stringy_format_c);
        CHECK(default_format_a != default_format_b);

        CHECK_FALSE(stringy_format_a != stringy_format_b);
        CHECK_FALSE(stringy_format_a == stringy_format_c);
        CHECK_FALSE(stringy_format_b == stringy_format_c);
        CHECK_FALSE(default_format_a == default_format_b);
    }
    
    SECTION("formattable is equivalent to an empty format specification")
    {
        CHECK(formattable == formattable);
        CHECK(formattable == formattable_with_default{});
        CHECK(formattable == formattable_with_default{""});
        CHECK(formattable != formattable_with_default{":"});
    }
}

TEST_CASE("formattable_with_default and formattable interoperate with [[=...]] annotations and static reflection", "[integration][enum_formatting][annotations][formattable_with_default][formattable]")
{
    using beman::enum_formatting::formattable_with_default;
    using beman::enum_formatting::formattable;

    enum class [[=formattable]] monochrome_color : std::uint8_t {
        black,
        white,
    };
    
    enum class [[=formattable_with_default("i:#04X")]] q_word : std::uint64_t {};

    auto get_formattable_annotation = [](std::meta::info reflection) consteval {
        return std::meta::extract<formattable_with_default>(std::meta::annotations_of_with_type(reflection, ^^formattable_with_default).front());
    };

    SECTION("formattable annotation is extractable")
    {
        constexpr auto annotation = get_formattable_annotation(^^monochrome_color);
        CHECK(annotation == formattable);
    }

    SECTION("formattable_with_default annotation is extractable")
    {
        constexpr auto annotation = get_formattable_annotation(^^q_word);
        CHECK(annotation == formattable_with_default("i:#04X"));
    }

    SECTION("extracted formattable_with_default annotation preserves format specification")
    {
        constexpr std::string_view specification = get_formattable_annotation(^^q_word);
        CHECK(specification == "i:#04X");
    }
}

TEST_CASE("canonical annotation object interoperates with [[=...]] annotations and static reflection", "[integration][enum_formatting][annotations][canonical]")
{
    using beman::enum_formatting::formattable;
    using beman::enum_formatting::canonical;

    enum class [[=formattable]] color : std::uint32_t {
                       red   = 0xFF0000,
        [[=canonical]] gules = 0xFF0000, 
                       green = 0x00FF00, 
        [[=canonical]] vert  = 0x00FF00, 
                       blue  = 0x0000FF,
        [[=canonical]] azure = 0x0000FF,
    };
    
    CHECK(std::meta::annotations_of_with_type(^^color::red, ^^canonical).empty());
    CHECK(std::meta::annotations_of_with_type(^^color::gules, ^^canonical).size() == 1);
}
