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
    cyan,
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

However, `std::format("{}", color::green) });` fails to compile because `color` didn't declare itself formattable.

### Format Specifiers

TODO

Full runnable examples can be found in [`examples/`](examples/).

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

You can build enum_formatting using a CMake workflow preset:

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

The preferred way to install enum_formatting is via vcpkg. To do so, after installing vcpkg
itself, you need to add support for the Beman project's [vcpkg
registry](https://github.com/bemanproject/vcpkg-registry) by configuring a
`vcpkg-configuration.json` file (which enum_formatting [provides](vcpkg-configuration.json)).

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
