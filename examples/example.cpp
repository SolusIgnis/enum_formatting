// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <beman/enum_formatting/config.hpp>
#include <beman/enum_formatting/enum_formatting.hpp>

#include <meta>
#include <print>

using namespace beman::enum_formatting::annotations;

using beman::enum_formatting::to_string;

enum class color : std::uint8_t {
    red,
    orange,
    yellow,
    green,
    blue,
    purple,
};

enum class [[=formattable]] ink_color : std::uint8_t {
    cyan = 0,
    magenta,
    yellow,
    black,
};

using cmyk_color = ink_color;

enum class [[=formattable_with_default("i:#010b")]] byte_t : std::uint8_t {};

int main()
{
    auto byte = static_cast<byte_t>(42);

    std::println("to_string for enumerators and unnamed values of enumerations");
    std::print("\t{}", to_string(color::red));
    std::print(", {}", to_string(color::blue));
    std::print(", {}", to_string(color::yellow));
    std::println();
    std::print("\t{}", to_string(static_cast<color>(42)));
    std::println();
    std::print("\t{}", to_string(static_cast<cmyk_color>(42)));
    std::print(", {}", to_string(static_cast<ink_color>(45)));
    std::println();
    std::println("std::format for scoped enumerations");
    std::println("\t{}", ink_color::cyan);
    std::println("\t{}", static_cast<byte_t>(8));
    std::print("\t{:su}", ink_color::cyan); //"cyan"
    std::print(", {:sq}", ink_color::cyan); //"ink_color::cyan"
    std::print(", {:iu}", ink_color::cyan); //"0"
    std::print(", {:iq}", ink_color::cyan); //"ink_color::0"
    std::println();
    std::print("\t{:su}", byte); //"42"
    std::print(", {:sq}", byte); //"byte_t::42"
    std::print(", {:iu}", byte); //"42"
    std::print(", {:iq}", byte); //"byte_t::42"
    std::println();
    std::print("\t{:s}", ink_color::cyan); //"cyan"
    std::print(", {:i}", ink_color::cyan); //"0"
    std::print(", {}", ink_color::cyan); //"cyan"
    std::println();
    std::print("\t{:s}", byte); //"byte_t::42"
    std::print(", {:i}", byte); //"42"
    std::print(", {}", byte); //"0b00101010"
    std::println();
    std::print("\t|{:s:>15}|", byte); //"|     byte_t::42|"
    std::print(", {:iq:4}", byte); //"byte_t:  42"
    std::print(", {:iq:04}", byte); //"byte_t:0042"
    std::println();
    std::print("\t|{:>9}::{:i:#04x}|", std::meta::identifier_of(^^decltype(byte)), byte); //"|   byte_t::0x2a|"
    std::print(", |{:>15}|", std::format("{:iq:#04x}", byte)); //"|   byte_t::0x2a|"
    std::print(", |{0:t:>9}::{0:i:#04x}|", byte); //"|   byte_t::0x2a|"
    std::println();
    std::print("\t{:.1}",   ink_color::cyan); //c
    std::print(", {0:.1}",  ink_color::cyan); //c
    std::print(", {::.1}",  ink_color::cyan); //c
    std::print(", {0::.1}", ink_color::cyan); //c
    std::print(", {:s:.1}", ink_color::cyan); //c
    std::println();
    std::print("\t{}",   byte); //"0b00101010"
    std::print(", {:}",  byte); //"0b00101010"
    std::print(", {::}", byte); //"0b00101010"
    std::println();
    std::print("\t{:s}",  byte); //"byte_t::42"
    std::print(", {:s:}", byte); //"byte_t::42"
    std::print(", {::s}", byte); //"byte_t::42"
    std::println();
    std::print("\t{:i}",  byte); //"42"
    std::print(", {:i:}", byte); //"42"
    std::println();
    std::print("\t{0:sq} ({0:i:#04x})", ink_color::cyan); //"ink_color::cyan (0x00)"
    std::println();

    return 0;
}
