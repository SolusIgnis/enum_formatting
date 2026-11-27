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
toc-depth: 4
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

_Describe the problem being addressed and why existing facilities are insufficient._


Enumeration types are frequently used to represent finite sets of named values, and those names often carry more meaning than their underlying integral representations.
When enumerations are used in diagnostics, logging, user interfaces, serialization, or other contexts requiring textual output, representing an enumerated value by its identifier is often preferable to representing it numerically.
However, the C++ Standard Library does not currently provide a general facility for converting an enumeration value to the identifier of its corresponding enumerator or for formatting enumerations in this manner.

Programmers have consequently resorted to a variety of workarounds, including manually maintained `switch` statements, macro-generated enumerator lists, and compiler-specific function-signature parsing techniques.
These approaches introduce varying degrees of maintenance burden, portability concerns, and implementation complexity.
Even when a library provides enumeration stringification, integrating it with the standard formatting facilities may require additional customization, preventing users from naturally composing enumeration representations with the formatting capabilities already available for other types.

This proposal provides a standard mechanism for stringifying enumerated values and formatting enumerations using their enumerator identifiers, underlying integral values, or type names.
It further provides mechanisms for selecting canonical names among aliases, specifying per-type formatting defaults, and controlling type qualification.
By integrating enumeration formatting with the existing standard library facilities, this proposal seeks to replace disparate workarounds with a portable, composable, and extensible interface.



Scoped enumerations are widely used in modern C++ to prevent implicit conversions to integers and avoid polluting enclosing scopes with enumerator names.
However, the standard library still lacks direct, ergonomic support for turning enumeration values into strings or formatting them with `std::format` and `std::print`.

Today, programmers who want to log, display, or serialize a scoped enumeration must choose between writing repetitive boilerplate (such as `switch` statements or X-macros), adopting third-party libraries that rely on compiler-specific signature-parsing workarounds, or authoring custom `std::formatter` specializations for individual enum types.
Meanwhile, `std::to_string` already serves as a familiar single-argument stringification entry point across the standard library, yet provides no overloads for scoped enumerations.

Even when enumerations are formatted, existing facilities provide no standard way to express author or call-site intent around common enumeration idioms:

* **Canonical names for aliased values:** Enumerations frequently define multiple enumerators with the same underlying value (such as aliases or status codes), with no standard way to indicate which identifier is canonical.
* **Opt-in and configurable defaults:** Users may already provide custom `std::formatter` specializations for certain enumerations, or want an enumeration to default to a specific representation or format string while still allowing call-site overrides.
* **Unenumerated values:** Values outside the declared enumerators can arise via casts or external input, requiring predictable behavior and a clear way to test whether a value corresponds to a named enumerator.

C++26 reflection (`[@P2996R13]`) and annotations (`[@P3393R4]`) provide the compile-time introspection and metadata mechanisms needed to inspect enumerators and attach library vocabulary cleanly.
This proposal builds on those facilities to provide `std::to_string` overloads for enumerable scoped enumerations and an opt-in, annotation-driven `std::formatter` specialization, supported by `<annotations>` and `std::is_enumerated_value`.



## Scope

This proposal intentionally excludes unscoped enumerations from `std::to_string` overloads.
It includes both scoped and unscoped enumerations in the `std::formatter` specialization on an **opt-in** basis.

Unscoped enumerations implicitly convert to their underlying integral types, meaning calls such as `std::to_string(e)` are already well-formed today and produce numeric output.
Defining additional `std::to_string` overloads for unscoped enumerations would inevitably change the meaning of some existing code, violating the proposal's status as a pure extension.

As scoped enumerations do not have this implicit conversion, calls like `std::to_string(e)` are not currently valid for them, and the proposed overloads support them unconditionally.
However, programmers may have supplied their own `std::formatter` specializations for some enumeration types.
To avoid potential conflicts with existing user-supplied formatting customizations, the proposed partial specialization restricts participation to enumerations annotated as formattable with one of the proposed annotations.

## Examples

::: cmptable
### Before (Macros and Signature Parsing)

