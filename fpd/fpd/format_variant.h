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
    template<class TValue> basic_format_variant(_In_ const TValue& value);

    /// <summary>
    /// Clears the content of the variant to
    /// <see cref="format_type::unknown" />.
    /// </summary>
    //void clear(void) noexcept;

    /// <summary>
    /// Answer a pointer to the data of the variant, or <c>nullptr</c> if the
    /// variant does not contain valid data.
    /// </summary>
    /// <returns>A pointer to the data if these are valid.</returns>
    inline _Ret_maybenull_ const void *data(void) const noexcept {
        return static_cast<bool>(*this) ? this->_data.data() : nullptr;
    }

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
    /// Answer the overall size of the variant in bytes.
    /// </summary>
    /// <returns>The size of the variant in bytes.</returns>
    inline constexpr std::size_t size(void) const noexcept {
        return this->_data.size();
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

    std::array<std::uint8_t, type_list_max_sizeof<format_types>::value> _data;
    format_type _format;
};


/// <summary>
/// A variant type that can hold all known <see cref="formats" />.
/// </summary>
typedef basic_format_variant<formats> format_variant;


FPD_NAMESPACE_END

#include "fpd/format_variant.inl"
