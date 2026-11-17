// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef BEMAN_ENUM_FORMATTING_UNEXPORTED_META_HPP
#define BEMAN_ENUM_FORMATTING_UNEXPORTED_META_HPP

#ifndef BEMAN_ENUM_FORMATTING_INCLUDED_FROM_INTERFACE_UNIT
    #include "../annotations.hpp"

    #include <meta>
#endif

namespace beman::enum_formatting::unexported {
    consteval bool is_specialization_of(std::meta::info r, std::meta::info template_r)
    {
        return false;
    }
} // namespace beman::enum_formatting::unexported

#endif // BEMAN_ENUM_FORMATTING_UNEXPORTED_META_HPP
