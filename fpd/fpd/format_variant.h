// <copyright file="format_variant.h" company="Visualisierungsinstitut der Universität Stuttgart">
// Copyright © 2023 - 2025 Visualisierungsinstitut der Universität Stuttgart.
// Licensed under the MIT licence. See LICENCE file for details.
// </copyright>
// <author>Christoph Müller</author>

#if !defined(_FPD_FORMAT_VARIANT_H)
#define _FPD_FORMAT_VARIANT_H
#pragma once

#include <array>
#include <cinttypes>
#include <memory>
#include <stdexcept>
#include <type_traits>

#include "fpd/format_traits.h"
#include "fpd/invoke.h"
#include "fpd/type_list.h"


FPD_DETAIL_NAMESPACE_BEGIN

template<class TValue> struct format_value_ctor final {

    template<class TArg>
    typename std::enable_if<std::is_convertible<TValue, TArg>::value>::type
    operator ()(_In_ TValue *target, _In_ TArg&& value) {
        assert(target != nullptr);
        new (target) TValue(static_cast<TValue>(value));
    }

    template<class TArg>
    typename std::enable_if<!std::is_convertible<TValue, TArg>::value>::type
    operator ()(_In_ TValue *target, _In_ TArg&&) {
        assert(target != nullptr);
        assert(false);  // This should never be called by invoke.
        new (target) TValue();
    }
};

template<class TValue> struct format_value_runtime_ctor final {
    inline void operator ()(_In_ TValue *target,
            _In_reads_(cnt) const void *value,
            _In_ std::size_t cnt) {
        assert(target != nullptr);
        assert(cnt >= sizeof(TValue));
        new (target) TValue(*reinterpret_cast<const TValue *>(value));
    }
};

template<class TValue> struct format_value_dtor final {
    inline void operator ()(TValue *target) noexcept {
        assert(target != nullptr);
        target->~TValue();
    }
};

FPD_DETAIL_NAMESPACE_END


FPD_NAMESPACE_BEGIN

/// <summary>
/// A variant type that can hold variables matching all of the given
/// <typeparamref name="Formats" />.
/// </summary>
/// <typeparam name="Formats">An instantiation of <see name="format_list" />
/// specifying all formats that can be stored in the variant.</typeparam>
template<class Formats> class basic_format_variant;

/// <summary>
/// A variant type that can hold variables matching all of the given
/// <typeparamref name="Formats" />.
/// </summary>
/// <typeparam name="Formats">The list of formats that can be stored in the
/// variant.</typeparam>
template<format... Formats>
class basic_format_variant<format_list<Formats...>> final {

public:

    /// <summary>
    /// The list of formats supported by the variant.
    /// </summary>
    /// <remarks>
    /// This type hides the global format list, which is on purpose.
    /// </remarks>
    typedef format_list<Formats...> formats;

    /// <summary>
    /// The type used to represent the format with.
    /// </summary>
    typedef FPD_NAMESPACE::format format_type;

    /// <summary>
    /// The list of types supported by the variant.
    /// </summary>
    /// <remarks>
    /// This type hides the global type list, which is on purpose.
    /// </remarks>
    typedef typename detail::format_type_list<formats>::type format_types;

    /// <summary>
    /// Initialises a new instance.
    /// </summary>
    inline basic_format_variant(void) noexcept
        : _data({ 0 }), _format(format_type::unknown) { }

    /// <summary>
    /// Initialises a new instance.
    /// </summary>
    /// <typeparam name="TValue">The type of the value stored in the variant.
    /// This must be one of the <see cref="format_types" />.</typeparam>
    /// <param name="value">The initial value in the variant.</param>
    template<class TValue>
    inline basic_format_variant(_In_ TValue&& value,
            _In_opt_ typename std::enable_if<
            is_supported_type<formats, TValue>::value>::type * = nullptr)
            : _format(detail::format_reverse_traits<TValue>::format) {
        assert(this->supports(this->_format));
        this->invoke<detail::format_value_ctor>(std::forward<TValue>(value));
    }

