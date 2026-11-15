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
    requires ::beman::enum_formatting::unexported::annotated_with<E, ::beman::enum_formatting::formattable_with_default>
          && (std::same_as<CharT, char>
        //|| std::same_as<CharT, char8_t>
             )
struct std::formatter<E, CharT> : private std::formatter<std::basic_string_view<CharT>, CharT>,
                                  private std::formatter<std::underlying_type_t<E>, CharT> {
private:
    consteval
    {
        using namespace std::literals;
        const auto annotation_count = [](std::meta::info enumeration) consteval {
            return std::meta::annotations_of_with_type(enumeration, ^^::beman::enum_formatting::formattable_with_default)
                .size();
        };

        const auto count_to_string = [](auto count) consteval {
            std::array<char, std::numeric_limits<decltype(count)>::digits10 + 2> buffer{};
            auto [ptr, _] = std::to_chars(buffer.begin(), buffer.end(), count);

            return std::string{buffer.data(), ptr};
        };

        static_assert(
            annotation_count(^^E) == 1,
            "Only one 'formattable' or 'formattable_with_default' annotation may appertain to a single scoped enumeration type. '"s
                + std::meta::identifier_of(^^E) + "' has "sv + count_to_string(annotation_count(^^E)) + "."sv
        );
    }
    
    static consteval auto generate_default_specification_context_from_annotation()
    {
        auto default_specifiers = std::basic_string_view<CharT>{
            std::meta::extract<::beman::enum_formatting::formattable_with_default>(
                std::meta::annotations_of_with_type(^^E, ^^::beman::enum_formatting::formattable_with_default).front()
            )
        };
        return std::basic_format_parse_context<CharT>{default_specifiers};
    }

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

    using format_specification = ::beman::enum_formatting::unexported::format_specification<CharT>;

public:
    constexpr auto parse(std::basic_format_parse_context<CharT>& ctx) -> decltype(ctx.begin())
    {
        auto default_spec_ctx = generate_default_specification_context_from_annotation();

        const auto specification         = format_specification{ctx};
        const auto default_specification = format_specification{default_spec_ctx};

        ctx.advance_to(specification.begin());

        if (specification.is_empty()) {
            if (default_specification.is_empty()) {
                return ctx.begin();
            }

            (void)parse(default_spec_ctx);
            return ctx.begin();
        }

        apply_representation(specification);
        apply_qualification(specification);

        if (representation_ == representation::string) {
            if constexpr (!std::meta::is_enumerable_type(^^E)) {
                throw std::format_error("String representation is not available for unenumerable types.");
            } else {
                return std::formatter<std::basic_string_view<CharT>, CharT>::parse(ctx);
            }
        } else if (representation_ == representation::integer) {
            return std::formatter<std::underlying_type_t<E>, CharT>::parse(ctx);
        } else if (representation_ == representation::type) {
            return std::formatter<std::basic_string_view<CharT>, CharT>::parse(ctx);
        } else {
            std::unreachable();
        }
    }

    constexpr auto format(E value, auto& ctx) const
    {
        if (representation_ == representation::string) {
            const bool qualified_name     = qualification_
                                         == qualification::qualified; //Only qualify the name when explicitly requested.
            const bool qualified_fallback = qualification_ != qualification::unqualified; //Qualify the fallback by default.

            return std::formatter<std::basic_string_view<CharT>, CharT>::format(
                ::beman::enum_formatting::unexported::to_string(value, qualified_name, qualified_fallback), ctx
            );
        } else if (representation_ == representation::integer) {
            if (qualification_ == qualification::qualified) {
                auto out = ctx.out();
                out      = std::format_to(out, "{}::", std::meta::identifier_of(^^E));
                ctx.advance_to(out);
            }
            return std::formatter<std::underlying_type_t<E>, CharT>::format(std::to_underlying(value), ctx);
        } else if (representation_ == representation::type) {
            return std::formatter<std::basic_string_view<CharT>, CharT>::format(std::meta::identifier_of(^^E), ctx);
        } else {
            std::unreachable();
        }
    }

private:
    constexpr void apply_representation(const format_specification& specification) noexcept
    {
        switch (specification.get_representation()) {
            case format_specification::representation::unspecified:
                representation_ = representation::string;
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
                qualification_ = qualification::use_default;
                break;
            case format_specification::qualification::qualified:
                qualification_ = qualification::qualified;
                break;
            case format_specification::qualification::unqualified:
                qualification_ = qualification::unqualified;
                break;
        }
    }
    
    consteval
    {
        try {
            [[maybe_unused]] auto _ = format_specification{generate_default_specification_context_from_annotation()};
        } catch (...) {
            throw;
        }
    }
};

#endif // BEMAN_ENUM_FORMATTING_FORMAT_HPP
