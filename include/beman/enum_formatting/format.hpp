// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef BEMAN_ENUM_FORMATTING_FORMAT_HPP
#define BEMAN_ENUM_FORMATTING_FORMAT_HPP

#ifndef BEMAN_ENUM_FORMATTING_INCLUDED_FROM_INTERFACE_UNIT
#include "annotations.hpp"
#include "unexported/to_string.hpp"

#include <meta>
#include <string_view>
#include <utility>

#include <format>
#endif

template<typename E, typename CharT>
    requires (std::meta::is_scoped_enum_type(^^E))
        && (!std::meta::annotations_of_with_type(^^E, ^^::beman::enum_formatting::formattable_with_default).empty())
struct std::formatter<E, CharT>
    : private std::formatter<std::basic_string_view<CharT>, CharT>
    , private std::formatter<std::underlying_type_t<E>, CharT> {
private:
    static_assert(std::meta::annotations_of_with_type(^^E, ^^::beman::enum_formatting::formattable_with_default).size() == 1, "Only one 'formattable' or 'formattable_with_default' annotation can be on a single type.");

    static constexpr char format_terminator = '}';
    static constexpr char format_delimiter = ':';

    enum class representation : std::uint8_t {
        string,
        integer,
        type,
    } representation_ = representation::string;

    enum class qualification : std::uint8_t {
        use_default,
        qualified,
        unqualified,
    } qualified_ = qualification::use_default;

public:
    constexpr auto parse(auto& ctx) -> decltype(ctx.begin())
    {
        auto specifiers = std::basic_string_view{ctx.begin(), ctx.end()};

        if (specifiers.starts_with(format_delimiter)) {
            specifiers.remove_prefix(1);
            ctx.advance_to(specifiers.begin());
        }

        if (specifiers.empty() || specifiers.starts_with(format_terminator)) {
            auto default_specifiers = std::basic_string_view{std::meta::extract<::beman::enum_formatting::formattable_with_default>(std::meta::annotations_of_with_type(^^E, ^^::beman::enum_formatting::formattable_with_default).front()).default_format_specification};

            if (default_specifiers.empty() || default_specifiers.starts_with(format_terminator)) {
                return ctx.begin();
            }

            auto default_spec_ctx = std::basic_format_parse_context{default_specifiers};
            (void)parse(default_spec_ctx);
            return ctx.begin();
        }

        if (!specifiers.starts_with(format_delimiter)) {
            if (parse_selector(specifiers)) {
                specifiers.remove_prefix(1);

                if (parse_qualification(specifiers)) {
                    if (representation_ == representation::type) {
                        throw std::format_error("Qualification ('q' or 'u') is invalid for type representation ('t').");
                    }

                    specifiers.remove_prefix(1);
                }
            } else {
                if (specifiers.contains(format_delimiter)) {
                    throw std::format_error("Invalid representation selector: expected 's' for enumerator name string, 'i' for underlying integral type, or 't' for enumeration type name.");
                }
            }
        }

        if (specifiers.starts_with(format_delimiter)) {
            specifiers.remove_prefix(1);
        }

        ctx.advance_to(specifiers.begin());

        if (representation_ == representation::string) {
            return std::formatter<std::basic_string_view<CharT>, CharT>::parse(ctx);
        } else if (representation_ == representation::integer) {
            return std::formatter<std::underlying_type_t<E>, CharT>::parse(ctx);
        } else if (representation_ == representation::type) {
            return std::formatter<std::basic_string_view<CharT>, CharT>::parse(ctx);
        } else {
            std::unreachable();
        }
    }

    constexpr auto format(E value, auto& ctx) const {
        if (representation_ == representation::string) {
            bool qualified_name = (qualified_ == qualification::qualified);       //Only qualify the name when explicitly requested.
            bool qualified_fallback = (qualified_ != qualification::unqualified); //Qualify the fallback by default.

            return std::formatter<std::basic_string_view<CharT>, CharT>::format(::beman::enum_formatting::unexported::to_string(value, qualified_name, qualified_fallback), ctx);
        } else if (representation_ == representation::integer) {
            if (qualified_ == qualification::qualified) {
                auto out = ctx.out();
                out = std::format_to(out, "{}::", std::meta::identifier_of(^^E));
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
    constexpr bool parse_selector(auto specifiers)
    {
        if (specifiers.empty() || specifiers.starts_with(format_terminator)) {
            return false;
        }

        bool has_selector = true;

        if (specifiers.starts_with('s')) {
            representation_ = representation::string;
        } else if (specifiers.starts_with('i')) {
            representation_ = representation::integer;
        } else if (specifiers.starts_with('t')) {
            representation_ = representation::type;
        } else {
            has_selector = false;
        }

        return has_selector;
    }

    constexpr bool parse_qualification(auto specifiers)
    {
        if (specifiers.empty() || specifiers.starts_with(format_terminator)) {
            return false;
        }

        bool has_qualification = true;

        if (specifiers.starts_with('q')) {
            qualified_ = qualification::qualified;
        } else if (specifiers.starts_with('u')) {
            qualified_ = qualification::unqualified;
        } else if (specifiers.starts_with(format_delimiter)) {
            has_qualification = false;
        } else {
            throw std::format_error("Invalid qualification specifier: expected 'q' for qualified or 'u' for unqualified.");
        }

        return has_qualification;
    }
};

#endif // BEMAN_ENUM_FORMATTING_FORMAT_HPP
