# `beman.enum_formatting`: Scoped Enumeration Stringification and Annotation-Driven Defaultable Formatting

<!--
SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
-->

<!-- markdownlint-disable line-length -->
[![Library Status](https://raw.githubusercontent.com/bemanproject/beman/refs/heads/main/images/badges/beman_badge-beman_library_under_development.svg)](https://github.com/bemanproject/beman/blob/main/docs/beman_library_maturity_model.md#the-beman-library-maturity-model)
<!-- [![Continuous Integration Tests](https://github.com/bemanproject/enum_formatting/actions/workflows/ci_tests.yml/badge.svg)](https://github.com/bemanproject/enum_formatting/actions/workflows/ci_tests.yml) -->
<!-- [![Lint Check (pre-commit)](https://github.com/bemanproject/enum_formatting/actions/workflows/pre-commit-check.yml/badge.svg)](https://github.com/bemanproject/enum_formatting/actions/workflows/pre-commit-check.yml) -->
<!-- [![Coverage](https://coveralls.io/repos/github/bemanproject/enum_formatting/badge.svg?branch=main)](https://coveralls.io/github/bemanproject/enum_formatting?branch=main) -->
![Standard Target](https://github.com/bemanproject/beman/blob/main/images/badges/cpp29.svg)

<!-- markdownlint-restore -->

Powered by C++26 static reflection and attribute annotations, `beman.enum_formatting` provides automatic `to_string` support, seamless `std::format` integration, and type-level default formatting specifications to scoped enumerations.

**Implements**: `std::to_string` overloads, `std::formatter` specializations, `std::formattable`, `std::formattable_with_default`, and `std::is_enumerated_value` proposed in [Paper # Unassigned: Initial Draft in Progress (PXXXXR0)](https://wg21.link/PXXXXR0).

**Status**: [Under development and not yet ready for production use.](https://github.com/bemanproject/beman/blob/main/docs/beman_library_maturity_model.md#under-development-and-not-yet-ready-for-production-use)

## License

`beman.enum_formatting` is licensed under the Apache License v2.0 with LLVM Exceptions.

## Usage

Full runnable examples can be found in [`examples/`](examples/).

### Direct Stringification

Overloads for `to_string` enable direct production of `std::string`s from scoped enumeration values with a fallback for values without a matching enumerator.

```cpp
enum class color : std::uint8_t {
    red,
    orange,
    yellow,
    green,
    blue,
    purple,
};

color my_color  = color::blue;
auto color_name = to_string(my_color); //color_name == "blue"s

color no_match     = static_cast<color>(42);
auto unnamed_color = to_string(no_match); //unnamed_color == "color::42"s
```

### Annotated Declarations

By applying the `[[=beman::enum_formatting::formattable]]` annotation on the declaration, a scoped enumeration can opt into `std::format` participation.

```cpp
using namespace beman::enum_formatting::annotations;

enum class [[=formattable]] ink_color : std::uint8_t {
    cyan = 0,
    magenta,
    yellow,
    black,
};
```

With the `[[=beman::enum_formatting::formattable_with_default(...)]]` annotation, a default format specifier can be attached to the type.

```cpp
using namespace beman::enum_formatting::annotations;

enum class [[=formattable_with_default("i,#010b")]] byte_t : std::uint8_t {};
```

### Simple Formatting Integration

With the above declarations, the full `std::format` ecosystem becomes available.

```cpp 
std::println("{}", ink_color::cyan); //prints "cyan"

std::println("{}", static_cast<byte_t>(8)); //prints "0b00001000"
```

However, `std::format("{}", color::green)` fails to compile because the `color` enumeration is not annotated as formattable.

### Format Specifiers

The `std::formatter` specializations divide the format specification between general specifiers and a representation-specific format specification, separated by a colon (`:`). The general specifiers select string (`s`) vs integer (`i`) vs type (`t`) formatting representation with the first character. For the string and integer representations, an optional second character specifies whether the representation is qualified with the name of the enumeration type (`q`) or unqualified (`u`). These qualification specifiers do not apply to the type representation as they would respectively be redundant (e.g., `ink_color::ink_color`) or self-contradictory. The representation-specific format specification is forwarded to the string or integer formatter as selected previously. (The type representation uses the string formatter.)

```cpp 
auto unqualified_cyan_name  = std::format("{:su}", ink_color::cyan); //"cyan"
auto qualified_cyan_name    = std::format("{:sq}", ink_color::cyan); //"ink_color::cyan"
auto unqualified_cyan_value = std::format("{:iu}", ink_color::cyan); //"0"
auto qualified_cyan_value   = std::format("{:iq}", ink_color::cyan); //"ink_color::0"

byte_t byte = static_cast<byte_t>(42);
auto unqualified_byte_name  = std::format("{:su}", byte); //"42"
auto qualified_byte_name    = std::format("{:sq}", byte); //"byte_t::42"
auto unqualified_byte_value = std::format("{:iu}", byte); //"42"
auto qualified_byte_value   = std::format("{:iq}", byte); //"byte_t::42"
```

By default, integer representations and enumerator names are unqualified while the string representation of a value that does not match an enumerator is a qualified integer. The general default representation is string, but a default specification annotated on the type takes precedence.

```cpp
auto default_cyan_name      = std::format("{:s}",  ink_color::cyan); //"cyan"
auto default_cyan_value     = std::format("{:i}",  ink_color::cyan); //"0"
auto default_cyan_format    = std::format("{}",    ink_color::cyan); //"cyan"

auto default_byte_name      = std::format("{:s}",  byte); //"byte_t::42"
auto default_byte_value     = std::format("{:i}",  byte); //"42"
auto default_byte_format    = std::format("{}",    byte); //"0b00101010" (from the type's annotation)
```

The string format specification applies to the whole qualified name (i.e., the string representation), but the integer format specification only applies to the value in the underlying type (i.e., the integer representation) before the qualification is prepended.

```cpp 
auto specified_byte_name  = std::format("{:s:>15}", byte); //"     byte_t::42"
auto specified_byte_value = std::format("{:iq:4}",  byte); //"byte_t::  42"
auto padded_byte_value    = std::format("{:iq:04}", byte); //"byte_t::0042"

auto explicit_byte_value = std::format("{:>9}::{:i:#04x}", std::meta::identifier_of(^^decltype(byte)), byte); //"   byte_t::0x2a"

auto nested_byte_value = std::format("{:>15}", std::format("{:iq:#04x}", byte)); //"   byte_t::0x2a"

auto clear_byte_value = std::format("{0:t:>9}::{0:i:#04x}", byte); //"   byte_t::0x2a"
```

When the general specifiers section is omitted, the general default representation (string) is selected, and the representation-specific format specification is passed to the string formatter. When the representation-specific format specification is empty, the selected formatter applies its own defaults. When the overall format specification is empty, the specification annotated on the type provides the default. Any nonempty format specification at the call site completely replaces the type-level default.

```cpp 
auto abbr_cyan1 = std::format("{:.1}",   ink_color::cyan); //"c"
auto abbr_cyan2 = std::format("{0:.1}",  ink_color::cyan); //"c"
auto abbr_cyan3 = std::format("{::.1}",  ink_color::cyan); //"c"
auto abbr_cyan4 = std::format("{0::.1}", ink_color::cyan); //"c"
auto abbr_cyan5 = std::format("{:s:.1}", ink_color::cyan); //"c"

auto default_byte1 = std::format("{}",    byte); //"0b00101010"
auto default_byte2 = std::format("{:}",   byte); //"0b00101010"
auto default_byte3 = std::format("{::}",  byte); //"0b00101010"
auto stringy_byte1 = std::format("{:s}",  byte); //"byte_t::42"
auto stringy_byte2 = std::format("{:s:}", byte); //"byte_t::42"
auto stringy_byte3 = std::format("{::s}", byte); //"byte_t::42"
auto integer_byte1 = std::format("{:i}",  byte); //"42"
auto integer_byte2 = std::format("{:i:}", byte); //"42"
```

Multiple representations of the same value can be combined in a single format string.

```cpp
auto verbose_cyan = std::format("{0:sq} ({0:i:#04x})", ink_color::cyan); //"ink_color::cyan (0x00)"
```

## Dependencies

### Build Environment

This project requires at least the following to build:

* A C++ compiler that conforms to the C++26 standard or greater
* CMake 3.30 or later
* (Test Only) Catch2

You can disable building tests by setting CMake option `BEMAN_ENUM_FORMATTING_BUILD_TESTS` to
`OFF` when configuring the project.

You can disable building examples by setting CMake option `BEMAN_ENUM_FORMATTING_BUILD_EXAMPLES` to
`OFF` when configuring the project.

### Supported Platforms

| Compiler   | Version | C++ Standards | Standard Library  |
|------------|---------|---------------|-------------------|
| GCC        | 16      | C++26         | libstdc++         |
| Clang      | TBA     | C++26         | libstdc++, libc++ |
| AppleClang | TBA     | C++26         | libc++            |
| MSVC       | TBA     | C++26         | MSVC STL          |

## Development

See the [Contributing Guidelines](CONTRIBUTING.md).

## Integrate `beman.enum_formatting` into your project

### Build

You can build `beman.enum_formatting` using a CMake workflow preset:

```bash
cmake --workflow --preset gcc-release
```

To list available workflow presets, you can invoke:

```bash
cmake --list-presets=workflow
```

For details on building `beman.enum_formatting` without using a CMake preset, refer to the
[Contributing Guidelines](CONTRIBUTING.md).

### Installation

#### Vcpkg

The preferred way to install `beman.enum_formatting` is via vcpkg. To do so, after installing vcpkg
itself, you need to add support for the Beman project's [vcpkg
registry](https://github.com/bemanproject/vcpkg-registry) by configuring a
`vcpkg-configuration.json` file (which `beman.enum_formatting` [provides](vcpkg-configuration.json)).

Then, simply run `vcpkg install beman-enum-formatting`.

#### Manual

To install `beman.enum_formatting` globally after building with the `gcc-release` preset, you can
run:

```bash
sudo cmake --install build/gcc-release
```

Alternatively, to install to a prefix, for example `/opt/beman`, you can run:

```bash
sudo cmake --install build/gcc-release --prefix /opt/beman
```

This will generate the following directory structure:

```txt
/opt/beman
├── include
│   └── beman
│       └── enum_formatting
│           ├── enum_formatting.hpp
│           └── ...
└── lib
    └── cmake
        └── beman.enum_formatting
            ├── beman.enum_formatting-config-version.cmake
            ├── beman.enum_formatting-config.cmake
            └── beman.enum_formatting-targets.cmake
```

### CMake Configuration

If you installed `beman.enum_formatting` to a prefix, you can specify that prefix to your CMake
project using `CMAKE_PREFIX_PATH`; for example, `-DCMAKE_PREFIX_PATH=/opt/beman`.

You need to bring in the `beman.enum_formatting` package to define the `beman::enum_formatting` CMake
target:

```cmake
find_package(beman.enum_formatting REQUIRED)
```

You will then need to add `beman::enum_formatting` to the link libraries of any libraries or
executables that include `beman.enum_formatting` headers.

```cmake
target_link_libraries(yourlib PUBLIC beman::enum_formatting)
```

### Using `beman.enum_formatting`

To use `beman.enum_formatting` in your C++ project,
include an appropriate `beman.enum_formatting` header from your source code.

```c++
#include <beman/enum_formatting/enum_formatting.hpp>
```