```cpp
#define COLOR_LIST(X)  \
    X(red,   0xFF0000) \
    X(green, 0x00FF00) \
    X(blue,  0x0000FF)

enum class color {
#define AS_ENUM(name, val) name = val,
    COLOR_LIST(AS_ENUM)
#undef AS_ENUM
};

constexpr std::string_view
  to_string(color c) noexcept {
    switch (c) {
#define AS_CASE(name, val) \
    case color::name: return #name;
        COLOR_LIST(AS_CASE)
#undef AS_CASE
    }
    return "<unknown>";
}

void log_color(color c) {
    std::print("{:>8}", to_string(c)); 
}
```

### After (Annotation-Driven Formatter)

```cpp
enum class [[=std::formattable]] color {
    red   = 0xFF0000,
    green = 0x00FF00,
    blue  = 0x0000FF, 
};

void log_color(color c) {
    std::print("{:>8}", c); 
}
```
---

```cpp
template <auto value>
constexpr std::string_view
  enum_name_impl() {
#if defined(__clang__) || defined(__GNUC__)
    return __PRETTY_FUNCTION__;
#elif defined(_MSC_VER)
    return __FUNCSIG__;
#endif
}

template<typename E>
    requires std::is_scoped_enum_v<E>
std::string to_string(E value) {
    //...compile-time probe for enumerators
    //using enum_name_impl to extract
    //signatures and non-portable string
    //processing of the compiler-specific
    //signature string happens and a
    //character range is copied into storage
    //for each found enumerator

    return lookup_with_fallback(storage,
        value);
}

enum class status {
    success     = 1,
    in_progress = 2,
    error       = 3,
    failed      = 3,
};

void log_status(status s) {
    std::print("{:^14}", to_string(s)); 
}
```

```cpp
enum class [[=std::formattable]] status {
    success                    = 1,
    in_progress                = 2,
    error                      = 3,
    failed [[=std::canonical]] = 3,
};

void log_status(status s) {
    std::print("{:^14}", s); 
}
```
:::

*The legacy workarounds in the "Before" column are illustrative sketches of common macro and signature parsing techniques rather than complete, production-quality implementations. The reality would probably be an order of magnitude worse.*

::: cmptable
### Before (Custom Formatter with Manual Switch)

```cpp
export module net.telnet:types;

import std;

export namespace net::telnet {
    enum class command : std::uint8_t {
        eor = 0xEF, ///< End of Record
        se  = 0xF0, ///< Subnegotiation End
        nop = 0xF1, ///< No Operation
        //...13 enumerators elided for space
        iac = 0xFF, ///< Interpret As Command
    }; //enum class command

    enum class negotiation_direction : std::uint8_t {
        local,  ///< Local side ("us", sends WILL/WONT, receives DO/DONT)
        remote, ///< Remote side ("them", sends DO/DONT, receives WILL/WONT)
    }; //enum class negotiation_direction
} //namespace net::telnet
```

### After (Annotation-Driven Formatter)

```cpp
export module net.telnet:types;

import std;

export namespace net::telnet {
    enum class [[=std::formattable]] command : std::uint8_t {
        eor = 0xEF, ///< End of Record
        se  = 0xF0, ///< Subnegotiation End
        nop = 0xF1, ///< No Operation
        //...13 enumerators elided for space
        iac = 0xFF, ///< Interpret As Command
    }; //enum class command

    enum class [[=std::formattable]] negotiation_direction : std::uint8_t {
        local,  ///< Local side ("us", sends WILL/WONT, receives DO/DONT)
        remote, ///< Remote side ("them", sends DO/DONT, receives WILL/WONT)
    }; //enum class negotiation_direction
} //namespace net::telnet
```

---