    /// <summary>
    /// Initialises a new instance.
    /// </summary>
    /// <param name="format">The format of the data designated by
    /// <paramref name="value" />.</param>
    /// <param name="value">A pointer to at least <paramref name="cnt" /> valid
    /// bytes that can be interpreted as the initial value.</param>
    /// <param name="cnt">The size of the data provided via
    /// <paramref name="value" />, in bytes.</param>
    /// <exception cref="std::invalid_argument">If the variant does not support
    /// values of type <typeparamref name="TValue" /> or if the provided data
    /// are invalid.</exception>
    basic_format_variant(_In_ const format_type format,
        _In_reads_(cnt) const void *value,
        _In_ const std::size_t cnt);

    /// <summary>
    /// Clone <paramref name="rhs" />.
    /// </summary>
    /// <param name="rhs">The object to be cloned.</param>
    basic_format_variant(_In_ const basic_format_variant& rhs);

    /// <summary>
    /// Initialise from another variant.
    /// </summary>
    /// <typeparam name="F>The formats supported by the right-hand side
    /// operand. These formats and the ones of the left-hand side operands do
    /// not need to match, but only the currently active member of the right
    /// side must be supported by the left side, too.</typeparam>
    /// <param name="rhs">The object to be cloned. The value contained in this
    /// parameter must be supported by the target variant. It is safe to clone
    /// an empty variant.</param>
    /// <exception cref="std::invalid_argument">If <paramref name="rhs" /> holds
    /// a value that is not supported by the new type of variant.</exception>
    template<FPD_NAMESPACE::format... F> basic_format_variant(
        _In_ const basic_format_variant<format_list<F...>>& rhs);

    /// <summary>
    /// Move <paramref name="rhs" />.
    /// </summary>
    /// <param name="rhs">The object to be moved.</param>
    basic_format_variant(_Inout_ basic_format_variant&& rhs);

    /// <summary>
    /// Finalises the instance.
    /// </summary>
    ~basic_format_variant(void) noexcept;

    /// <summary>
    /// Clears the content of the variant to
    /// <see cref="format_type::unknown" />.
    /// </summary>
    void clear(void) noexcept;

    /// <summary>
    /// Answer the format of the data currently stored in the variant.
    /// </summary>
    /// <returns>The format of the data in the variant.</returns>
    inline format_type format(void) const noexcept {
        return this->_format;
    }

    /// <summary>
    /// Gets the value of the variant, provided it has the specified format.
    /// </summary>
    /// <returns>The value of the variant.</returns>
    /// <exception cref="std::logic_error">If <typeparamref name="Format" /> is
    /// not the format of the current value in the variant.</exception>
    template<format_type Format>
    inline const typename detail::format_forward_traits<Format>::type& get(
            void) const {
        typedef typename detail::format_forward_traits<Format>::type value_type;
        return this->get<value_type>();
    }

    /// <summary>
    /// Gets the value of the variant, provided it has the specified format.
    /// </summary>
    /// <returns>The value of the variant.</returns>
    /// <exception cref="std::logic_error">If <typeparamref name="Format" /> is
    /// not the format of the current value in the variant.</exception>
    template<format_type Format>
    inline typename detail::format_forward_traits<Format>::type& get(void) {
        typedef typename detail::format_forward_traits<Format>::type value_type;
        return this->get<value_type>();
    }

    /// <summary>
    /// Gets the value of the variant, provided it has the specified type.
    /// </summary>
    /// <typeparam name="TValue">The type of the value to retrieve, which must
    /// be the type of the format currently stored in the variant.</typeparam>
    /// <returns>The value of the variant.</returns>
    /// <exception cref="std::logic_error">If <typeparamref name="TValue" /> is
    /// not the type of the current value in the variant.</exception>
    template<class TValue> const TValue& get(void) const;

