// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef BEMAN_ENUM_FORMATTING_UNEXPORTED_FORMATTER_HPP
#define BEMAN_ENUM_FORMATTING_UNEXPORTED_FORMATTER_HPP

#ifndef BEMAN_ENUM_FORMATTING_INCLUDED_FROM_INTERFACE_UNIT
    #include <cstdint>
#endif

namespace beman::enum_formatting::unexported {
    class enum_formatter_base {
    protected:
        enum class specifier_consumption_t : std::uint8_t {
            retain,
            consume,
        };

        enum class parser_state : std::uint8_t {
            initialized,
            leading_delimiter,
            valid_representation,
            invalid_representation,
            valid_qualification,
            invalid_qualification,
            unexpected_specifier,
            ready_to_forward,
            empty,
            done,
        } state_ = parser_state::initialized;
    };
} // namespace beman::enum_formatting::unexported

#endif // BEMAN_ENUM_FORMATTING_UNEXPORTED_FORMATTER_HPP
