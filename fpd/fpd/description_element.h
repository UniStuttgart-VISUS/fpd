// <copyright file="description_element.h" company="Visualisierungsinstitut der Universität Stuttgart">
// Copyright © 2023 Visualisierungsinstitut der Universität Stuttgart.
// Licensed under the MIT licence. See LICENCE file for details.
// </copyright>
// <author>Christoph Müller</author>

#pragma once

#include <string>

#include "fpd/format_traits.h"
#include "fpd/semantic.h"


FPD_NAMESPACE_BEGIN

/// <summary>
/// Defines an element that makes a particle <see cref="format_description" />.
/// </summary>
/// <typeparam name="TChar">The type of a character in a string.</typeparam>
template<class TChar> class description_element final {

public:

    /// <summary>
    /// The type of a character in a string.
    /// </summary>
    typedef TChar char_type;

    /// <summary>
    /// The type used to define the format of an element.
    /// </summary>
    typedef FPD_NAMESPACE::format format_type;

    /// <summary>
    /// The type used to describe the semantic of an element.
    /// </summary>
    typedef FPD_NAMESPACE::semantic semantic_type;

    /// <summary>
    /// The type of a string.
    /// </summary>
    typedef std::basic_string<char_type> string_type;

    /// <summary>
    /// Gets the format of the element described by this object.
    /// </summary>
    /// <returns>The format of the element.</returns>
    inline format_type format(void) const noexcept {
        return this->_format;
    }

    /// <summary>
    /// Gets the offset of the element from the begin of the particle.
    /// </summary>
    /// <remarks>
    /// <para>Offsets are always relative to a stream, i.e. if a particle is
    /// split into multiple streams, the data starts at zero in each stream.
    /// </para>
    /// </remarks>
    /// <returns>The offset of the element in bytes.</returns>
    inline std::uint16_t offset(void) const noexcept {
        return this->_offset;
    }

    /// <summary>
    /// Gets the semantic of the element.
    /// </summary>
    /// <returns>The semantic of the element.</returns>
    inline semantic_type semantic(void) const noexcept {
        return this->_semantic;
    }

    /// <summary>
    /// Gets a textual description of the semantic.
    /// </summary>
    /// <returns>The stringised built-in semantic or the name of custom semantic
    /// as defined in the file.</returns>
    inline const string_type& semantic_name(void) const noexcept {
        return this->_semantic_name;
    }

    /// <summary>
    /// Gets the size of the element in bytes.
    /// </summary>
    /// <returns>The size of the element in bytes.</returns>
    inline std::uint16_t size(void) const noexcept {
        return this->_size;
    }

private:

    format_type _format;
    std::uint16_t _offset;
    semantic_type _semantic;
    string_type _semantic_name;
    std::uint16_t _size;
};

FPD_NAMESPACE_END
