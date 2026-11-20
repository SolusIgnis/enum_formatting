// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

///@todo Handle UTF8 support.

#ifndef BEMAN_ENUM_FORMATTING_FORMAT_HPP
#define BEMAN_ENUM_FORMATTING_FORMAT_HPP

#ifndef BEMAN_ENUM_FORMATTING_INCLUDED_FROM_INTERFACE_UNIT
    #include "annotations.hpp"
    #include "detail_unexported/concepts.hpp"
    #include "detail_unexported/formatter.hpp"
    #include "detail_unexported/to_string.hpp"

    #include <array>
    #include <charconv>
    #include <cstdint>
    #include <limits>
    #include <meta>
    #include <string>
    #include <string_view>
    #include <utility>

    #include <format>
#endif

template<::beman::enum_formatting::unexported::scoped_enum E, typename CharT>
    requires ::beman::enum_formatting::unexported::annotated_as_formattable<E>
          && (std::same_as<CharT, char>
#ifdef BEMAN_ENUM_FORMATTING_FEATURE_SUPPORT_UTF8
          ||  std::same_as<CharT, char8_t>
#endif
          )
struct std::formatter<E, CharT>
    : private std::formatter<std::conditional_t<
        !std::meta::annotations_of_with_type(^^E, ^^::beman::enum_formatting::formattable_with_default<CharT>).empty(),
        std::string_view,
#ifdef BEMAN_ENUM_FORMATTING_FEATURE_SUPPORT_UTF8
        std::u8string_view
#else
        std::string_view
