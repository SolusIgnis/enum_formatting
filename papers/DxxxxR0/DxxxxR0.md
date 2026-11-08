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

Define what this proposal intends to address and what remains outside its scope.

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

Explain the overall design and its guiding principles.

## Alternatives Considered

Describe alternative approaches and the rationale for accepting or rejecting them.

::: cmptable
### Option A
```cpp
//chose this one
```

### Option B
```cpp {.not_proposed}
//didn't do this
```
:::

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

### `std::annotations::formattable_with_default`

### `std::annotations::formattable`

### `std::annotations::canonical`

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

# References

- [Nxxxx] ISO/IEC 14882: Programming Languages — C++.
- [Pxxxx] Related WG21 proposal.
- [Pxxxx] Related WG21 proposal.
