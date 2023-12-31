// <copyright file="format_variant.h" company="Visualisierungsinstitut der Universität Stuttgart">
// Copyright © 2023 Visualisierungsinstitut der Universität Stuttgart.
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
    inline format_variant(void) noexcept : _format(format_type::unknown) { }

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

    /// <summary>
    /// Answer the overall size of the variant in bytes.
    /// </summary>
    /// <returns>The size of the variant in bytes.</returns>
    inline constexpr std::size_t size(void) const noexcept {
        return this->_data.size();
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