#endif
    >, CharT>
    , private std::formatter<std::underlying_type_t<E>, CharT> {
private:
    static consteval auto narrow_format_annotations_of(std::meta::info enumeration)
    {
        return std::meta::annotations_of_with_type(enumeration, ^^::beman::enum_formatting::formattable_with_default<CharT>);
    }

    static consteval bool has_narrow_format_annotations(std::meta::info enumeration)
    {
        return !narrow_format_annotations_of(enumeration).empty();
    }

    static consteval auto narrow_annotation_count(std::meta::info enumeration)
    {
        return narrow_format_annotations_of(enumeration).size();
    }

    static consteval auto utf8_format_annotations_of(std::meta::info enumeration)
    {
        return std::meta::annotations_of_with_type(enumeration, ^^::beman::enum_formatting::u8formattable_with_default<CharT>);
    }

    static consteval bool has_utf8_format_annotations(std::meta::info enumeration)
    {
        return !utf8_format_annotations_of(enumeration).empty();
    }

    static consteval auto utf8_annotation_count(std::meta::info enumeration)
    {
        return utf8_format_annotations_of(enumeration).size();
    }

    consteval
    {
        using namespace std::literals;

        const auto count_to_string = [](auto count) consteval {
            std::array<char, std::numeric_limits<decltype(count)>::digits10 + 2> buffer{};
            auto [ptr, _] = std::to_chars(buffer.begin(), buffer.end(), count);

            return std::string{buffer.begin(), ptr};
        };

        static_assert(
            !(has_narrow_format_annotations(^^E) && has_utf8_format_annotations(^^E)),
            "'"s + std::meta::identifier_of(^^E) + "' is annotated as both narrow-character formattable and utf8 formattable."sv
        );

        static_assert(
            narrow_annotation_count(^^E) <= 1,
            "Only one 'formattable' or 'formattable_with_default' annotation with the same format specifier character type may appertain to a single scoped enumeration type. '"s
                + std::meta::identifier_of(^^E) + "' has "sv + count_to_string(narrow_annotation_count(^^E)) + "."sv
        );

        static_assert(
            utf8_annotation_count(^^E) <= 1,
            "Only one 'u8formattable' or 'u8formattable_with_default' annotation with the same format specifier character type may appertain to a single scoped enumeration type. '"s
                + std::meta::identifier_of(^^E) + "' has "sv + count_to_string(utf8_annotation_count(^^E)) + "."sv
        );

#ifndef BEMAN_ENUM_FORMATTING_FEATURE_SUPPORT_UTF8
        static_assert(!has_utf8_format_annotations(^^E), "std::format does not currently support formatting utf8 strings.");
#endif
    }

    using format_specification = ::beman::enum_formatting::unexported::format_specification<CharT>;

    struct format_defaults_t {
        std::basic_format_parse_context<CharT> ctx;
        format_specification spec;

        constexpr format_defaults_t(std::basic_string_view<CharT> specifiers) : ctx{specifiers}, spec{specifiers}
        {
            ctx.advance_to(spec.begin());
        }
    };

    static consteval format_defaults_t get_validated_defaults()
    {
        try {
            if constexpr (has_narrow_format_annotations(^^E)) {
                return {std::meta::extract<::beman::enum_formatting::formattable_with_default<CharT>>(
                    narrow_format_annotations_of(^^E).front()
                )};
            } else {
                return {std::meta::extract<::beman::enum_formatting::u8formattable_with_default<CharT>>(
                    utf8_format_annotations_of(^^E).front()
                )};
            }
        } catch (const std::format_error& error) {
            throw std::format_error(
                "Invalid default format specification annotated onto '" + std::string{std::meta::identifier_of(^^E)}
                + "': " + error.what()
            );
        }
    }

    static constexpr auto make_qualification_format_prefix()
        requires std::same_as<CharT, char>
    {
        return std::string_view{"{}::"};
    }
#ifdef BEMAN_ENUM_FORMATTING_FEATURE_SUPPORT_UTF8
    static constexpr auto make_qualification_format_prefix()
        requires std::same_as<CharT, char8_t>
    {
        return std::u8string_view{u8"{}::"};
    }
#endif

    enum class representation : std::uint8_t {
        string,
        integer,
        type,
    } representation_ = representation::string;

    enum class qualification : std::uint8_t {
        use_default,
        qualified,
        unqualified,
    } qualification_ = qualification::use_default;

public:
    constexpr auto parse(std::basic_format_parse_context<CharT>& caller_ctx)
    {
        auto [default_spec_ctx, default_spec] = get_validated_defaults();

        const auto caller_spec = format_specification{caller_ctx};

        caller_ctx.advance_to(caller_spec.begin());
        default_spec_ctx.advance_to(default_spec.begin());

        auto& active_spec = caller_spec.is_empty() ? default_spec : caller_spec;

        apply_representation(active_spec);
        apply_qualification(active_spec);

        auto& active_ctx = caller_spec.is_empty() ? default_spec_ctx : caller_ctx;

        if (representation_ == representation::string) {
            if constexpr (!std::meta::is_enumerable_type(^^E)) {
                throw std::format_error("String representation is not available for unenumerable types.");
            } else {
                active_ctx.advance_to(std::formatter<std::basic_string_view<CharT>, CharT>::parse(active_ctx));
            }
        } else if (representation_ == representation::integer) {
            active_ctx.advance_to(std::formatter<std::underlying_type_t<E>, CharT>::parse(active_ctx));
        } else if (representation_ == representation::type) {
            active_ctx.advance_to(std::formatter<std::basic_string_view<CharT>, CharT>::parse(active_ctx));
        } else {
            std::unreachable();
        }

        return caller_ctx.begin();
    }

    constexpr auto format(E value, auto& ctx) const
    {
        if (representation_ == representation::string) {
            bool qualified_name = qualification_ == qualification::qualified; //Only qualify the name when explicitly requested.
            bool qualified_fallback = qualification_ != qualification::unqualified; //Qualify the fallback by default.

            if constexpr (has_narrow_format_annotations(^^E)) {
                return std::formatter<std::string_view, CharT>::format(
                    ::beman::enum_formatting::unexported::to_string<char>(value, qualified_name, qualified_fallback), ctx
                );
            } else {
                return std::formatter<std::u8string_view, CharT>::format(
                    ::beman::enum_formatting::unexported::to_string<char8_t>(value, qualified_name, qualified_fallback), ctx
                );
            }
        } else if (representation_ == representation::integer) {
            if (qualification_ == qualification::qualified) {
                auto out = ctx.out();
                if constexpr (has_narrow_format_annotations(^^E)) {
                    out = std::format_to(out, std::basic_string_view<CharT>{"{}::"}, std::meta::identifier_of(^^E));
                } else {
                    out = std::format_to(out, std::basic_string_view<CharT>{"{}::"}, std::meta::u8identifier_of(^^E));
                }
                ctx.advance_to(out);
            }
            return std::formatter<std::underlying_type_t<E>, CharT>::format(std::to_underlying(value), ctx);
        } else if (representation_ == representation::type) {
            if constexpr (has_narrow_format_annotations(^^E)) {
                return std::formatter<std::string_view, CharT>::format(std::meta::identifier_of(^^E), ctx);
            } else {
                return std::formatter<std::u8string_view, CharT>::format(std::meta::u8identifier_of(^^E), ctx);
            }
        } else {
            std::unreachable();
        }
    }

private:
    constexpr void apply_representation(const format_specification& specification) noexcept
    {
        switch (specification.get_representation()) {
            case format_specification::representation::unspecified:
                //Keep the current value.
                break;
            case format_specification::representation::string:
                representation_ = representation::string;
                break;
            case format_specification::representation::integer:
                representation_ = representation::integer;
                break;
            case format_specification::representation::type:
                representation_ = representation::type;
                break;
        }
    }

    constexpr void apply_qualification(const format_specification& specification) noexcept
    {
        switch (specification.get_qualification()) {
            case format_specification::qualification::unspecified:
                //Keep the current value.
                break;
            case format_specification::qualification::qualified:
                qualification_ = qualification::qualified;
                break;
            case format_specification::qualification::unqualified:
                qualification_ = qualification::unqualified;
                break;
        }
    }
};

#endif // BEMAN_ENUM_FORMATTING_FORMAT_HPP
