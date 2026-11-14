// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef BEMAN_ENUM_FORMATTING_UNEXPORTED_FORMATTER_HPP
#define BEMAN_ENUM_FORMATTING_UNEXPORTED_FORMATTER_HPP

#ifndef BEMAN_ENUM_FORMATTING_INCLUDED_FROM_INTERFACE_UNIT
//No header dependencies, but structure retained for maintainability.
#endif

namespace beman::enum_formatting::unexported {
    class enum_formatter_base {
    protected:
        enum class specifier_consumption_t : std::uint8_t {
            retain,
            consume,
        };

        enum class parser_state : std::uint8_t {
            init,
            init_delimiter,
            has_representation,
            invalid_representation,
            has_qualification,
            invalid_qualification,
            extra_specifiers,
            has_delimiter,
            empty,
            done,
        } state_ = parser_state::init;
    };
} // namespace beman::enum_formatting::unexported

#endif // BEMAN_ENUM_FORMATTING_UNEXPORTED_FORMATTER_HPP
