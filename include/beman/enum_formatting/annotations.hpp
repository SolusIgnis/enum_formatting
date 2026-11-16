// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef BEMAN_ENUM_FORMATTING_ANNOTATIONS_HPP
#define BEMAN_ENUM_FORMATTING_ANNOTATIONS_HPP

#ifndef BEMAN_ENUM_FORMATTING_INCLUDED_FROM_INTERFACE_UNIT
    #include <cstdint>
    #include <meta>
    #include <string_view>
    #include <type_traits>
#endif

namespace beman::enum_formatting::inline annotations {
    template<typename RepresentationCharT, typename FormatSpecifierCharT = char>
    struct basic_formattable_with_default {
    private:
        template<typename T, std::size_t N>
        using array_reference_t = T (&)[N];

    public:
        const std::add_pointer_t<const FormatSpecifierCharT> please_use_other_door;

        template<std::size_t N>
        consteval explicit basic_formattable_with_default(array_reference_t<const FormatSpecifierCharT, N> format_spec)
            : please_use_other_door{std::define_static_string(format_spec)}
        {}

        consteval basic_formattable_with_default() : basic_formattable_with_default("") {}

        constexpr explicit(false) operator std::basic_string_view<FormatSpecifierCharT>() const
        {
            return please_use_other_door;
        }

        friend constexpr bool operator==(const basic_formattable_with_default& lhs, const basic_formattable_with_default& rhs)
        {
            return std::basic_string_view<FormatSpecifierCharT>{lhs} == std::basic_string_view<FormatSpecifierCharT>{rhs};
        }
    };

    using formattable_with_default   = basic_formattable_with_default<char>;
    using u8formattable_with_default = basic_formattable_with_default<char8_t>;

    inline constexpr formattable_with_default formattable{};
    inline constexpr u8formattable_with_default u8formattable{};

    struct canonical_annotation {};

    inline constexpr canonical_annotation canonical{};
} // namespace beman::enum_formatting::inline annotations

#endif // BEMAN_ENUM_FORMATTING_ANNOTATIONS_HPP
