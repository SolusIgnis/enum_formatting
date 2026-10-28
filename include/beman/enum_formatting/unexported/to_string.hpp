// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef BEMAN_ENUM_FORMATTING_UNEXPORTED_TO_STRING_HPP
#define BEMAN_ENUM_FORMATTING_UNEXPORTED_TO_STRING_HPP

#ifndef BEMAN_ENUM_FORMATTING_INCLUDED_FROM_INTERFACE_UNIT
#include <meta>
#include <string>
#include <utility>
#endif

namespace beman::enum_formatting::unexported {
    template<typename E>
        requires (std::meta::is_scoped_enum_type(^^E))
    constexpr std::string to_string(E value, [[maybe_unused]] bool use_qualified_name = false, bool use_qualified_fallback = true, bool use_empty_fallback = false)
    {
        template for (constexpr auto enumerator : std::define_static_array(std::meta::enumerators_of(^^E))) {
            if (value == [:enumerator:]) {
                if (use_qualified_name) {
                    return std::string{} + std::meta::identifier_of(^^E) + "::" + std::meta::identifier_of(enumerator);
                } else {
                    return std::string{std::meta::identifier_of(enumerator)};
                }
            }
        }

        if (use_empty_fallback) {
            return std::string{};
        } else if (use_qualified_fallback) {
            return std::string{} + std::meta::identifier_of(^^E) + "::" + std::to_string(std::to_underlying(value));
        } else {
            return std::to_string(std::to_underlying(value));
        }
    }
} // namespace beman::enum_formatting::unexported

#endif // BEMAN_ENUM_FORMATTING_UNEXPORTED_TO_STRING_HPP
