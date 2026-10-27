// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef BEMAN_ENUM_FORMATTING_ENUM_FORMATTING_HPP
#define BEMAN_ENUM_FORMATTING_ENUM_FORMATTING_HPP

#include <beman/enum_formatting/config.hpp>

#if BEMAN_ENUM_FORMATTING_USE_MODULES() && !defined(BEMAN_ENUM_FORMATTING_INCLUDED_FROM_INTERFACE_UNIT)

import beman.enum_formatting;

#else

    #include <beman/enum_formatting/annotations.hpp>
    #include <beman/enum_formatting/string.hpp>
    #include <beman/enum_formatting/utility.hpp>

    #include <beman/enum_formatting/format.hpp>

#endif // BEMAN_ENUM_FORMATTING_USE_MODULES() &&
       // !defined(BEMAN_ENUM_FORMATTING_INCLUDED_FROM_INTERFACE_UNIT)

#endif // BEMAN_ENUM_FORMATTING_ENUM_FORMATTING_HPP