```cpp
template<>
struct std::formatter<::net::telnet::command, char> {
private:
    char presentation_ = 'n'; ///< Format specifier: 'n' for name, 'x' or 'X' for hex.
public:
    constexpr auto parse(format_parse_context& ctx)
    {
        auto view = std::ranges::subrange(ctx.begin(), ctx.end());
        if (!view.empty()) {
            const char c = view.front();
            if (c == 'n' || c == 'x' || c == 'X') {
                presentation_ = c;
                view          = view.advance(1);
            }
        }
        if (!view.empty() && view.front() != '}') {
            throw std::format_error("Invalid format specifier for telnet::command");
        }
        return view.begin();
    } //parse(format_parse_context&)

    template<typename FormatContext>
    auto format(::net::telnet::command cmd, FormatContext& ctx) const
    {
        std::string_view name;
        switch (cmd) {
            case ::net::telnet::command::eor:
                name = "eor";
                break;
            case ::net::telnet::command::se:
                name = "se";
                break;
            case ::net::telnet::command::nop:
                name = "nop";
                break;
            //...13 cases elided for space
            case ::net::telnet::command::iac:
                name = "iac";
                break;
            default:
                name = "UNKNOWN";
                break;
        }

        if (presentation_ == 'n') {
            return std::format_to(ctx.out(), "{}", name);
        } else if (presentation_ == 'x') {
            return std::format_to(ctx.out(), "0x{:02x}", std::to_underlying(cmd));
        } else if (presentation_ == 'X') {
            return std::format_to(ctx.out(), "0x{:02X}", std::to_underlying(cmd));
        }
    } //format(::net::telnet::command, FormatContext&)
}; //struct formatter<::net::telnet::command>

template<>
struct std::formatter<::net::telnet::negotiation_direction, char> {
    constexpr auto parse(format_parse_context& ctx)
    {
        auto view = std::ranges::subrange(ctx.begin(), ctx.end());
        if (!view.empty() && view.front() != '}') {
            throw std::format_error("Invalid format specifier for negotiation_direction");
        }
        return view.begin();
    } //parse(format_parse_context&)

    template<typename FormatContext>
    auto format(::net::telnet::negotiation_direction dir, FormatContext& ctx) const
    {
        std::string_view name = (dir == ::net::telnet::negotiation_direction::local ? "local" : "remote");
        return std::format_to(ctx.out(), "{}", name);
    } //format(::net::telnet::negotiation_direction, FormatContext&)
}; //struct formatter<::net::telnet::negotiation_direction>
```

```cpp
//empty
```

---

```cpp
template<typename... Args>
void log_error(const std::error_code& ec,
               std::format_string<std::remove_cvref_t<Args>...> fmt,
               Args&&... args)
{
    std::cerr << "Telnet FSM error: " << ec.message() << " (" << std::format(fmt, std::forward<Args>(args)...) << ")\n";
}

using namespace ::net::telnet;
void contrived_use(std::error_code ec, command  cmd)
{
    log_error(ec, "cmd: {}", cmd);
    log_error(ec, "cmd: {:n}", cmd);
    log_error(ec, "cmd: {:x}", cmd);
    log_error(ec, "cmd: {:X}", cmd);
    log_error(ec, "cmd: {0:n} (0x{0:X})", cmd);
}
```

```cpp
template<typename... Args>
void log_error(const std::error_code& ec,
               std::format_string<std::remove_cvref_t<Args>...> fmt,
               Args&&... args)
{
    std::cerr << "Telnet FSM error: " << ec.message() << " (" << std::format(fmt, std::forward<Args>(args)...) << ")\n";
}

using namespace ::net::telnet;
void contrived_use(std::error_code ec, command  cmd)
{
    log_error(ec, "cmd: {}", cmd);
    log_error(ec, "cmd: {:s}", cmd);
    log_error(ec, "cmd: 0x{:i:02x}", cmd);
    log_error(ec, "cmd: 0x{:i:02X}", cmd);
    log_error(ec, "cmd: {0:s} (0x{0:i:02X})", cmd);
}
```
:::

*The code in the "Before" column is adapted from a real project written by the author of this paper. Block comments were removed for brevity.*

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

### Enumeration Membership Predicate

The utility predicate `std::is_enumerated_value`, proposed for addition to `<utility>`, complements the formatting and stringification features at the center of the proposal.
It allows users to detect unenumerated values directly.
This provides opportunities to cleanly enforce function preconditions or establish fallbacks.

