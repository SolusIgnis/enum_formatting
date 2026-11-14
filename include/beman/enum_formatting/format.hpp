// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

///@todo Handle UTF8 support.

#ifndef BEMAN_ENUM_FORMATTING_FORMAT_HPP
#define BEMAN_ENUM_FORMATTING_FORMAT_HPP

#ifndef BEMAN_ENUM_FORMATTING_INCLUDED_FROM_INTERFACE_UNIT
    #include "annotations.hpp"
    #include "detail_unexported/concepts.hpp"
    #include "detail_unexported/formatter.hpp"
    #include "detail_unexported/to_string.hpp"

    #include <charconv>
    #include <limits>
    #include <meta>
    #include <string_view>
    #include <utility>

    #include <format>
#endif

template<::beman::enum_formatting::unexported::scoped_enum E, typename CharT>
    requires ::beman::enum_formatting::unexported::annotated_with<E, ::beman::enum_formatting::formattable_with_default>
struct std::formatter<E, CharT> : private ::beman::enum_formatting::unexported::enum_formatter_base,
                                  private std::formatter<std::basic_string_view<CharT>, CharT>,
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

    static constexpr CharT format_terminator = '}';
    static constexpr CharT format_delimiter  = ':';

public:
    constexpr auto parse(auto& ctx) -> decltype(ctx.begin())
    {
        auto specifiers = std::basic_string_view{ctx.begin(), ctx.end()};
#ifdef NEED_PRE_TRIM
        if (specifiers.starts_with(format_delimiter)) {
            specifiers.remove_prefix(1);
            ctx.advance_to(specifiers.begin());
        }
#endif

        while (!specifiers.empty() && state_ != parser_state::done && state_ != parser_state::empty) {
            if (transition_state(process_next_specifier(specifiers)) == specifier_consumption_t::consume) {
                specifiers.remove_prefix(1);
            }
        }

        ctx.advance_to(specifiers.begin());

        if (state_ == parser_state::empty) {
            auto default_specifiers = std::basic_string_view{
                std::meta::extract<::beman::enum_formatting::formattable_with_default>(
                    std::meta::annotations_of_with_type(^^E, ^^::beman::enum_formatting::formattable_with_default).front()
                )
            };
            if (default_specifiers.empty() || default_specifiers.starts_with(format_terminator)) {
                return ctx.begin();
            }
            state_                = parser_state::init;
            auto default_spec_ctx = std::basic_format_parse_context{default_specifiers};
            (void)parse(default_spec_ctx);
            return ctx.begin();
        }

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
            bool qualified_name = qualification_ == qualification::qualified; //Only qualify the name when explicitly requested.
            bool qualified_fallback = qualification_ != qualification::unqualified; //Qualify the fallback by default.

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
    constexpr specifier_consumption_t transition_state(parser_state next_state)
    {
        const auto old_state = std::exchange(state_, next_state);

        using enum parser_state;
        using enum specifier_consumption_t;
        switch (state_) {
            case invalid_qualification:
                [[fallthrough]];
            case extra_specifiers:
                [[fallthrough]];
            case empty:
                [[fallthrough]];
            case done:
                return retain;
            case has_delimiter:
                return (old_state == invalid_representation || old_state == init_delimiter) ? retain : consume;
            default:
                return consume;
        }
    }

    constexpr parser_state process_next_specifier(auto specifiers)
    {
        using enum parser_state;
        switch (state_) {
            case init:
                return handle_state_init(specifiers);
            case init_delimiter:
                return handle_state_init_delimiter(specifiers);
            case has_representation:
                return handle_state_has_representation(specifiers);
            case invalid_representation:
                return handle_state_invalid_representation(specifiers);
            case has_qualification:
                return handle_state_has_qualification(specifiers);
            case invalid_qualification:
                return handle_state_invalid_qualification(specifiers);
            case extra_specifiers:
                return handle_state_extra_specifiers(specifiers);
            case has_delimiter:
                return handle_state_has_delimiter(specifiers);
            case empty:
                return handle_state_empty(specifiers);
            default:
                return done;
        }
    }

    constexpr parser_state handle_state_init(auto specifiers)
    {
        if (specifiers.starts_with(format_terminator)) {
            return parser_state::empty;
        } else if (specifiers.starts_with(format_delimiter)) {
            return parser_state::init_delimiter;
        } else if (process_representation_specifier(specifiers)) {
            return parser_state::has_representation;
        } else {
            return parser_state::invalid_representation;
        }
    }

    constexpr parser_state handle_state_init_delimiter(auto specifiers)
    {
        if (specifiers.starts_with(format_terminator)) {
            return parser_state::empty;
        } else {
            return parser_state::has_delimiter;
        }
    }

    constexpr parser_state handle_state_has_representation(auto specifiers)
    {
        if (specifiers.starts_with(format_terminator)) {
            return parser_state::done;
        } else if (specifiers.starts_with(format_delimiter)) {
            return parser_state::has_delimiter;
        } else if (process_qualification_specifier(specifiers)) {
            return parser_state::has_qualification;
        } else {
            return parser_state::invalid_qualification;
        }
    }

    constexpr parser_state handle_state_has_qualification(auto specifiers)
    {
        if (specifiers.starts_with(format_terminator)) {
            return parser_state::done;
        } else if (specifiers.starts_with(format_delimiter)) {
            return parser_state::has_delimiter;
        } else {
            return parser_state::extra_specifiers;
        }
    }

    constexpr parser_state handle_state_invalid_representation(auto specifiers)
    {
        if (specifiers.substr(0, specifiers.find(format_terminator)).contains(format_delimiter)) {
            throw std::format_error(
                "Invalid representation specifier: expected 's' for enumerator name string, 'i' for underlying integral type, or 't' for enumeration type name."
            );
        } else {
            return parser_state::has_delimiter;
        }
    }

    constexpr parser_state handle_state_invalid_qualification(auto specifiers)
    {
        throw std::format_error("Invalid qualification specifier: expected 'q' for qualified or 'u' for unqualified.");
    }

    constexpr parser_state handle_state_extra_specifiers(auto specifiers)
    {
        throw std::format_error("Expected ':' delimiter or closing '}' after qualification specifier ('q' or 'u').");
    }

    constexpr parser_state handle_state_has_delimiter(auto specifiers) { return parser_state::done; }

    constexpr parser_state handle_state_empty(auto specifiers) { return parser_state::done; }

    constexpr bool process_representation_specifier(auto specifiers)
    {
        if (specifiers.starts_with('s')) {
            representation_ = representation::string;
            return true;
        } else if (specifiers.starts_with('i')) {
            representation_ = representation::integer;
            return true;
        } else if (specifiers.starts_with('t')) {
            representation_ = representation::type;
            return true;
        } else {
            return false;
        }
    }

    constexpr bool process_qualification_specifier(auto specifiers)
    {
        if (representation_ == representation::type) {
            throw std::format_error("Qualification ('q' or 'u') is invalid for type representation ('t').");
        }

        if (specifiers.starts_with('q')) {
            qualification_ = qualification::qualified;
            return true;
        } else if (specifiers.starts_with('u')) {
            qualification_ = qualification::unqualified;
            return true;
        } else {
            return false;
        }
    }
};

#endif // BEMAN_ENUM_FORMATTING_FORMAT_HPP
