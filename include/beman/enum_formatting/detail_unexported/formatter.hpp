// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef BEMAN_ENUM_FORMATTING_UNEXPORTED_FORMATTER_HPP
#define BEMAN_ENUM_FORMATTING_UNEXPORTED_FORMATTER_HPP

#ifndef BEMAN_ENUM_FORMATTING_INCLUDED_FROM_INTERFACE_UNIT
    #include <concepts>
    #include <cstdint>
    #include <format>
    #include <ranges>
    #include <string_view>
    #include <type_traits>
    #include <utility>
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

    template<typename CharT>
    class format_specification {
    private:
        enum class consumption_signal : std::uint8_t {
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
        };

    public:
        enum class representation : std::uint8_t {
            unspecified,
            string,
            integer,
            type,
        };

        enum class qualification : std::uint8_t {
            unspecified,
            qualified,
            unqualified,
        };

    private:
        static constexpr CharT format_terminator = '}';
        static constexpr CharT format_delimiter  = ':';

        static constexpr CharT string_specifier  = 's';
        static constexpr CharT integer_specifier = 'i';
        static constexpr CharT type_specifier    = 't';

        static constexpr CharT qualified_specifier   = 'q';
        static constexpr CharT unqualified_specifier = 'u';
    
        std::basic_string_view<CharT> specifiers_;

        auto state_          = parser_state::initialized;
        auto representation_ = representation::unspecified;
        auto qualification_  = qualification::unspecified;

    public:
        constexpr explicit format_specification(std::basic_string_view<CharT> specifiers)
            : specifiers_{specifiers}
        {
            while (!specifiers_.empty() && state_ != parser_state::done && state_ != parser_state::empty) {
                if (transition_state(process_next_specifier(specifiers_)) == consumption_signal::consume) {
                    specifiers_.remove_prefix(1);
                }
            }
        }
        
        template<std::ranges::borrowed_range Range>
            requires std::ranges::contiguous_range<Range>
                  && std::ranges::sized_range<Range>
                  && std::same_as<std::remove_cv_t<std::ranges::range_value_t<Range>>, CharT>
        constexpr explicit format_specification(Range&& range)
            : format_specification{std::basic_string_view<CharT>{std::ranges::data(range), std::ranges::size(range)}}
        {
        }
        
        constexpr bool is_empty() const noexcept { return state_ == parser_state::empty; }

        constexpr auto begin() const noexcept { return specifiers_.begin(); }
        constexpr auto end()   const noexcept { return specifiers_.end(); }

        constexpr representation get_representation() const noexcept { return representation_; }
        constexpr qualification  get_qualification()  const noexcept { return qualification_; }

    private:
        constexpr consumption_signal transition_state(parser_state next_state)
        {
            const auto old_state = std::exchange(state_, next_state);
    
            using enum parser_state;
            using enum consumption_signal;
    
            switch (state_) {
                case invalid_qualification:
                    [[fallthrough]];
                case unexpected_specifier:
                    [[fallthrough]];
                case empty:
                    [[fallthrough]];
                case done:
                    return retain;
                case ready_to_forward:
                    return (old_state == invalid_representation || old_state == leading_delimiter) ? retain : consume;
                default:
                    return consume;
            }
        }
    
        constexpr parser_state process_next_specifier(auto specifiers)
        {
            using enum parser_state;
            switch (state_) {
                case initialized:
                    return handle_state_initialized(specifiers);
                case leading_delimiter:
                    return handle_state_leading_delimiter(specifiers);
                case valid_representation:
                    return handle_state_valid_representation(specifiers);
                case invalid_representation:
                    return handle_state_invalid_representation(specifiers);
                case valid_qualification:
                    return handle_state_valid_qualification(specifiers);
                case invalid_qualification:
                    return handle_state_invalid_qualification(specifiers);
                case unexpected_specifier:
                    return handle_state_unexpected_specifier(specifiers);
                case ready_to_forward:
                    return handle_state_ready_to_forward(specifiers);
                case empty:
                    return handle_state_empty(specifiers);
                default:
                    std::unreachable();
            }
        }
    
        constexpr parser_state handle_state_initialized(auto specifiers)
        {
            if (specifiers.starts_with(format_terminator)) {
                return parser_state::empty;
            } else if (specifiers.starts_with(format_delimiter)) {
                return parser_state::leading_delimiter;
            } else if (process_representation_specifier(specifiers)) {
                return parser_state::valid_representation;
            } else {
                return parser_state::invalid_representation;
            }
        }
    
        constexpr parser_state handle_state_leading_delimiter(auto specifiers)
        {
            if (specifiers.starts_with(format_terminator)) {
                return parser_state::empty;
            } else {
                return parser_state::ready_to_forward;
            }
        }
    
        constexpr parser_state handle_state_valid_representation(auto specifiers)
        {
            if (specifiers.starts_with(format_terminator)) {
                return parser_state::done;
            } else if (specifiers.starts_with(format_delimiter)) {
                return parser_state::ready_to_forward;
            } else if (process_qualification_specifier(specifiers)) {
                return parser_state::valid_qualification;
            } else {
                return parser_state::invalid_qualification;
            }
        }
    
        constexpr parser_state handle_state_valid_qualification(auto specifiers)
        {
            if (specifiers.starts_with(format_terminator)) {
                return parser_state::done;
            } else if (specifiers.starts_with(format_delimiter)) {
                return parser_state::ready_to_forward;
            } else {
                return parser_state::unexpected_specifier;
            }
        }
    
        constexpr parser_state handle_state_invalid_representation(auto specifiers)
        {
            if (specifiers.substr(0, specifiers.find(format_terminator)).contains(format_delimiter)) {
                throw std::format_error(
                    "Invalid representation specifier: expected 's' for enumerator name string, 'i' for underlying integral type, or 't' for enumeration type name."
                );
            } else {
                return parser_state::ready_to_forward;
            }
        }
    
        constexpr parser_state handle_state_invalid_qualification(auto specifiers)
        {
            throw std::format_error("Invalid qualification specifier: expected 'q' for qualified or 'u' for unqualified.");
        }
    
        constexpr parser_state handle_state_unexpected_specifier(auto specifiers)
        {
            throw std::format_error("Expected ':' delimiter or closing '}' after qualification specifier ('q' or 'u').");
        }
    
        constexpr parser_state handle_state_ready_to_forward(auto specifiers) { return parser_state::done; }
    
        constexpr parser_state handle_state_empty(auto specifiers) { return parser_state::done; }
    
        constexpr bool process_representation_specifier(auto specifiers)
        {
            if (specifiers.starts_with(string_specifier)) {
                representation_ = representation::string;
                return true;
            } else if (specifiers.starts_with(integer_specifier)) {
                representation_ = representation::integer;
                return true;
            } else if (specifiers.starts_with(type_specifier)) {
                representation_ = representation::type;
                return true;
            } else {
                return false;
            }
        }
    
        constexpr bool process_qualification_specifier(auto specifiers)
        {
            if (representation_ == representation::type) {
                throw std::format_error("Qualification ('q' or 'u') is invalid for type representation ('t').");
            }
    
            if (specifiers.starts_with(qualified_specifier)) {
                qualification_ = qualification::qualified;
                return true;
            } else if (specifiers.starts_with(unqualified_specifier)) {
                qualification_ = qualification::unqualified;
                return true;
            } else {
                return false;
            }
        }
    };
} // namespace beman::enum_formatting::unexported

#endif // BEMAN_ENUM_FORMATTING_UNEXPORTED_FORMATTER_HPP