The name `is_enumerated_value` is proposed instead of `is_unenumerated_value` in keeping with the standard library's preference for naming predicates in the positive sense.

While there is an obvious implementation that parallels the example of `enum_to_string` from [@P2996R13], that paper points out that implementations can exploit properties of the enumeration type known to the compiler to optimize performance.
Therefore, the specific algorithm for the function is not specified by this proposal.

### Formatting Specifiers and Semantics

Rather than complicating the *std-format-spec* [format.string] grammar with additional characters, this proposal defines its own formatting mini-language of which *std-format-spec* is a subset.
The specification grammar proposed for enumeration formatting consists of a representation specifier, a qualification specifier, and a delimiter followed by a *std-format-spec* valid for the selected representation.

#### Representation (Enumerator Identifier String vs Underlying Integral Value)

When the representation specifier is omitted, the *format-spec* portion of the replacement field must contain only an optional delimiter followed by a *std-format-spec* valid for the default representation.
When the representation specifier is present, it selects between the string formatter and the built-in formatter for the enumeration's underlying integral type.
Specifying the *type* representation selects the string formatter.

The *string* representation, specified by `'s'`, represents the value as the chosen identifier of its named enumerator.
See [String Representation of Unenumerated Values] for the case where the value does not correspond to any named enumerator and [Enumerator Aliases] for the case where it corresponds to multiple named enumerators.

The *integer* representation, specified by `'i'`, represents the value as its representation in the enumeration's underlying integral type.

The *type* representation, specified by `'t'`, represents the value as the identifier of the enumeration type itself.
This allows the user to separately format the type name and the value from the same positional argument (e.g., `std::format("{0:t:>9}::{0:i:#04x}", e);`).

#### Type-Qualification

When a representation other than *type* is specified, the optional qualification specifier selects whether to *type-qualify* the output.
This prepends the enumeration type's identifier and two colons in the form of the scope resolution operator (`::`) to the string or integer representation.
When the representation is *string*, the type qualification is prepended before forwarding to the string formatter.
Since the underlying integral type formatter does not format arbitrary characters, the *integer* representation prepends the type qualification to the result.

Type-qualified output is specified by `'q'`, and unqualified output is specified by `'u'`.

#### Format Specification Delimiter

This proposal uses `':'` (the format-specifier separator) to separate the enumeration-specific portion of the format specification from the portion for formatting the specified representation.
This avoids introducing a different character into the format grammar and naturally extends the replacement field to give the appearance of a third section.
This results in replacement fields that look like `"{:i:02X}"`, `"{:i}"`, `"{:i:}"`, `"{:}"`, or `"{::}"`.

#### Enumerator Aliases

Choosing the named enumerator whose identifier represents an enumerated value is an exercise in discerning programmer intent.
When multiple names alias the same value, this exercise becomes non-trivial.
This proposal divides the set of enumerators into tiers and selects the first name in declaration order from the highest priority tier.
The *canonical* tier holds the highest priority and consists of enumerators designated as in [Designating Canonical Names].
The *normal* tier follows and consists of enumerators not so designated.
(See [Reflecting on Attributes] for an as-yet-unproposed extension to disfavor `[[deprecated]]` enumerators.)

### Annotations

Annotations are vocabulary.
Introduced in [@P3393R4], they bring a new dimension in which code can communicate both with other code and with programmers.
Absent standard terminology, libraries will inevitably invent their own, and like with natural languages, speakers of one dialect will not understand the slang of other dialects.
Introducing sound annotations into the standard in an ergonomic way will have a large impact on compatibility, interoperability, and consistency as the feature becomes widely used.

This paper proposes the introduction of the `<annotations>` header to contain standard annotations.
This avoids needing to include `<format>` just to declare an enumeration, prevents the header proliferation that would ensue if every header had a corresponding `<X_annotations>`, and prefers the option that is easiest to teach.
"Where is the annotation for ...?" always has the same obvious answer.

