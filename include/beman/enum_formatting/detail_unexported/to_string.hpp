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
    constexpr std::basic_string<CharT>
        to_string(E value, bool use_qualified_name = false, bool use_qualified_fallback = true, bool use_empty_fallback = false)
    {
        constexpr auto scope_operator = std::invoke([] consteval {
            if constexpr (std::same_as<CharT, char8_t>) {
                return u8"::";
            } else {
                return "::";
            }
        });

        (void)
            use_qualified_name; //GCC 16.2 warns "unused-but-set-parameter" because it can't see the branch below when the expansion statement is empty (i.e., `E` declares no enumerators).
        template for (constexpr auto enumerator : std::define_static_array(enumerators_of_by_tier(^^E)))
        {
            if (value == [:enumerator:]) {
                if (use_qualified_name) {
                    return std::basic_string<CharT>{} + identifier_of<CharT>(^^E) + scope_operator
                         + identifier_of<CharT>(enumerator);
                } else {
                    return std::basic_string<CharT>{identifier_of<CharT>(enumerator)};
                }
            }
        }

        if (use_empty_fallback) {
            return {};
        } else if (use_qualified_fallback) {
            return std::basic_string<CharT>{} + identifier_of<CharT>(^^E) + scope_operator
                 + str_convert<CharT>(std::to_string(std::to_underlying(value)));
        } else {
            return str_convert<CharT>(std::to_string(std::to_underlying(value)));
        }
    }
} // namespace beman::enum_formatting::unexported

#endif // BEMAN_ENUM_FORMATTING_UNEXPORTED_TO_STRING_HPP
