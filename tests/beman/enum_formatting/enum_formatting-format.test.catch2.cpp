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

TEST_CASE("std::formatter is available only for annotated scoped enumerations",
          "[unit][enum_formatting][std::formatter]")
{
    enum class unannotated {};
    enum unscoped {};
    enum class [[=beman::enum_formatting::formattable]] annotated {};

    CHECK(std::formattable<annotated, char>);
    CHECK_FALSE(std::formattable<unannotated, char>);
    CHECK_FALSE(std::formattable<unscoped, char>);
}

TEST_CASE("formatter parses valid specifications",
          "[unit][enum_formatting][formatter][parse]")
{
    enum class [[=beman::enum_formatting::formattable]] byte : std::uint8_t {};

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

TEST_CASE("formatter rejects invalid representation combinations",
          "[unit][enum_formatting][formatter][parse]")
{
    enum class [[=beman::enum_formatting::formattable]] byte {};

    std::formatter<byte> formatter;

    CHECK_THROWS_AS(
        formatter.parse(std::format_parse_context{"x:>10"}),
        std::format_error
    );

    CHECK_THROWS_AS(
        formatter.parse(std::format_parse_context{"sx:>10"}),
        std::format_error
    );
    
    CHECK_THROWS_AS(
        formatter.parse(std::format_parse_context{"ix:#04x"}),
        std::format_error
    );

    CHECK_THROWS_AS(
        formatter.parse(std::format_parse_context{"tq"}),
        std::format_error
    );
    
    CHECK_THROWS_AS(
        formatter.parse(std::format_parse_context{"tu"}),
        std::format_error
    );
}

TEST_CASE("std::format formats annotated scoped enumerations",
          "[integration][enum_formatting][std::formatter]")
{
    enum class [[=beman::enum_formatting::formattable]] byte : std::uint8_t {
        red   = 42,
        green = 128,
        blue  = 255,
    };

    SECTION("default string representation")
    {
        CHECK(std::format("{}", byte::red) == "red");
        CHECK(std::format("{:}", byte::green) == "green");

        CHECK(std::format("{}", static_cast<byte>(7)) == "byte::7");
    }

    SECTION("representation selection")
    {
        CHECK(std::format("{:s}", byte::red) == "red");
        CHECK(std::format("{:i}", byte::red) == "42");
        CHECK(std::format("{:t}", byte::red) == "byte");
    }

    SECTION("qualification")
    {
        CHECK(std::format("{:sq}", byte::red) == "byte::red");
        CHECK(std::format("{:su}", byte::red) == "red");

        CHECK(std::format("{:iq}", byte::red) == "byte::42");
        CHECK(std::format("{:iu}", byte::red) == "42");
    }

    SECTION("qualification of unnamed values")
    {
        auto value = static_cast<byte>(7);

        CHECK(std::format("{:s}", value) == "byte::7");
        CHECK(std::format("{:sq}", value) == "byte::7");
        CHECK(std::format("{:su}", value) == "7");

        CHECK(std::format("{:i}", value) == "7");
        CHECK(std::format("{:iq}", value) == "byte::7");
        CHECK(std::format("{:iu}", value) == "7");
    }
}

TEST_CASE("std::formatter forwards representation-specific format specifications",
          "[integration][enum_formatting][std::formatter]")
{
    enum class [[=beman::enum_formatting::formattable]] byte : std::uint8_t {
        red = 42,
    };

    SECTION("string representation")
    {
        CHECK(std::format("{:s:>10}", byte::red) == std::format("{:>10}", "red"));
        CHECK(std::format("{:s:.2}", byte::red) == std::format("{:.2}", "red"));
    }

    SECTION("integer representation")
    {
        CHECK(std::format("{:i:04}", byte::red) == std::format("{:04}", std::to_underlying(byte::red)));
        CHECK(std::format("{:i:#04x}", byte::red) == std::format("{:#04x}", std::to_underlying(byte::red)));
    }

    SECTION("type representation")
    {
        CHECK(std::format("{:t:>10}", byte::red) == std::format("{:>10}", "byte"));
    }
}

TEST_CASE("std::formatter uses annotation-provided default specifications",
          "[integration][enum_formatting][std::formatter]")
{
    enum class [[=beman::enum_formatting::formattable_with_default("i:#04X")]]
    byte : std::uint8_t {
        red = 42,
    };

    SECTION("annotation default is used when no specification is provided")
    {
        CHECK(std::format("{}", byte::red) == "0X2A");
        CHECK(std::format("{:}", byte::red) == "0X2A");
    }

    SECTION("explicit specification overrides annotation default")
    {
        CHECK(std::format("{:s}", byte::red) == "red");
        CHECK(std::format("{:i:04}", byte::red) == "0042");
    }
}

TEST_CASE("std::formatter uses canonical enumerator names",
          "[integration][enum_formatting][std::formatter]")
{
    enum class [[=beman::enum_formatting::formattable]] color : std::uint32_t {
        red                  = 0xFF0000,
        green                = 0x00FF00,
        blue                 = 0x0000FF,
        gules [[=beman::enum_formatting::canonical]] = 0xFF0000,
        vert                 = 0x00FF00,
        azure [[=beman::enum_formatting::canonical]] = 0x0000FF,
        azul  [[=beman::enum_formatting::canonical]] = 0x0000FF,
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
