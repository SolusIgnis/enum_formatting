---
title: "Scoped Enumeration Stringification and Annotation-Driven Defaultable Formatting with C++26 Static Reflection"
subtitle: "Extensions for `std::to_string` and `std::formatter`"
document: D0000R0
date: today
audience: Library Evolution Working Group (LEWG)
author:
  - name: Jeremy Murphy
    email: jeremy dot r dot murphy at protonmail dot com
toc: true
toc-depth: 3
---

# Abstract

This paper proposes ...

Introduce the problem domain and the proposed facility.

Explain the relationship between this proposal and existing C++ formatting facilities.

# Revision History {.collapsed}

## R0
- Initial revision.

# Motivation and Scope

## Motivation

Describe the problem being addressed and why existing facilities are insufficient.

## Scope

This proposal intentionally excludes unscoped enumerations from `std::to_string` overloads.
It includes both scoped and unscoped enumerations in the `std::formatter` specialization on an **opt-in** basis.

Unscoped enumerations implicitly convert to their underlying integral types, meaning calls such as `std::to_string(e)` are already well-formed today and produce numeric output.
Defining additional `std::to_string` overloads for unscoped enumerations would inevitably change the meaning of some existing code, violating the proposal's status as a pure extension.

As scoped enumerations do not have this implicit conversion, calls like `std::to_string(e)` are not currently valid for them, and the proposed overloads support them unconditionally.
However, programmers may have supplied their own `std::formatter` specializations for some enumeration types.
To avoid potential conflicts with existing user-supplied formatting customizations, the proposed partial specialization restricts participation to enumerations annotated as formattable with one of the proposed annotations.

## Examples

Provide motivating examples demonstrating the need for the proposed functionality.

```cpp
// Example usage goes here.
```

::: cmptable
### Before (Current C++)

```cpp
// Motivating example showing current verbose,
// error-prone, or inefficient code
#include <iostream>

void legacy_way() {
    // Current approach
}
```

### After (With Proposal)

```cpp
// Clean code using your proposed feature
void proposed_way() {
    // Proposed syntax or library feature
}
```
---

```cpp
void legacy_way() {
    // Current approach
}
```

```cpp
void proposed_way() {
    // Proposed syntax or library feature
}
```
:::

# Impact on the Standard

This is a **pure library extension**.

This proposal **does not** require any changes in the core language.
It **does not** alter the meaning or behavior of existing well-formed programs (save observability of new function overloads).
It **does not** depend on any other library extensions; however, it **would** evolve to take advantage of the adoption of attribute reflection or changes to structural type requirements.
It **does** account for and accommodate the eventual adoption of UTF-8 support in `std::formatter`.

This proposal introduces a new header, `<annotations>`, containing a new inline namespace, `std::annotations`.
It further introduces the class template `std::basic_formattable_with_default`, the alias template `std::formattable_with_default`, and the annotation objects `std::formattable` and `std::canonical` therein.
It also introduces the utility predicate `std::is_enumerated_value` in `<utility>`, a constrained partial specialization of `std::formatter` for annotated enumerations in `<format>`, and overloads of `std::to_string` for enumerable scoped enumerations in `<string>`.

# Design Decisions

## Overview

