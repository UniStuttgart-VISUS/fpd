// <copyright file="element_initialiser.h" company="Visualisierungsinstitut der Universität Stuttgart">
// Copyright © 2023 - 2025 Visualisierungsinstitut der Universität Stuttgart.
// Licensed under the MIT licence. See LICENCE file for details.
// </copyright>
// <author>Christoph Müller</author>

#if !defined(_FPD_ELEMENT_INITIALISER_H)
#define _FPD_ELEMENT_INITIALISER_H
#pragma once

#include "fpd/format.h"
#include "fpd/semantic.h"


FPD_NAMESPACE_BEGIN

/// <summary>
/// Defines an element that makes a particle <see cref="format_description" />.
/// </summary>
/// <typeparam name="TChar">The type of a character in a string.</typeparam>
template<class TChar> class element_initialiser final {

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
    /// Initialises a new instance.
    /// </summary>
    /// <param name="semantic">The semantic of the element.</param>
    /// <param name="semantic_name">The human-readable description of
    /// the semantic. The caller remains owner of this string and must ensure
    /// that it remains valid until the initialiser has been assigned to a
    /// <see cref="element" /> that copies the data.</param>
    /// <param name="format">The format used to store the element.</param>
    /// <param name="group">The zero-based index of the group to add the element
    /// to. If this group does not yet exist, it will be created.</param>
    /// <param name="offset">The offset of the element from the begin of
    /// the stream.</param>
    /// <param name="size">The size of the element in bytes.</param>
    /// <exception cref="std::invalid_argument">If an empty
    /// <paramref name="semantic_name" /> was provided for a user-defined
    /// semantic, or if <paramref name="format" /> is
    /// <see cref="format::unknown" /> and no non-zero <paramref name="size" />
    /// was specified.</exception>
    element_initialiser(_In_ const semantic_type semantic,
            _In_opt_z_ const char_type *semantic_name,
            _In_ const format_type format,
            _In_ const std::size_t group = 0,
            _In_ const std::uint16_t offset = 0,
            _In_ const std::uint16_t size = 0)
        : _format(format),
            _group(group),
            _offset(offset),
            _semantic(semantic),
            _semantic_name(semantic_name),
            _size(size) { }

    /// <summary>
    /// Initialises a new instance for a built-in semantic.
    /// </summary>
    /// <param name="semantic">The semantic of the element, which cannot be a
    /// user-defined semantic.</param>
    /// <param name="format">The format used to store the element.</param>
    /// <param name="group">The zero-based index of the group to add the element
    /// to. If this group does not yet exist, it will be created.</param>
    /// <param name="offset">The offset of the element from the begin of
    /// the stream.</param>
    /// <param name="size">The size of the element in bytes.</param>
    /// <exception cref="std::invalid_argument">If an empty
    /// <paramref name="semantic_name" /> was provided for a user-defined
    /// semantic, or if <paramref name="format" /> is
    /// <see cref="format::unknown" /> and no non-zero <paramref name="size" />
    /// was specified.</exception>
    element_initialiser(_In_ const semantic_type semantic,
            _In_ const format_type format,
            _In_ const std::size_t group = 0,
            _In_ const std::uint16_t offset = 0,
            _In_ const std::uint16_t size = 0)
        : _format(format),
            _group(group),
            _offset(offset),
            _semantic(semantic),
            _semantic_name(nullptr),
            _size(size) { }

private:

    format_type _format;
    std::size_t _group;
    std::uint16_t _offset;
    semantic_type _semantic;
    const char_type *_semantic_name;
    std::uint16_t _size;

    template<class> friend class element;
    template<class> friend class format_description;
    template<class> friend class meta_data_builder;
};

FPD_NAMESPACE_END

#endif /* !defined(_FPD_ELEMENT_INITIALISER_H) */
