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

The core principle of the proposed design is to respect programmer intent, whether expressed or reasonably inferred. This sometimes means crafting a more complex structure behind a simple facade in order to facilitate that expression. C++ is a multi-paradigm language with a broad base of users and a rich history of trusting those individuals to decide what is best for their projects rather than forcing one-size-fits-all policy decisions.[^de-rules] The proposal also strives to maintain both backward and forward compatibility with the evolution of the Standard.
[^de-rules]: See [@D&E] Chapter 4: Rules X and Y.

## Alternatives Considered

Describe alternative approaches and the rationale for accepting or rejecting them.

### Unified Syntax

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

### Default Representation

### Default Type-Qualification

### Format Specifiers

### Unenumerable Types

#### Representation (Name String vs Underlying Integral Value)

#### 

## Naming and Interface Design

Explain the public interface and naming decisions.

## Constraints and Limitations

Describe any deliberate restrictions or limitations.

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
    publisher-place: Reading, MA
    issued:
      date-parts:
        - [1994]
    ISBN: "0-201-54330-3"
---
