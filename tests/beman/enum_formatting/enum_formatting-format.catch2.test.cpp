// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <beman/enum_formatting/config.hpp>
#include <catch2/catch_all.hpp>

#if BEMAN_ENUM_FORMATTING_USE_MODULES()
import std;
import beman.enum_formatting;
#else
    #include <beman/enum_formatting/format.hpp>

    #include <cstdint>
    #include <format>
    #include <meta>
    #include <string>
#endif

using namespace beman::enum_formatting::annotations;

TEST_CASE("std::formatter is available only for annotated scoped enumerations", "[unit][enum_formatting][std::formatter]")
{
    enum class unannotated {};

    enum unscoped {};
    enum class[[= formattable]] annotated {};

    CHECK(std::formattable<annotated, char>);
    CHECK_FALSE(std::formattable<unannotated, char>);
    CHECK_FALSE(std::formattable<unscoped, char>);
}

TEST_CASE("formatter parses valid specifications", "[unit][enum_formatting][formatter][parse]")
{
    enum class[[= formattable]] byte : std::uint8_t {};

    std::formatter<byte> formatter;

    SECTION("bare format specification string")
    {
        SECTION("string representation")
        {
            std::format_parse_context context{"s:>10"};
            CHECK(formatter.parse(context) == context.end());
        }

        SECTION("integer representation")
        {
            std::format_parse_context context{"iq:#04X"};
            CHECK(formatter.parse(context) == context.end());
        }

        SECTION("type representation")
        {
            std::format_parse_context context{"t:<12"};
            CHECK(formatter.parse(context) == context.end());
        }
    }
    SECTION("replacement field format string")
    {
        std::format_parse_context ctx{"i:#04x}) {:t}"};
        CHECK(std::string_view{formatter.parse(ctx), ctx.end()} == "}) {:t}");
    }
}

TEST_CASE("formatter rejects invalid representation combinations", "[unit][enum_formatting][formatter][parse]")
{
    enum class[[= formattable]] byte {};

    std::formatter<byte> formatter;

    CHECK_THROWS_AS(
        std::invoke([&] {
            auto ctx = std::format_parse_context{"x:>10"};
            formatter.parse(ctx);
        }),
        std::format_error
    );

    CHECK_THROWS_AS(
        std::invoke([&] {
            auto ctx = std::format_parse_context{"sx:>10"};
            formatter.parse(ctx);
        }),
        std::format_error
    );

    CHECK_THROWS_AS(
        std::invoke([&] {
            auto ctx = std::format_parse_context{"ix:#04x"};
            formatter.parse(ctx);
        }),
        std::format_error
    );

    CHECK_THROWS_AS(
        std::invoke([&] {
            auto ctx = std::format_parse_context{"tq"};
            formatter.parse(ctx);
        }),
        std::format_error
    );

    CHECK_THROWS_AS(
        std::invoke([&] {
            auto ctx = std::format_parse_context{"tu"};
            formatter.parse(ctx);
        }),
        std::format_error
    );
}

TEST_CASE("std::format formats annotated scoped enumerations", "[integration][enum_formatting][std::formatter]")
{
    enum class[[= formattable]] arbitrary_color : std::uint8_t {
        red   = 42,
        green = 128,
        blue  = 255,
    };

    SECTION("default string representation")
    {
        CHECK(std::format("{}", arbitrary_color::red) == "red");
        CHECK(std::format("{:}", arbitrary_color::green) == "green");

        CHECK(std::format("{}", static_cast<arbitrary_color>(7)) == "arbitrary_color::7");
    }

    SECTION("representation selection")
    {
        CHECK(std::format("{:s}", arbitrary_color::red) == "red");
        CHECK(std::format("{:i}", arbitrary_color::red) == "42");
        CHECK(std::format("{:t}", arbitrary_color::red) == "arbitrary_color");
    }

    SECTION("qualification")
    {
        CHECK(std::format("{:sq}", arbitrary_color::red) == "arbitrary_color::red");
        CHECK(std::format("{:su}", arbitrary_color::red) == "red");

        CHECK(std::format("{:iq}", arbitrary_color::red) == "arbitrary_color::42");
        CHECK(std::format("{:iu}", arbitrary_color::red) == "42");
    }

    SECTION("qualification of unnamed values")
    {
        auto value = static_cast<arbitrary_color>(7);

        CHECK(std::format("{:s}", value) == "arbitrary_color::7");
        CHECK(std::format("{:sq}", value) == "arbitrary_color::7");
        CHECK(std::format("{:su}", value) == "7");

        CHECK(std::format("{:i}", value) == "7");
        CHECK(std::format("{:iq}", value) == "arbitrary_color::7");
        CHECK(std::format("{:iu}", value) == "7");
    }
}

TEST_CASE(
    "std::formatter forwards representation-specific format specifications",
    "[integration][enum_formatting][std::formatter]"
)
{
    enum class[[= formattable]] test_enumeration : std::uint8_t {
        enumerator = 42,
    };

    SECTION("string representation")
    {
        CHECK(std::format("{:s:>10}", test_enumeration::enumerator) == std::format("{:>10}", "enumerator"));
        CHECK(std::format("{:s:.2}", test_enumeration::enumerator) == std::format("{:.2}", "enumerator"));
    }

    SECTION("integer representation")
    {
        CHECK(
            std::format("{:i:04}", test_enumeration::enumerator)
            == std::format("{:04}", std::to_underlying(test_enumeration::enumerator))
        );
        CHECK(
            std::format("{:i:#04x}", test_enumeration::enumerator)
            == std::format("{:#04x}", std::to_underlying(test_enumeration::enumerator))
        );
    }

    SECTION("type representation")
    {
        CHECK(std::format("{:t:>10}", test_enumeration::enumerator) == std::format("{:>10}", "test_enumeration"));
    }
}

TEST_CASE("std::formatter uses annotation-provided default specifications", "[integration][enum_formatting][std::formatter]")
{
    enum class[[= formattable_with_default("i:#04X")]] test_enumeration : std::uint8_t {
        test = 7,
    };

    SECTION("annotation default is used when no specification is provided")
    {
        CHECK(std::format("{}", test_enumeration::test) == std::format("{:#04X}", std::to_underlying(test_enumeration::test)));
        CHECK(std::format("{:}", test_enumeration::test) == std::format("{:#04X}", std::to_underlying(test_enumeration::test)));
    }

    SECTION("explicit specification overrides annotation default")
    {
        CHECK(std::format("{:s}", test_enumeration::test) == std::format("{}", "test"));
        CHECK(
            std::format("{:i:04}", test_enumeration::test) == std::format("{:04}", std::to_underlying(test_enumeration::test))
        );
    }
}

TEST_CASE("std::formatter uses canonical enumerator names", "[integration][enum_formatting][std::formatter]")
{
    enum class[[= formattable]] color : std::uint32_t {
        red                  = 0xFF0000,
        green                = 0x00FF00,
        blue                 = 0x0000FF,
        gules[[= canonical]] = 0xFF0000,
        vert                 = 0x00FF00,
        azure[[= canonical]] = 0x0000FF,
        azul[[= canonical]]  = 0x0000FF,
    };

    SECTION("canonical names are preferred")
    {
        CHECK(std::format("{}", color::red) == "gules");
        CHECK(std::format("{}", color::blue) == "azure");
    }

    SECTION("first declared name is used when no canonical name exists")
    {
        CHECK(std::format("{}", color::green) == "green");
    }

    SECTION("qualification applies to the selected name")
    {
        CHECK(std::format("{:sq}", color::red) == "color::gules");
        CHECK(std::format("{:su}", color::red) == "gules");
    }
}
