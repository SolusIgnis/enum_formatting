# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

find_package(Catch2 3 REQUIRED)

add_executable(beman.enum_formatting.tests.annotations)
add_executable(beman.enum_formatting.tests.format)
add_executable(beman.enum_formatting.tests.string)
add_executable(beman.enum_formatting.tests.utility)

target_sources(
    beman.enum_formatting.tests.annotations
    PRIVATE enum_formatting-annotations.test.catch2.cpp
)
target_sources(
    beman.enum_formatting.tests.format
    PRIVATE enum_formatting-format.test.catch2.cpp
)
target_sources(
    beman.enum_formatting.tests.string
    PRIVATE enum_formatting-string.test.catch2.cpp
)
target_sources(
    beman.enum_formatting.tests.utility
    PRIVATE enum_formatting-utility.test.catch2.cpp
)

target_link_libraries(
    beman.enum_formatting.tests.annotations
    PRIVATE beman::enum_formatting Catch2::Catch2WithMain
)
target_link_libraries(
    beman.enum_formatting.tests.format
    PRIVATE beman::enum_formatting Catch2::Catch2WithMain
)
target_link_libraries(
    beman.enum_formatting.tests.string
    PRIVATE beman::enum_formatting Catch2::Catch2WithMain
)
target_link_libraries(
    beman.enum_formatting.tests.utility
    PRIVATE beman::enum_formatting Catch2::Catch2WithMain
)

if(BEMAN_EXEMPLAR_USE_MODULES)
    set_target_properties(
        beman.enum_formatting.tests.annotations
        beman.enum_formatting.tests.format
        beman.enum_formatting.tests.string
        beman.enum_formatting.tests.utility
        PROPERTIES CXX_MODULE_STD ON
    )
endif()

include(Catch)
catch_discover_tests(beman.enum_formatting.tests.annotations)
catch_discover_tests(beman.enum_formatting.tests.format)
catch_discover_tests(beman.enum_formatting.tests.string)
catch_discover_tests(beman.enum_formatting.tests.utility)
