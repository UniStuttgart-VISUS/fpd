// <copyright file="element.h" company="Visualisierungsinstitut der Universität Stuttgart">
// Copyright © 2023 Visualisierungsinstitut der Universität Stuttgart.
// Licensed under the MIT licence. See LICENCE file for details.
// </copyright>
// <author>Christoph Müller</author>

#pragma once

#include <stdexcept>
#include <string>

#include "fpd/element_initialiser.h"
#include "fpd/format_traits.h"
#include "fpd/null_coalesce.h"


FPD_NAMESPACE_BEGIN

/// <summary>
/// Defines an element that makes a particle <see cref="format_description" />.
/// </summary>
/// <typeparam name="TChar">The type of a character in a string.</typeparam>
template<class TChar> class element final {

public:

    /// <summary>
    /// The type of a compatible element initialiser that can be used to
    /// construct instances of this type from an initialiser lsit.
    /// </summary>
    typedef element_initialiser<TChar> initialiser_type;

    /// <summary>
    /// The type of a character in a string.
    /// </summary>
    typedef typename initialiser_type::char_type char_type;

    /// <summary>
    /// The type used to define the format of an element.
    /// </summary>
    typedef typename initialiser_type::format_type format_type;

    /// <summary>
    /// The type used to describe the semantic of an element.
    /// </summary>
    typedef typename initialiser_type::semantic_type semantic_type;

    /// <summary>
    /// The type of a string.
    /// </summary>
    typedef std::basic_string<char_type> string_type;

    /// <summary>
    /// Initialises a new instance.
    /// </summary>
    /// <param name=""></param>
    inline element(void) : _format(format_type::unknown),
        _offset(0), _semantic(semantic_type::unknown), _size(0) { }

    /// <summary>
    /// Initialises a new instance.
    /// </summary>
    /// <param name="semantic">The semantic of the element.</param>
    /// <param name="semantic_name">The human-readable description of
    /// the semantic. This can be <c>nullptr</c> if <pararmef name="semantic" />
    /// is a built-in semantic.</param>
    /// <param name="format">The format used to store the element.</param>
    /// <param name="offset">The offset of the element from the begin of
    /// the stream.</param>
    /// <param name="size">The size of the element in bytes.</param>
    /// <exception cref="std::invalid_argument">If an empty
    /// <paramref name="semantic_name" /> was provided for a user-defined
    /// semantic, or if <paramref name="format" /> is
    /// <see cref="format::unknown" /> and no non-zero <paramref name="size" />
    /// was specified.</exception>
    element(_In_ const semantic_type semantic,
        _In_opt_z_ const char_type *semantic_name,
        _In_ const format_type format,
        _In_ const std::uint16_t offset,
        _In_ const std::uint16_t size);

    /// <summary>
    /// Initialises a new instance for a built-in semantic.
    /// </summary>
    /// <param name="semantic">The semantic of the element, which cannot be a
    /// user-defined semantic.</param>
    /// <param name="format">The format used to store the element.</param>
    /// <param name="offset">The offset of the element from the begin of
    /// the stream.</param>
    /// <param name="size">The size of the element in bytes.</param>
    /// <exception cref="std::invalid_argument">If an empty
    /// <paramref name="semantic_name" /> was provided for a user-defined
    /// semantic, or if <paramref name="format" /> is
    /// <see cref="format::unknown" /> and no non-zero <paramref name="size" />
    /// was specified.</exception>
    element(_In_ const semantic_type semantic,
        _In_ const format_type format,
        _In_ const std::uint16_t offset,
        _In_ const std::uint16_t size);

    /// <summary>
    /// Constructs a new instance from the values in the given
    /// <see cref="initialiser_type" />.
    /// </summary>
    /// <remarks>
    /// <para>The constructor will copy all fields of the initialiser except for
    /// the group index, which is implicitly determined by the
    /// <see cref="format_description" /> that holds the element.</para>
    /// <para>This constructor allows for implicit conversion from initialsiers
    /// to <see cref="element" />s.</para>
    /// </remarks>
    /// <param name="initialiser">The initialiser to copy the fields from.
    /// </param>
    /// <param name="offset_override">If not zero, replace a zero-offset of the
    /// initialiser with this value.</param>
    element(_In_ const initialiser_type& initialiser,
        _In_ const std::uint16_t offset_override = 0);

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

    static constexpr std::uint16_t size_coalesce(
            _In_ const std::uint16_t size, _In_ const format_type format) {
        return (size > 0) ? size : static_cast<std::uint16_t>(to_size(format));
    }

    format_type _format;
    std::uint16_t _offset;
    semantic_type _semantic;
    string_type _semantic_name;
    std::uint16_t _size;
};

FPD_NAMESPACE_END

#include "fpd/element.inl"
