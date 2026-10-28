// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef BEMAN_ENUM_FORMATTING_UTILITY_HPP
#define BEMAN_ENUM_FORMATTING_UTILITY_HPP

#ifndef BEMAN_ENUM_FORMATTING_INCLUDED_FROM_INTERFACE_UNIT
#include <meta>
#endif

namespace beman::enum_formatting {
    template<typename E>
        requires (std::meta::is_scoped_enum_type(^^E))
    bool is_enumerated_value(E value)
    {
        template for (constexpr auto enumerator : std::define_static_array(std::meta::enumerators_of(^^E))) {
            if (value == [:enumerator:]) {
                return true;
            }
        }

        return false;
    }
} // namespace beman::enum_formatting

#endif // BEMAN_ENUM_FORMATTING_UTILITY_HPP
