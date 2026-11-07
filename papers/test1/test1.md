---
title: "Scoped Enumeration Stringification and Annotation-Driven Defaultable Formatting with C++26 Static Reflection"
subtitle: "Extensions for `std::to_string` and `std::formatter`"
document: D0000R0
date: today
audience: Library Evolution Working Group (LEWG)
author:
  - name: Jeremy Murphy
    email: <jeremy.r.murphy at protonmail dot com>
toc: true
toc-depth: 3
---

# Abstract

This paper proposes ...

Introduce the problem domain and the proposed facility.

Explain the relationship between this proposal and existing C++ formatting facilities.

# Revision History

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
