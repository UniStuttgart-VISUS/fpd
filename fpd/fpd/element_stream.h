// <copyright file="element_stream.h" company="Visualisierungsinstitut der Universität Stuttgart">
// Copyright © 2023 Visualisierungsinstitut der Universität Stuttgart.
// Licensed under the MIT licence. See LICENCE file for details.
// </copyright>
// <author>Christoph Müller</author>

#pragma once

#include <string>
#include <vector>

#include "fpd/description_element.h"


FPD_NAMESPACE_BEGIN

/// <summary>
/// An element stream is part of a <see cref="format_description" /> and holds
/// all <see cref="description_elements" /> that are consecutively stored in
/// the file (array of structures).
/// </summary>
/// <typeparam name="TChar">The type of a character in a string.</typeparam>
template<class TChar> class element_stream final {

public:

    /// <summary>
    /// The type of a character in a string.
    /// </summary>
    typedef TChar char_type;

    /// <summary>
    /// The type of a description element.
    /// </summary>
    typedef description_element<char_type> element_type;

    /// <summary>
    /// The type used to define the format of an element.
    /// </summary>
    typedef FPD_NAMESPACE::format format_type;

    /// <summary>
    /// The type used to describe the semantic of an element.
    /// </summary>
    typedef FPD_NAMESPACE::semantic semantic_type;

    /// <summary>
    /// Gets the number of consecutive elements in the stream that belong to
    /// a single particle.
    /// </summary>
    /// <returns>The number of elements in the stream.</returns>
    inline std::size_t size(void) const noexcept {
        return this->_elements.size();
    }

    /// <summary>
    /// Provides access to the <paramref name="i" />th element.
    /// </summary>
    /// <param name="i">The zero-based index of the element to retrieve.
    /// </param>
    /// <returns>The <paramref name="i" />th element.</returns>
    inline const element_type& operator [](_In_ const std::size_t i) const {
        return this->_elements[i];
    }

private:

    std::vector<element_type> _elements;
};

FPD_NAMESPACE_END

#include "fpd/element_stream.inl"
