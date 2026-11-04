// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef BEMAN_ENUM_FORMATTING_STRING_HPP
#define BEMAN_ENUM_FORMATTING_STRING_HPP

#ifndef BEMAN_ENUM_FORMATTING_INCLUDED_FROM_INTERFACE_UNIT
#include "detail_unexported/concepts.hpp"
#include "detail_unexported/to_string.hpp"

#include <string>
#endif

namespace beman::enum_formatting {
    template<unexported::enumerable_scoped_enum E>
    constexpr std::string to_string(E value)
    {
        return unexported::to_string(value);
    }
} // namespace beman::enum_formatting

#endif // BEMAN_ENUM_FORMATTING_STRING_HPP
