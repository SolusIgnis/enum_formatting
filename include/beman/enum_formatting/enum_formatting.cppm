export module beman.enum_formatting;

import std;

#define BEMAN_ENUM_FORMATTING_INCLUDED_FROM_INTERFACE_UNIT

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Winclude-angled-in-module-purview"

//NOTE: These includes are in dependency order.
//      They cannot safely include their own dependencies
//      while building the module because doing so would
//      export the dependency header transitively. Thus,
//      they include dependencies only when we have not
//      defined BEMAN_ENUM_FORMATTING_INCLUDED_FROM_INTERFACE_UNIT.

#include <beman/enum_formatting/unexported/to_string.hpp>

export {
#include <beman/enum_formatting/utility.hpp>
#include <beman/enum_formatting/annotations.hpp>
#include <beman/enum_formatting/string.hpp>

#include <beman/enum_formatting/format.hpp>
}
#pragma clang diagnostic pop