Likewise, this paper defines its annotation types and objects in the proposed `std::annotations` inline namespace.
The syntax for annotations becomes unwieldy with deeply nested names, so facilitating forms like `[[=std::some_annotation]]` and `[[=some_annotation]]` enhances developer ergonomics.
The nested namespace, inspired by `std::literals`, allows users to access annotation names directly from `std` or use `std::annotations` to limit the names brought into their current scope.

While the annotations proposed here are designed to enable the proposed stringification and formatting facilities, they have been developed with broader use in mind.
The author therefore invites feedback on the exact spelling of the annotation names to ensure they serve the wider ecosystem.

#### Designating Canonical Names

When selecting the enumerator name for a given value, one must sometimes select among a set of aliases all corresponding to the same value.
At present, there is no mechanism to favor one over another, and there is no mechanism to recognize that one is `[[deprecated]]` in favor of another (see [Reflecting on Attributes]).
This proposal introduces the `std::annotations::canonical` annotation to allow the designation of canonical enumerators inside their declaration.
When the `canonical` annotation appertains to an enumerator, its name is favored over unannotated enumerators with the same value.

#### Format Opt-In Annotations

Modeled on the pattern used throughout the STL, the proposed formatting opt-in annotation types specialize the class template `std::annotations::basic_formattable_with_default`.
The `char` representation is thus exposed as the alias template `std::annotations::formattable_with_default`, and its default-constructed object is `std::annotations::formattable`.
This leaves the door open for `u8formattable_with_default`, `wformattable_with_default`, et cetera as the need arises to provide per-type formatting defaults in additional contexts.

The `basic_formattable_with_default` template takes both the representation and format specifier character types as independent template parameters for maximum flexibility.
This gives the committee slack as UTF-8 formatting support evolves because the formatting annotations can adapt to fit new proposals without substantial design changes.
Class template argument deduction allows `formattable_with_default` (and any future aliases selecting different representation character types) to deduce its format specifier character type from its constructor argument.

#### User-Defined Formatting Annotations

```cpp 
namespace application {
    inline constexpr std::formattable_with_default hex_dword{"i:#10X"};

    struct layout_table {
        static constexpr std::formattable_with_default heading{"s:^20.20"};
        //...elided for brevity
    };

    enum class color24 [[=hex_dword]] {
        red   = 0xFF0000,
        green = 0x00FF00,
        blue  = 0x0000FF,
    };

    enum class cake [[=layout_table::heading]] {
        vanilla,
        chocolate,
        strawberry,
        butterscotch,
        sarsaparilla,
        pumpkin,
    };
} //namespace application
```

Because the formatter recognizes annotations by their type rather than by the name of the annotation object, users may define their own annotation objects of type `std::formattable_with_default` to establish reusable formatting conventions.
These objects can be declared in user scopes and applied to enumeration types without requiring additional annotation types or special handling by the formatter.
Such annotations establish defaults, which may still be overridden by the format specification at the call site.

## Default Representation

The proposed formatter uses the string representation when unspecified by the caller and the annotation.
Users directly formatting an enumeration with `{}` rather than converting it to its underlying integral type typically want to display its symbolic name.
This is the primary reason for giving enumerations formatter support in the first place.

While it would be possible to infer intent and default to integer representation for enumeration types with no declared enumerators should the committee prefer it, that would complicate the interface and reduce usability in generic contexts. See [Uniform vs. Inferred Default Representation].

## Default Type-Qualification

This paper proposes a "smart default" type-qualification where the default depends on the specified representation and whether the value corresponds to a named enumerator.
As the name of an enumerator often carries sufficient context for a familiar user to identify its probable type, the default in this case is unqualified.
For values without a named enumerator, the default for the string representation treats the value as a type-qualified pseudo-enumerator because the numeric value alone carries no context.
These defaults apply to both the `std::formatter` and to `std::to_string`.
When the specified format representation is the underlying integral type, the default is an unqualified number, trusting the caller to provide appropriate context (or to specify a type-qualified format if desired).

## Non-Enumerable Types

