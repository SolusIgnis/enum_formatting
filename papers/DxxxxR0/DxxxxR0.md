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

This proposal does not address stringifying or formatting unscoped enumerations. Due to implicit conversions, both `std::to_string` and `std::format` can accept unscoped enumerations and treat them as integers. As such, applying these overloads to them would change the behavior of existing code. Should the committee prefer to do so, this proposal could trivially be extended to them, but such extension is **not** currently proposed.

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

Describe the impact on the C++ standard, including:

- Which existing library components are affected.
- Whether new library components are introduced.
- Whether the proposal changes existing behavior.
- Whether the proposal requires language changes.
- Compatibility and implementation considerations.

# Design Decisions

## Overview

The core principle of the proposed design is to respect programmer intent, whether expressed or reasonably inferred. This sometimes means crafting a more complex structure behind a simple facade in order to facilitate that expression. C++ is a multi-paradigm language with a broad base of users and a rich history of trusting those individuals to decide what is best for their projects rather than forcing one-size-fits-all policy decisions. (See [@D&E] section 4.2's rule: "Don't try to force people.") The proposal also strives to maintain both backward and forward compatibility with the evolution of the Standard.

## Constraints and Limitations

This proposal intentionally excludes unscoped enumerations from `std::formatter` specialization and `std::to_string` overloads. It further only includes scoped enumerations in the `std::formatter` specialization on an **opt-in** basis.

Unscoped enumerations implicitly convert to their underlying integral types, meaning calls such as `std::format("{}", e)` and `std::to_string(e)` are already well-formed today and produce numeric output. Defining a constrained `std::formatter` specialization or additional `std::to_string` overloads for unscoped enumerations would inevitably change the meaning of existing code. Furthermore, unscoped enumerations are frequently used in legacy code as named integer constants or bitwise flag masks rather than discrete symbolic values. Restricting support to scoped enumerations preserves strict backward compatibility while reinforcing modern C++ type safety.

As scoped enumerations do not have this implicit conversion, calls like `std::to_string(e)` are not currently valid for them, and the proposed overloads support them unconditionally. However, programmers may have supplied their own `std::formatter` specializations for some scoped enumeration types. To avoid conflicts with existing user-supplied specializations, scoped enumerations are excluded from the proposed partial specialization unless they are annotated as formattable with one of the proposed annotations.

## Naming and Interface Design

Explain the public interface and naming decisions.

### Utility Functions

### Annotations

### Format Specifiers

#### Representation (Name String vs Underlying Integral Value)

#### Type-Qualification

## Default Representation

## Default Type-Qualification

This paper proposes a "smart default" type-qualification where the default depends on the specified representation and whether the value corresponds to a named enumerator. As the name of an enumerator often carries sufficient context for a familiar user to identify its probable type, the default in this case is unqualified. For values without a named enumerator, the default for the string representation treats the value as a type-qualified pseudo-enumerator because the numeric value alone carries no context. These defaults apply to both the `formatter` and to `std::to_string`. When the specified format representation is the underlying integral type, the default is an unqualified number, trusting the caller to provide appropriate context (or to specify a type-qualified format if desired).

## Non-Enumerable Types

The `std::to_string` overloads are constrained to require enumerable scoped enumerations, so calls will fail to compile. The `formatter` specialization, on the other hand, rejects non-enumerable types during format-string parsing, at the moment the caller specifies representing the value as an enumerator name string. Instantiation of the `formatter` or annotation of an enumeration is permitted because it is perfectly reasonable for a programmer to format an enumeration value as a number or format the enumeration type name without the type being enumerable.

## Alternatives Considered

Describe alternative approaches and the rationale for accepting or rejecting them.

### Unified Syntax vs. Separate Annotations

### Annotation Class Template vs. Separate Annotation Types

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

Considering this basic scoped enumeration representing RGB colors, what should be the string representation of an unnamed color? Since named enumerators are written `rgb_color::red`, `rgb_color::green`, and `rgb_color::blue`, this proposal uses a parallel syntax for unenumerated values and produces `rgb_color::0`, treating the underlying value as a pseudo-enumerator. This preserves the enumeration type and underlying value for debugging/logging purposes. Other candidate representations discard some of this information or treat unenumerated values as construction expressions.

| Representation | Output for `black` | Status | Trade-Off |
| :--- | :--- | :--- | :--- |
| **Type-Qualified Value** | `"rgb_color::0"` | **Proposed** | Keeps both type and value; parallels scoped enumerator syntax. |
| **Cast Expression** | `"rgb_color(0)"` | Alternative | Keeps both type and value; valid C++ expression, but breaks `enumeration::enumerator` visual symmetry. |
| **Braced Initializer** | `"rgb_color{0}"` | Alternative | Same as cast expression. |
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

We considered constraining the `std::formatter` specialization to require enumerable scoped enumerations, matching `std::to_string`. That approach is unacceptable because `std::formatter` supports formatting modes (i.e., the underlying numeric value and the enumeration type name) that do not depend on enumerator names and remain valid for non-enumerable types. The chosen design follows the [@D&E] principle that C++ rejects actual errors rather than potential errors.

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
