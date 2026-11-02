// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

///@todo Consider `basic_formattable_with_default` template and aliases for `char` and `char8_t` or at least `formattable` and `u8formattable`

#ifndef BEMAN_ENUM_FORMATTING_ANNOTATIONS_HPP
#define BEMAN_ENUM_FORMATTING_ANNOTATIONS_HPP

#ifndef BEMAN_ENUM_FORMATTING_INCLUDED_FROM_INTERFACE_UNIT
#include <cstdint>
#include <meta>
#include <string_view>
#include <type_traits>
#endif

namespace beman::enum_formatting::inline annotations {
    struct formattable_with_default {
    private:
        template<typename T, std::size_t N>
        using array_reference_t = T(&)[N];

    public:
        const std::add_pointer_t<const char> please_use_other_door;

        template<std::size_t N>
        consteval explicit formattable_with_default(array_reference_t<const char, N> format_spec) : please_use_other_door{std::define_static_string(format_spec)} {}

        consteval formattable_with_default() : formattable_with_default("") {}

        constexpr explicit(false) operator std::string_view() const { return please_use_other_door; }
        
        friend constexpr bool operator==(
            const formattable_with_default& lhs,
            const formattable_with_default& rhs
        ) {
            return std::string_view{lhs.please_use_other_door}
                == std::string_view{rhs.please_use_other_door};
        }
    };

    inline constexpr formattable_with_default formattable{};

    struct canonical_annotation {};

    inline constexpr canonical_annotation canonical{};
} // namespace beman::enum_formatting::inline annotations

#endif // BEMAN_ENUM_FORMATTING_ANNOTATIONS_HPP