The `std::to_string` overloads are constrained to require enumerable scoped enumerations, so calls will fail to compile.
The `std::formatter` specialization, on the other hand, rejects non-enumerable types during format-string parsing, at the moment the caller specifies representing the value as an enumerator name string.
Instantiation of the `std::formatter` or annotation of an enumeration is permitted because it is perfectly reasonable for a programmer to format an enumeration value as a number or format the enumeration type name without the type being enumerable.

## Constraints and Limitations

### Reflecting on Attributes

Should [@P3385R8] be adopted into the Working Draft, this proposal would use its facilities to favor non-deprecated enumerator names over deprecated names.
The tiers discussed in [Enumerator Aliases] would simply be extended with a *deprecated* tier below the *normal* tier consisting of enumerators to which the `[[deprecated]]` attribute appertains.

### Structural Types

Should [@P4414R0] be adopted into the Working Draft, this proposal would modify the proposed definition of `std::basic_formattable_with_default` to encapsulate a `std::basic_string_view` rather than storing public non-static data members.
These data members are currently *unspecified* with the intent that implementations could plausibly replace them without breaking the binary representation of the annotation objects if the standard were to permit it in the future.
The revised definition would also employ the `structural` class-property-specifier to allow real encapsulation.

### UTF-8 Support in `std::formatter`

This proposal does not extend `std::formatter` to support formatting UTF-8 strings or character types directly.
If support such as that proposed in [@P3258R0] were to be adopted, the mechanism proposed here could be extended to connect that facility with `std::meta::u8identifier_of` by means of a `std::u8formattable_with_default` type alias.

This paper also does not propose to extend `std::formatter` to support UTF-8 characters in format strings.
If support were to be adopted, the mechanism proposed here would automatically interoperate through its character type template parameters that accept `char8_t` arguments.

## Alternatives Considered

### Stringification Function Name

Extending the existing `std::to_string` overload set for enumerable scoped enumerations rather than introducing a distinct function name (e.g., `enum_to_string`) keeps stringification familiar and integrates directly with generic code expecting `std::to_string` to take a value argument and return `std::string`.

```cpp
template<typename T>
    requires requires (T t) { std::to_string(t); }
void log_invalid(T t) {
     std::cerr << "Invalid Value: " << std::to_string(t);
}

enum class color { red, blue };

void contrived_use(color hue) {
    if (hue != color::red)
        log_invalid(hue); //previously failed constraint satisfaction
    //...elided for brevity
}
```

However, introducing `std::to_string` overloads can introduce overload ambiguity in existing code that brings `std::to_string` into scope with a `using` declaration and performs unqualified calls:

```cpp
namespace ambiguity {
    enum class status { ok, err };
  
    template<typename E>
        requires std::is_scoped_enum_v<E>
    std::string to_string(E value) { /* elided for brevity */ }
}

void contrived_ambiguous_adl_use(ambiguity::status result_status) {
    using std::to_string;
    auto str = to_string(result_status); //previously unambiguous; ambiguous under this proposal
}

namespace safety {
    enum class status { ok, err };
  
    std::string to_string(status value) { /* elided for brevity */ }
}

void contrived_safe_adl_use(safety::status result_status) {
    using std::to_string;
    auto str = to_string(result_status); //still unambiguous
}
```

When `using std::to_string;` is in scope and `to_string` is called unqualified, argument-dependent lookup (ADL) considers `ambiguity::to_string` and `safety::to_string` alongside the overloads found in `std`.
Adding a viable `std::to_string` overload for scoped enumerations would make this previously unambiguous call ambiguous when both overloads are equally viable.
For an exact match on `safety::status`, both candidates have equally ranked conversion sequences, and overload resolution prefers the non-template function over a function template specialization.
For `ambiguity::status`, the user-defined `to_string` is itself an equally constrained template (i.e., a template of equal or lesser partial-ordering specificity); therefore, the viable candidates have equal conversion rank and neither wins tie-breaking, and the call is ambiguous.
Because the proposed `std::to_string` overloads rely on reflection metafunctions instead of type traits for their constraints, neither template is more constrained than the other in this example.
If `ambiguity::to_string` had used `std::meta::is_scoped_enum_type(^^E)` instead, the proposed `std::to_string` overload, which additionally requires `std::meta::is_enumerable_type(^^E)`, would be more constrained and thus called unambiguously.