The core principle of the proposed design is to respect programmer intent, whether expressed or reasonably inferred.
This sometimes means crafting a more complex structure behind a simple facade in order to facilitate that expression.
C++ is a multi-paradigm language with a broad base of users and a rich history of trusting those individuals to decide what is best for their projects rather than forcing one-size-fits-all policy decisions.
(See [@D&E] section 4.2's rule: "Don't try to force people.")
The proposal also strives to maintain both backward and forward compatibility with the evolution of the Standard.

## Naming and Interface Design

Explain the public interface and naming decisions.

### Utility Functions

### Annotations

### Format Specifiers

#### Representation (Name String vs Underlying Integral Value)

#### Type-Qualification

#### Format Specification Delimiter

After considering several characters to separate the enumeration-specific portion of the format specification from the portion for formatting the specified representation, this proposal chose to use the `:` (format-specifier separator) as the delimiter.
This avoids introducing a different character into the format grammar and naturally extends the replacement field to give the appearance of a third section.
This results in replacement fields that look like `{:i:02X}`, `{:i}`, `{:i:}`, `{:}`, or `{::}`.

## Default Representation



## Default Type-Qualification

This paper proposes a "smart default" type-qualification where the default depends on the specified representation and whether the value corresponds to a named enumerator.
As the name of an enumerator often carries sufficient context for a familiar user to identify its probable type, the default in this case is unqualified.
For values without a named enumerator, the default for the string representation treats the value as a type-qualified pseudo-enumerator because the numeric value alone carries no context.
These defaults apply to both the `formatter` and to `std::to_string`.
When the specified format representation is the underlying integral type, the default is an unqualified number, trusting the caller to provide appropriate context (or to specify a type-qualified format if desired).

## Non-Enumerable Types

The `std::to_string` overloads are constrained to require enumerable scoped enumerations, so calls will fail to compile. The `formatter` specialization, on the other hand, rejects non-enumerable types during format-string parsing, at the moment the caller specifies representing the value as an enumerator name string. Instantiation of the `formatter` or annotation of an enumeration is permitted because it is perfectly reasonable for a programmer to format an enumeration value as a number or format the enumeration type name without the type being enumerable.

## Constraints and Limitations

### Reflecting on Attributes

Should [@P3385] be adopted into the Working Draft, this proposal would use its facilities to favor non-deprecated enumerator names over deprecated names.

### Structural Types

Should [@P4414] be adopted into the Working Draft, this proposal would modify the proposed definition of `std::basic_formattable_with_default` to encapsulate a `std::basic_string_view` rather than storing public non-static data members.
These data members are currently *unspecified* with the intent that implementations could plausibly replace them without breaking the binary representation of the annotation objects if the standard were to permit it in the future.
The revised definition would also employ the `structural` class-property-specifier to allow real encapsulation.

### UTF-8 Support in `std::formatter`

This proposal does not extend `std::formatter` to support formatting UTF-8 strings or character types directly.
If support such as that proposed in [@P3258] were to be adopted, the mechanism proposed here could be extended to connect that facility with `std::meta::u8identifier_of` by means of a `std::u8formattable_with_default` type alias.

This paper also does not propose to extend `std::formatter` to support UTF-8 characters in format strings.
If support were to be adopted, the mechanism proposed here would automatically interoperate through its character type template parameters that accept `char8_t` arguments.

## Alternatives Considered

### Unified Syntax vs. Separate Annotations

[@P4267R1] proposes `std::format_as_identifier` and `std::format_as_underlying` as separate annotations.
This requires enumeration type authors to select the representation of their enumeration types at the point of declaration and prevents users of those types from overriding that choice at their call sites.

This paper, instead, provides a unified format specification syntax to defer the choice of representation to the caller and a single format annotation type which still allows the creator of the type to select a per-type default.

### Annotation Class Template vs. Separate Annotation Types

The initial draft of the reference implementation had `formattable_with_default` and `formattable` annotations.
When considering how to be forward compatible with UTF-8 formatting, the obvious solution was `u8formattable_with_default` and `u8formattable`.
This works, but the definitions of the two classes are, aside from identifier names, identical.
In addition to duplicating the definitions, the `formatter` specialization had to replicate its annotation detection for both annotation types.
The straightforward solution was to make the representation character type a template parameter that distinguishes the representation families and unify the definitions into a single class template.
When exploring options for supporting non-`char`-based format specification strings, adding the format specification character type as a template parameter and connecting it with the character type of the `std::formatter` made that support relatively simple.

Thus, this paper relies on `std::basic_formattable_with_default<typename RepresentationCharT, typename FormatSpecifierCharT>` to build an extensible mechanism capable of supporting future `char8_t` formatting.
It further provides `std::formattable_with_default<typename CharT>` as a user-facing alias for `std::basic_formattable_with_default<char, CharT>` and holds space for `std::u8formattable_with_default<typename CharT>` as the corresponding alias for `std::basic_formattable_with_default<char8_t, CharT>`.
The annotation object `std::formattable` (like the as-yet-unproposed `std::u8formattable`) simplifies the case where no default format specification is provided.
Because the constructor of `std::basic_formattable_with_default` takes a reference to an array of `FormatSpecifierCharT`, CTAD correctly deduces `[[=formattable_with_default("i")]]` as `std::basic_formattable_with_default<char, char>`.
This provides the best of both worlds: clean annotation syntax, a unified annotation family, and a clear path for extension.

### Accessing the Annotated Default Format Specification

The syntax for accessing the default format specification stored by a `std::basic_formattable_with_default` object could take practically infinite forms.
Directly accessing the pointer stored by the constructor would work, but it would be preferable to avoid exposing a raw pointer or a public data member.
After considering many member function names such as `get`, `value`, `as_string`, `get_string_view`, `as_string_view`, `get_basic_string_view`, `as_basic_string_view`, a pattern appeared.
This operation requires converting the stored representation into a type-safe view of the characters stored by the annotation, which is idiomatically spelled `operator std::basic_string_view` for the stored character type.
Using the conversion operator provides a clean interface that does not require selecting a name.
Furthermore, should another type such as an explicit `std::string` copy be desired in the future, the door is open to extension.

### String Representation of Unenumerated Values

```cpp
enum class rgb_color : std::uint32_t {
    red   = 0xFF0000,
    green = 0x00FF00,
    blue  = 0x0000FF,
};

auto black     = rgb_color{0x000000};
auto black_str = std::to_string(black);
```

Considering this basic scoped enumeration representing RGB colors, what should be the string representation of an unnamed color?
Since named enumerators are written `rgb_color::red`, `rgb_color::green`, and `rgb_color::blue`, this proposal uses a parallel syntax for unenumerated values and produces `rgb_color::0`, treating the underlying value as a pseudo-enumerator.
This preserves the enumeration type and underlying value for debugging/logging purposes.
Other candidate representations discard some of this information or treat unenumerated values as construction expressions that are less readily parsed by consumers.
The proposed representation also avoids favoring either parenthesized or braced initialization syntax, each of which has its own stylistic advocates and semantic distinctions in C++.

| Representation | Output for `black` | Status | Trade-Off |
| :--- | :--- | :--- | :--- |
| **Type-Qualified Value** | `"rgb_color::0"` | **Proposed** | Keeps both type and value; parallels scoped enumerator syntax. |
| **Cast Expression** | `"rgb_color(0)"` | Alternative | Keeps both type and value; valid C++ expression but requires an additional token after the value. |
| **Braced Initializer** | `"rgb_color{0}"` | Alternative | Same as cast expression but with braces instead of parentheses. |
| **`std::optional`** | `std::nullopt` | *Not Proposed* | Changes return type from other `std::to_string` overloads; may break generic code. |
| **Raw Underlying Value** | `"0"` | *Not Proposed* | Loses the enumeration type; indistinguishable from plain integers. |
| **Fixed Fallback** | `"<UNNAMED>"` / `"<UNKNOWN>"` | *Not Proposed* | Loses both type and value. |

Should the programmer desire either of the last two options, the `is_enumerated_value` utility function proposed here provides that functionality:

:::cmptable
#### Fallback to Pure Value
```cpp
if (!is_enumerated_value(black)) {
    black_str = std::to_string(
        std::to_underlying(black)
    );
}
```

#### Fixed Fallback
```cpp
if (!is_enumerated_value(black)) {
    black_str = std::string{"<UNNAMED>"};
}
```
:::

### Rejecting Non-Enumerable Types at `formatter` Instantiation

The `std::formatter` specialization could be constrained to require enumerable enumerations, matching `std::to_string`.
This paper finds that approach unacceptable because `std::formatter` supports formatting modes (i.e., the underlying numeric value and the enumeration type name) that do not depend on enumerator names and remain valid for non-enumerable types.
The chosen design follows the [@D&E] principle that C++ rejects actual errors rather than potential errors.

# Proposal

## Overview

Summarize the proposed functionality.

## Stringification

## Formatting Semantics

Describe the formatting behavior, including:

- Default representations.
- Format-specification syntax.
- Qualified and unqualified representations.
- Integer representations.
- Forwarded format specifications.
- Type-name representations.

## Enumerator Aliases

Describe canonical enumerator selection and alias handling.

## Annotations

Describe the annotation facilities and their semantics.

### `std::annotations::canonical`

### `std::annotations::basic_formattable_with_default`

### `std::annotations::formattable_with_default`

### `std::annotations::formattable`

### UTF8 Support

#### `std::annotations::u8formattable_with_default`

#### `std::annotations::u8formattable`

## Examples

Provide representative examples demonstrating the proposed functionality.

# Implementation Experience

Describe existing implementations, compiler support, implementation experience, and testing.

## Reference Implementation

Describe the reference implementation.

## Implementation Status

Describe compiler and library support.

## Testing

Describe the testing strategy and relevant test coverage.

# Feature-Test Macro

Specify the proposed feature-test macro and its value.

# Proposed Wording

This section contains the proposed changes to the C++ working draft.

Use markup tags like `[added text]{.add}`, `[removed text]{.rm}`, and `[removed text](added text){.sub}`.

> Modify Section [general.description] as follows:
>
> The behavior of the function shall be modified to [support the new functionality]{.add} [throw an exception]{.rm}.

## Header Changes

[Insert proposed wording.]

## Library Specification Changes

[Insert proposed wording.]

## Feature-Test Macro Changes

[Insert proposed wording.]

# Acknowledgements

Acknowledge contributors and reviewers.

# Placeholder Biblio

- [Nxxxx] ISO/IEC 14882: Programming Languages — C++.
- [Pxxxx] Related WG21 proposal.
- [Pxxxx] Related WG21 proposal.

---
references:
  - id: D&E
    citation-label: D&E
    type: book
    title: "The Design and Evolution of C++"
    author:
      - family: Stroustrup
        given: Bjarne
    publisher: Addison-Wesley
    isbn: "0-201-54330-3"
---
