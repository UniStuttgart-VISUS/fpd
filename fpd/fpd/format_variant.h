// <copyright file="format_variant.h" company="Visualisierungsinstitut der Universität Stuttgart">
// Copyright © 2023 - 2024 Visualisierungsinstitut der Universität Stuttgart.
// Licensed under the MIT licence. See LICENCE file for details.
// </copyright>
// <author>Christoph Müller</author>

#pragma once

#include <array>
#include <cinttypes>
#include <stdexcept>

#include "fpd/format_traits.h"
#include "fpd/invoke.h"
#include "fpd/type_list.h"


FPD_NAMESPACE_BEGIN

/// <summary>
/// A variant type that can hold variables matching all of the given
/// <typeparamref name="TFormatList" />.
/// </summary>
/// <typeparam name="TFormatList">An instantiation of <see name="format_list" />
/// specifying all formats that can be stored in the variant.</typeparam>
template<class Formats> class basic_format_variant;


namespace detail {

    template<class TValue> struct format_value_ctor final {
        inline void operator ()(TValue& value) {
            value = TValue();
        }
    };

    template<class TValue> struct format_value_dtor final {
        inline void operator ()(TValue& value) noexcept {
            value.~TValue();
        }
    };

} /* namespace detail */


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
        : _format(format_type::unknown) { }

    /// <summary>
    /// Initialises a new instance.
    /// </summary>
    /// <typeparam name="TValue">The type of the value stored in the variant.
    /// This must be one of the <see cref="format_types" />.</typeparam>
    /// <param name="value">The initial value in the variant.</param>
    template<class TValue> basic_format_variant(_In_ TValue&& value);

    /// <summary>
    /// Clears the content of the variant to
    /// <see cref="format_type::unknown" />.
    /// </summary>
    //void clear(void) noexcept;

    /// <summary>
    /// Answer the format of the data currently stored in the variant.
    /// </summary>
    /// <returns>The format of the data in the variant.</returns>
    inline format_type format(void) const noexcept {
        return this->_format;
    }

    ///// <summary>
    ///// Invoke the given functor template for the type that is actually
    ///// stored in the variant.
    ///// </summary>
    ///// <typeparam name="TFunction">The functor template, which must be
    ///// supported for all the types that can be stored in the variant.
    ///// </typeparam>
    ///// <param name="func">The functor to invoke.</param>
    ///// <returns></returns>
    //template<template<class> class TFunction>
    //const basic_format_variant& invoke(_In_ TFunction&& func) const;

    /// <summary>
    /// Gets the value of the variant, provided it has the specified format.
    /// </summary>
    /// <returns>The value of the variant.</returns>
    /// <exception cref="std::logic_error">If <typeparamref name="Format" /> is
    /// not the format of the current value in the variant.</exception>
    template<format_type Format>
    inline const typename format_forward_traits<Format>::type& get(void) const {
        return this->get<typename format_forward_traits<Format>::type>();
    }

    /// <summary>
    /// Gets the value of the variant, provided it has the specified format.
    /// </summary>
    /// <returns>The value of the variant.</returns>
    /// <exception cref="std::logic_error">If <typeparamref name="Format" /> is
    /// not the format of the current value in the variant.</exception>
    template<format_type Format>
    inline typename format_forward_traits<Format>::type& get(void) {
        return this->get<typename format_forward_traits<Format>::type>();
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
    inline constexpr bool supports(
            _In_ const format_type format) const noexcept {
        return in_format_list(formats(), format);
    }

    /// <summary>
    /// Answer whether the specified format is supported by the variant.
    /// </summary>
    /// <typeparam name="Format">The format to be tested.</typeparam>
    /// <returns><c>true</c> if values of <typeparamref name="Format" /> can
    /// be stored in the variant, <c>false</c> otherwise.</returns>
    template<format_type Format>
    inline constexpr bool supports(void) const noexcept {
        return in_format_list(formats(), Format);
    }

    /// <summary>
    /// Answer whether the specified type is supported by the variant.
    /// </summary>
    /// <typeparam name="TValue">The type to be tested.</typeparam>
    /// <returns><c>true</c> if values of <typeparamref name="TValue" /> can
    /// be stored in the variant, <c>false</c> otherwise.</returns>
    template<class TValue>
    inline constexpr bool supports(void) const noexcept {
        return in_format_list(formats(), format_reverse_traits<TValue>::format);
    }

    /// <summary>
    /// Gets the value of the variant, provided it has the specified format.
    /// </summary>
    /// <returns>The value of the variant, or <c>nullptr</c> if the value does
    /// not have the requested format.</returns>
    template<format_type Format>
    inline const typename format_forward_traits<Format>::type *get(
            void) const noexcept {
        return this->try_get<typename format_forward_traits<Format>::type>();
    }

    /// <summary>
    /// Gets the value of the variant, provided it has the specified format.
    /// </summary>
    /// <returns>The value of the variant, or <c>nullptr</c> if the value does
    /// not have the requested format.</returns>
    template<format_type Format>
    inline typename format_forward_traits<Format>::type *get(void) noexcept {
        return this->try_get<typename format_forward_traits<Format>::type>();
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
        return (format_reverse_traits<TValue>::format == this->_format)
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
        return (format_reverse_traits<TValue>::format == this->_format)
            ? reinterpret_cast<TValue *>(retval)
            : nullptr;
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

    static constexpr inline bool in_format_list(_In_ const format_list<>,
            _In_ const format_type) noexcept {
        return false;
    }

    template<format_type Format, format_type... Formats>
    static constexpr inline bool in_format_list(
            _In_ const format_list<Format, Formats...>,
            _In_ const format_type f) noexcept {
        return ((Format == f) || in_format_list(format_list<Formats...>(), f));
    }

    //template<class TValue, class TFunc>
    //inline auto invoke_for(_In_ TFunc &&func) {
    //    return invoke(std::forward<TFunc>(func))
    //}



    std::array<std::uint8_t, type_list_max_sizeof<format_types>::value> _data;
    format_type _format;
};


/// <summary>
/// A variant type that can hold all known <see cref="formats" />.
/// </summary>
typedef basic_format_variant<formats> format_variant;


FPD_NAMESPACE_END

#include "fpd/format_variant.inl"