The exact prevalence of such cases across existing C++ codebases is not empirically measured, as syntactic text search alone cannot reliably distinguish namespace-scoped `to_string` definitions, template vs. non-template declarations, and active `using std::to_string;` ADL call sites.
However, the practical impact is expected to be bounded: scoped enumerations do not implicitly convert to arithmetic types, `std::to_string` is not widely established as an ADL customization point for user enumerations, generic stringification was limited before the C++26, and affected call sites can be resolved by qualifying the call or removing the redundant local overload.
This paper considers the ergonomics and uniformity of supporting `std::to_string` for enumerable scoped enumerations to outweigh this potential compatibility cost.

### Unified Syntax vs. Separate Annotations

[@P4267R1] proposes `std::format_as_identifier` and `std::format_as_underlying` as separate annotations.
This requires enumeration type authors to select the representation of their enumeration types at the point of declaration and prevents users of those types from overriding that choice at their call sites.

This paper, instead, provides a unified format specification syntax to defer the choice of representation to the caller and a single format annotation class template which still allows the creator of the type to select a per-type default.

Should the committee prefer the syntax of discrete named annotation objects like `std::format_as_identifier` and `std::format_as_underlying`, they could be implemented under this proposal:
```cpp {.not_proposed}
inline constexpr formattable_with_default format_as_identifier{"s"};
inline constexpr formattable_with_default format_as_underlying{"i"};
```
The user would retain the ability to override the representation at the call site.

As a practical matter, should a developer wish to enforce their choice of formatting representation on users, the **only** way within the `std::format` framework is to explicitly specialize `std::formatter` for the type.
An annotation-based facility cannot enforce such a choice, as users remain free to explicitly specialize `std::formatter` for the type and format it as they wish.

### Annotation Class Template vs. Separate Annotation Types

The initial draft of the reference implementation had `formattable_with_default` and `formattable` annotations.
When considering how to be forward compatible with UTF-8 formatting, the obvious solution was `u8formattable_with_default` and `u8formattable`.
This works, but the definitions of the two classes are, aside from identifier names, identical.
In addition to duplicating the definitions, the `formatter` specialization had to replicate its annotation detection for both annotation types.
The straightforward solution was to make the representation character type a template parameter that distinguishes the representation families and unify the definitions into a single class template.
When exploring options for supporting non-`char`-based format specification strings, adding the format specification character type as a template parameter and connecting it with the character type of the `std::formatter` made that support relatively simple.

Thus, this paper relies on `std::annotations::basic_formattable_with_default<typename RepresentationCharT, typename FormatSpecifierCharT>` to build an extensible mechanism capable of supporting future `char8_t` formatting.
It further provides `std::annotations::formattable_with_default<typename CharT>` as a user-facing alias for `std::annotations::basic_formattable_with_default<char, CharT>` and holds space for `std::annotations::u8formattable_with_default<typename CharT>` as the corresponding alias for `std::annotations::basic_formattable_with_default<char8_t, CharT>`.
The annotation object `std::annotations::formattable` (like the as-yet-unproposed `std::annotations::u8formattable`) simplifies the case where no default format specification is provided.
Because the constructor of `std::annotations::basic_formattable_with_default` takes a reference to an array of `FormatSpecifierCharT`, CTAD correctly deduces `[[=formattable_with_default("i")]]` as `std::annotations::basic_formattable_with_default<char, char>` (assuming the relevant using directive/declaration).
This provides the best of both worlds: clean annotation syntax, a unified annotation family, and a clear path for extension.

### Uniform vs. Inferred Default Representation

An alternative design would infer the default representation from the presence or absence of declared enumerators, selecting integer representation when no enumerator is declared and string representation otherwise.

This inferred approach has practical appeal for enumerations used purely as strong integer aliases (such as `std::byte`).
Because every value in an enumeration with no declared enumerators is unenumerated, authors annotating such a type with `[[=std::formattable]]` may frequently want bare numeric output rather than type-qualified pseudo-enumerators (such as `byte::42`).

