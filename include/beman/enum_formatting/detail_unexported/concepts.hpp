// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef BEMAN_ENUM_FORMATTING_UNEXPORTED_CONCEPTS_HPP
#define BEMAN_ENUM_FORMATTING_UNEXPORTED_CONCEPTS_HPP

#ifndef BEMAN_ENUM_FORMATTING_INCLUDED_FROM_INTERFACE_UNIT
    #include <meta>
#endif

namespace beman::enum_formatting::unexported {
    template<typename T>
    concept scoped_enum = std::meta::is_scoped_enum_type(^^T);

    template<typename T>
    concept enumerable_scoped_enum = scoped_enum<T> && std::meta::is_enumerable_type(^^T);

    template<typename T, typename Annotation>
    concept annotated_with = (!std::meta::annotations_of_with_type(^^T, ^^Annotation).empty());
    
    template<typename T>
    concept annotated_as_formattable = annotated_with<T, basic_formattable_with_default<char, char>>
                                    || annotated_with<T, basic_formattable_with_default<char, char8_t>>
                                    || annotated_with<T, basic_formattable_with_default<char8_t, char>>
                                    || annotated_with<T, basic_formattable_with_default<char8_t, char8_t>>;
} // namespace beman::enum_formatting::unexported

#endif // BEMAN_ENUM_FORMATTING_UNEXPORTED_CONCEPTS_HPP