    /// <summary>
    /// Gets the value of the variant, provided it has the specified type.
    /// </summary>
    /// <typeparam name="TValue">The type of the value to retrieve, which must
    /// be the type of the format currently stored in the variant.</typeparam>
    /// <returns>The value of the variant.</returns>
    /// <exception cref="std::logic_error">If <typeparamref name="TValue" /> is
    /// not the type of the current value in the variant.</exception>
    template<class TValue> TValue& get(void);

    /// <summary>
    /// Answer the overall size of the variant in bytes.
    /// </summary>
    /// <returns>The size of the variant in bytes.</returns>
    inline constexpr std::size_t size(void) const noexcept {
        return this->_data.size();
    }

    /// <summary>
    /// Answer whether <paramref name="format" /> is a supported format in
    /// the variant.
    /// </summary>
    /// <param name="format">The format to be tested.</param>
    /// <returns><c>true</c> if <paramref name="format" /> is among the formats
    /// supported by the variant, <c>false</c> otherwise.</returns>
    inline bool supports(_In_ const format_type format) const noexcept {
        auto s = to_array(formats());
        return (std::find(s.begin(), s.end(), format) != s.end());
    }

    /// <summary>
    /// Answer whether the specified format is supported by the variant.
    /// </summary>
    /// <typeparam name="Format">The format to be tested.</typeparam>
    /// <returns><c>true</c> if values of <typeparamref name="Format" /> can
    /// be stored in the variant, <c>false</c> otherwise.</returns>
    template<format_type Format>
    inline constexpr bool supports(void) const noexcept {
        return is_supported_format<formats, Format>::value;
    }

    /// <summary>
    /// Answer whether the specified type is supported by the variant.
    /// </summary>
    /// <typeparam name="TValue">The type to be tested.</typeparam>
    /// <returns><c>true</c> if values of <typeparamref name="TValue" /> can
    /// be stored in the variant, <c>false</c> otherwise.</returns>
    template<class TValue>
    inline constexpr bool supports(void) const noexcept {
        return is_supported_type<formats, TValue>::value;
    }

    /// <summary>
    /// Gets the value of the variant, provided it has the specified format.
    /// </summary>
    /// <returns>The value of the variant, or <c>nullptr</c> if the value does
    /// not have the requested format.</returns>
    template<format_type Format>
    inline const typename detail::format_forward_traits<Format>::type *try_get(
            void) const noexcept {
        typedef typename detail::format_forward_traits<Format>::type value_type;
        return this->try_get<value_type>();
    }

    /// <summary>
    /// Gets the value of the variant, provided it has the specified format.
    /// </summary>
    /// <returns>The value of the variant, or <c>nullptr</c> if the value does
    /// not have the requested format.</returns>
    template<format_type Format>
    inline typename detail::format_forward_traits<Format>::type *try_get(
            void) noexcept {
        typedef typename detail::format_forward_traits<Format>::type value_type;
        return this->try_get<value_type>();
    }

    /// <summary>
    /// Gets the value of the variant, provided it has the specified type.
    /// </summary>
    /// <typeparam name="TValue">The type of the value to retrieve, which must
    /// be the type of the format currently stored in the variant.</typeparam>
    /// <returns>The value of the variant, or <c>nullptr</c> if the value does
    /// not have the requested type.</returns>
    template<class TValue> inline const TValue *try_get(void) const noexcept {
        auto retval = static_cast<bool>(*this) ? this->_data.data() : nullptr;
        return (detail::format_reverse_traits<TValue>::format == this->_format)
            ? reinterpret_cast<const TValue *>(retval)
            : nullptr;
    }

    /// <summary>
    /// Gets the value of the variant, provided it has the specified type.
    /// </summary>
    /// <typeparam name="TValue">The type of the value to retrieve, which must
    /// be the type of the format currently stored in the variant.</typeparam>
    /// <returns>The value of the variant, or <c>nullptr</c> if the value does
    /// not have the requested type.</returns>
    template<class TValue> inline TValue *try_get(void) noexcept {
        auto retval = static_cast<bool>(*this) ? this->_data.data() : nullptr;
        return (detail::format_reverse_traits<TValue>::format == this->_format)
            ? reinterpret_cast<TValue *>(retval)
            : nullptr;
    }