However, a uniform string default is preferable for several reasons.

A single rule, "`{}` always defaults to string representation", is trivial to teach and reason about.
With an inferred default, generic code cannot know an enumeration's default representation without inspecting its enumerators (for example, with a `has_named_enumerators` helper).
Furthermore, adding a sentinel enumerator to a previously empty enumeration would silently change the default representation from integer to string.
Worse, any existing call site relying on that inferred integer default with numeric format specifiers (e.g., `"{:#010b}"`) would suddenly fail to compile, because `'#'` and `'0'` are invalid in string format specifications.

Enumerations without enumerators are often introduced specifically to distinguish a domain type from a raw integer.
Producing a type-qualified pseudo-enumerator by default preserves that distinction, while `[[=std::formattable_with_default("i")]]` gives authors a concise, explicit way to opt into bare integer formatting when desired.

### Accessing the Annotated Default Format Specification

The syntax for accessing the default format specification stored by a `std::basic_formattable_with_default` object could take practically infinite forms.
Directly accessing the pointer stored by the constructor would work, but it would be preferable to avoid exposing a raw pointer or a public data member.
After considering many member function names such as `get`, `value`, `as_string`, `get_string_view`, `as_string_view`, `get_basic_string_view`, `as_basic_string_view`, a pattern appeared.
This operation requires converting the stored representation into a type-safe view of the characters stored by the annotation, which is idiomatically spelled `operator std::basic_string_view` for the stored character type.
Using the conversion operator provides a clean interface that does not require selecting a name.
Furthermore, should another type such as an explicit `std::string` copy be desired in the future, the door is open to extension.

### Default Format Specification Syntax

The default format specification string used to construct a `std::basic_formattable_with_default` object could represent either a valid *replacement field* or the *format specification proper* (see [format.string]).
Since it is the default rather than the caller's format string, it cannot reference positional arguments.
Requiring a valid *replacement field* could therefore be confusing for users.
Using the *format specification proper* simplifies the annotation construction, the feature specification, and the implementation.
This paper proposes the latter but could adapt to whichever form the committee prefers.

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
Since named enumerators are written `rgb_color::red`, `rgb_color::green`, and `rgb_color::blue`, this proposal uses a parallel syntax for unenumerated values and produces `"rgb_color::0"`, treating the underlying value as a pseudo-enumerator.
This preserves the enumeration type and underlying value for diagnostic output.
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
if (!std::is_enumerated_value(black)) {
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

_Describe the formatting behavior, including:

- Default representations.
- Format-specification syntax.
- Qualified and unqualified representations.
- Integer representations.
- Forwarded format specifications.
- Type-name representations.
_
## Enumerator Aliases

_Describe canonical enumerator selection and alias handling._

## Annotations

_Describe the annotation facilities and their semantics._

### `std::annotations::canonical`

### `std::annotations::basic_formattable_with_default`

### `std::annotations::formattable_with_default`

### `std::annotations::formattable`

### UTF8 Support

#### `std::annotations::u8formattable_with_default`

#### `std::annotations::u8formattable`

## Examples

_Provide representative examples demonstrating the proposed functionality._

# Implementation Experience

_Describe existing implementations, compiler support, implementation experience, and testing._

## Reference Implementation

_Describe the reference implementation._

## Implementation Status

_Describe compiler and library support._

## Testing

_Describe the testing strategy and relevant test coverage._

# Feature-Test Macro

_Specify the proposed feature-test macro and its value._

# Proposed Wording

_This section contains the proposed changes to the C++ working draft.

Use markup tags like `[added text]{.add}`, `[removed text]{.rm}`, and `[removed text](added text){.sub}`._

> Modify Section [general.description] as follows:
>
> The behavior of the function shall be modified to [support the new functionality]{.add} [throw an exception]{.rm}.

## Header Changes

_Insert proposed wording._

## Library Specification Changes

_Insert proposed wording._

## Feature-Test Macro Changes

_Insert proposed wording._

# Acknowledgements

_Acknowledge contributors and reviewers._

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
