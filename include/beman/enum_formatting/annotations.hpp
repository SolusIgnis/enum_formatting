// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef BEMAN_ENUM_FORMATTING_ANNOTATIONS_HPP
#define BEMAN_ENUM_FORMATTING_ANNOTATIONS_HPP

#include <cstdint>
#include <meta>

namespace beman::enum_formatting::inline annotations {
    struct formattable_with_default {
    private:
        template<typename T, std::size_t N>
        using array_reference_t = T(&)[N];

    public:
        const char* default_format_specification;

        template<std::size_t N>
        consteval formattable_with_default(array_reference_t<const char, N> format_spec) : default_format_specification{std::define_static_string(format_spec)} {}

        consteval formattable_with_default() : formattable_with_default("") {}
    };

    inline constexpr formattable_with_default formattable{};
} // namespace beman::enum_formatting::inline annotations

#endif // BEMAN_ENUM_FORMATTING_ANNOTATIONS_HPP
