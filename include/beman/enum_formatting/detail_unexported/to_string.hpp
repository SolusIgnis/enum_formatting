// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef BEMAN_ENUM_FORMATTING_UNEXPORTED_TO_STRING_HPP
#define BEMAN_ENUM_FORMATTING_UNEXPORTED_TO_STRING_HPP

#ifndef BEMAN_ENUM_FORMATTING_INCLUDED_FROM_INTERFACE_UNIT
    #include "../annotations.hpp"
    #include "concepts.hpp"
    #include "meta.hpp"

    #include <algorithm>
    #include <meta>
    #include <string>
    #include <type_traits>
    #include <utility>
#endif

namespace beman::enum_formatting::unexported {
    template<typename TargetCharT>
    constexpr auto str_convert(const std::string& narrow)
    {
        std::basic_string<TargetCharT> result;
        result.reserve(narrow.size());

        for (auto character : narrow) {
            result.push_back(static_cast<TargetCharT>(character));
        }

        return result;
    }

    template<>
    constexpr auto str_convert<char>(const std::string& narrow)
    {
        return narrow;
    }

    template<typename CharT, unexported::enumerable_scoped_enum E>
        requires std::same_as<CharT, char> || std::same_as<CharT, char8_t>
    constexpr std::basic_string<CharT> to_string(
        E value,
        [[maybe_unused]] bool use_qualified_name = false,
        bool use_qualified_fallback              = true,
        bool use_empty_fallback                  = false
    )
    {
        std::add_pointer_t<const CharT> scope_operator;
        if constexpr (std::same_as<CharT, char8_t>) {
            scope_operator = u8"::";
        } else {
            scope_operator = "::";
        }

        template for (constexpr auto enumerator : std::define_static_array(enumerators_of_by_tier(^^E)))
        {
            if (value == [:enumerator:]) {
                if (use_qualified_name) {
                    if constexpr (std::same_as<CharT, char8_t>) {
                        return std::u8string{} + std::meta::u8identifier_of(^^E) + scope_operator
                             + std::meta::u8identifier_of(enumerator);
                    } else {
                        return std::string{} + std::meta::identifier_of(^^E) + scope_operator
                             + std::meta::identifier_of(enumerator);
                    }
                } else {
                    if constexpr (std::same_as<CharT, char8_t>) {
                        return std::u8string{std::meta::u8identifier_of(enumerator)};
                    } else {
                        return std::string{std::meta::identifier_of(enumerator)};
                    }
                }
            }
        }

        if (use_empty_fallback) {
            return {};
        } else if (use_qualified_fallback) {
            if constexpr (std::same_as<CharT, char8_t>) {
                return std::u8string{} + std::meta::u8identifier_of(^^E) + scope_operator
                     + str_convert<CharT>(std::to_string(std::to_underlying(value)));
            } else {
                return std::string{} + std::meta::identifier_of(^^E) + scope_operator
                     + str_convert<CharT>(std::to_string(std::to_underlying(value)));
            }
        } else {
            return str_convert<CharT>(std::to_string(std::to_underlying(value)));
        }
    }
} // namespace beman::enum_formatting::unexported

#endif // BEMAN_ENUM_FORMATTING_UNEXPORTED_TO_STRING_HPP