    /// <summary>
    /// Assignment.
    /// </summary>
    /// <param name="rhs">The right-hand side operand.</param>
    /// <returns><c>*this</c>.</returns>
    basic_format_variant& operator =(_In_ const basic_format_variant& rhs);

    /// <summary>
    /// Assignment.
    /// </summary>
    /// <typeparam name="F>The formats supported by the right-hand side
    /// operand. These formats and the ones of the left-hand side operands do
    /// not need to match, but only the currently active member of the right
    /// side must be supported by the left side, too.</typeparam>
    /// <param name="rhs">The right-hand side operand.</param>
    /// <returns><c>*this</c>.</returns>
    template<FPD_NAMESPACE::format... F> basic_format_variant& operator =(
        _In_ const basic_format_variant<format_list<F...>>& rhs);

    /// <summary>
    /// Move assignment.
    /// </summary>
    /// <param name="rhs">The right-hand side operand.</param>
    /// <returns><c>*this</c>.</returns>
    basic_format_variant& operator =(_In_ basic_format_variant&& rhs) noexcept;

    /// <summary>
    /// Value assignment.
    /// </summary>
    /// <typeparam name="TValue">The type of the value being assigned. This must
    /// be a type supported by the variant.</typeparam>
    /// <param name="value">The value to be assigned.</param>
    /// <returns><c>*this</c>.</returns>
    template<class TValue>
    inline typename std::enable_if<is_supported_type<formats, TValue>::value,
        basic_format_variant>::type&
    operator =(_In_ TValue&& value) {
        this->invoke<detail::format_value_dtor>();
        this->_format = detail::format_reverse_traits<TValue>::format;
        assert(this->supports(this->_format));
        this->invoke<detail::format_value_ctor>(std::forward<TValue>(value));
        return *this;
    }

    /// <summary>
    /// Answer whether the variant holds valid data.
    /// </summary>
    /// <returns><c>true</c> if the variant holds valid data,
    /// <c>false</c> otherwise.</returns>
    inline operator bool(void) const noexcept {
        return ((this->_format != format_type::unknown)
            && (this->_data.size() > 0));
    }

private:

    template<template<class> class TFunc, class... TArgs>
    inline void invoke(TArgs&&... args) {
        this->invoke0<TFunc>(formats(), std::forward<TArgs>(args)...);
    }

    template<template<class> class TFunc, class... TArgs>
    inline void invoke(TArgs&&... args) const {
        // Rationale: We do not want to implement the whole stack twice, so
        // we use the const_cast here as we know that our own functors do
        // not do nasty things.
        auto that = const_cast<basic_format_variant *>(this);
        that->invoke0<TFunc>(FPD_NAMESPACE::format_list(),
            std::forward<TArgs>(args)...);
    }

    template<template<class> class TFunc,
        FPD_NAMESPACE::format Fmt,
        FPD_NAMESPACE::format... Fmts,
        class... TArgs>
    void invoke0(_In_ format_list<Fmt, Fmts...>, _In_ TArgs&&... args);

    template<template<class> class TFunc, class... TArgs>
    inline void invoke0(_In_ const format_list<>, TArgs&&...) { }

    std::array<std::uint8_t, type_list_max_sizeof<format_types>::value> _data;
    format_type _format;

    template<class> friend class basic_format_variant;
};


/// <summary>
/// A variant type that can hold all known <see cref="formats" />.
/// </summary>
typedef basic_format_variant<formats> format_variant;

FPD_NAMESPACE_END

#include "fpd/format_variant.inl"

#endif /* !defined(_FPD_FORMAT_VARIANT_H) */
