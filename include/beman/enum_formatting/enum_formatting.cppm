module;
#define BEMAN_ENUM_FORMATTING_INCLUDED_FROM_INTERFACE_UNIT
#include <beman/enum_formatting/enum_formatting.hpp>

export module beman.enum_formatting;

export namespace beman::enum_formatting::annotations {
    using beman::enum_formatting::annotations::formattable_with_default;
    using beman::enum_formatting::annotations::formattable;
}

export namespace beman::enum_formatting {
    using beman::enum_formatting::to_string;
    using beman::enum_formatting::is_enumerated_value;
}

using std::formatter;
