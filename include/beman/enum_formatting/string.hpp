// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef BEMAN_ENUM_FORMATTING_STRING_HPP
#define BEMAN_ENUM_FORMATTING_STRING_HPP

#include "unexported/to_string.hpp"

#include <meta>
#include <string>

namespace beman::enum_formatting {
    template<typename E>
        requires (std::meta::is_scoped_enum_type(^^E))
    constexpr std::string to_string(E value)
    {
        return unexported::to_string(value);
    }
} // namespace beman::enum_formatting

#endif // BEMAN_ENUM_FORMATTING_STRING_HPP
