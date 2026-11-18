// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef BEMAN_ENUM_FORMATTING_UNEXPORTED_META_HPP
#define BEMAN_ENUM_FORMATTING_UNEXPORTED_META_HPP

#ifndef BEMAN_ENUM_FORMATTING_INCLUDED_FROM_INTERFACE_UNIT
    #include "../annotations.hpp"

    #include <meta>
    #include <string_view>
#endif

namespace beman::enum_formatting::unexported {
    consteval bool is_specialization_of(std::meta::info candidate, std::meta::info primary_template)
    {
        if (!std::meta::is_template(primary_template)) {
            throw std::meta::exception("reflection does not represent a template", primary_template);
        } else if (std::meta::is_alias_template(primary_template)) {
            std::string_view display = std::meta::display_string_of(primary_template);
            std::string_view simple_display = "without the alias";
            if (auto equals_marker = display.find("= "); equals_marker != std::string_view::npos) {
                auto substring = display.substr(equals_marker + 2);
                simple_display = substring.substr(0, substring.find('<'));
            }
            throw std::meta::exception(std::string{"reflection does not represent a primary template; try "} + simple_display, primary_template);
        } else if (std::meta::is_template(candidate)) {
            throw std::meta::exception("reflection must not represent a template: partial specialization is not currently supported", candidate);
        }

        if (std::meta::is_annotation(candidate) || std::meta::is_variable(candidate) || std::meta::is_object(candidate)) {
            candidate = std::meta::type_of(candidate);
        }

        candidate = std::meta::dealias(candidate);

        if (std::meta::is_type(candidate)) {
            candidate = std::meta::remove_cvref(candidate);
        }

        return std::meta::has_template_arguments(candidate) && std::meta::template_of(candidate) == primary_template;
    }

    consteval auto annotations_of_with_template(std::meta::info entity, std::meta::info annotation_template)
    {
        auto annotations = std::meta::annotations_of(entity);
        decltype(annotations) results;

        for (auto annotation : annotations) {
            if (is_specialization_of(annotation, annotation_template)) {
                results.push_back(annotation);
            }
        }

        return results;
    }

    consteval bool is_annotated_as_formattable(std::meta::info candidate) {
        return !annotations_of_with_template(candidate, ^^::beman::enum_formatting::annotations::basic_formattable_with_default).empty();
    }
} // namespace beman::enum_formatting::unexported

#endif // BEMAN_ENUM_FORMATTING_UNEXPORTED_META_HPP
