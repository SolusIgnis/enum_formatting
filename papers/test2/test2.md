---
title: "Scoped Enumeration Stringification and Annotation-Driven Defaultable Formatting with C++26 Static Reflection"
subtitle: "Extensions for `std::to_string` and `std::formatter`"
document: D0000R0
date: today
audience: Library Evolution Working Group (LEWG)
author:
  - name: Jeremy Murphy
    email: jeremy.r.murphy at protonmail dot com
toc: true
toc-depth: 3
---

# Abstract

A short 1–2 paragraph overview explaining the proposal, the problem it solves, and the basic approach being proposed for C++.

# Revision History

## R0
- Initial revision.

# Motivation and Scope

Explain why this proposal exists, what real-world use cases it addresses, and the limitations of current C++.

## Before / After Comparison

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

# Design Decisions

Explain the technical design choices, trade-offs, ABI implications, and feature interactions.

## Alternatives Considered

* **Option A:** Description of an alternative approach and reasons for rejection.
* **Option B:** Another candidate syntax or API and its drawbacks.

# Proposed Wording

Draft edits relative to the C++ Working Draft. Use markup tags like `<ins>added text</ins>` and `<del>removed text</del>`.

> Modify Section [general.description] as follows:
>
> The behavior of the function shall be modified to <ins>support the new functionality</ins> <del>throw an exception</del>.

# Implementation Experience

Note whether a prototype exists, compiler branches where it has been implemented, or user experience gained from library implementations.

# References
