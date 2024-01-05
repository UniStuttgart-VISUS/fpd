// <copyright file="format_description.h" company="Visualisierungsinstitut der Universität Stuttgart">
// Copyright © 2023 Visualisierungsinstitut der Universität Stuttgart.
// Licensed under the MIT licence. See LICENCE file for details.
// </copyright>
// <author>Christoph Müller</author>

#pragma once

#include <numeric>

#include "fpd/element_group.h"


FPD_NAMESPACE_BEGIN

/// <summary>
/// Defines an element that makes a particle <see cref="format_description" />.
/// </summary>
/// <typeparam name="TChar">The type of a character in a string.</typeparam>
template<class TChar> class format_description final {

public:

    /// <summary>
    /// The type of a character in a string.
    /// </summary>
    typedef TChar char_type;

    /// <summary>
    /// The type of a group of description elements.
    /// </summary>
    typedef element_group<TChar> group_type;

    /// <summary>
    /// The type of a description element.
    /// </summary>
    typedef typename group_type::element_type element_type;

    /// <summary>
    /// The type used to define the format of an element.
    /// </summary>
    typedef typename group_type::format_type format_type;

    /// <summary>
    /// The type of a compatible element initialiser that can be used to
    /// construct elements from an initialiser lsit.
    /// </summary>
    typedef typename group_type::initialiser_type initialiser_type;

    /// <summary>
    /// The type used to describe the semantic of an element.
    /// </summary>
    typedef typename group_type::semantic_type semantic_type;

    /// <summary>
    /// Initialsies a new instance.
    /// </summary>
    inline format_description(void) noexcept = default;

    /// <summary>
    /// Initialise a new instance.
    /// </summary>
    /// <param name="elements">The initialisers for the elements in the format
    /// description.</param>
    /// <param name="eval_offsets">If <c>true</c>, add explicit offsets to each
    /// of the the elements.</param>
    format_description(
        _In_ const std::initializer_list<initialiser_type>& elements,
        _In_ const bool eval_offsets = false);

    /// <summary>
    /// In-place constructs a new element at the end of the specified group.
    /// </summary>
    /// <param name="semantic">The semantic of the element to be added.</param>
    /// <param name="semantic_name">The human-readable name of the semantic to
    /// be added. This can be <c>nullptr</c> if the <paramref name="semantic" />
    /// is a built-in one.</param>
    /// <param name="group">The zero-based index of the group to add the element
    /// to. If this group does not yet exist, it will be created.</param>
    /// <param name="format">The format of the element.</param>
    /// <param name="offset">The offset of the element from the begin of the
    /// group. This can be zero if the element starts immediately after the
    /// previous one without any padding.</param>
    /// <param name="size">The size of the element. This can be zero if the
    /// <paramref name="format" /> is one with a known size.</param>
    /// <param name="eval_offset">If <c>true</c>, add an explicit offset to the
    /// element based on the current content of the group.</param>
    /// <returns><c>*this</c>.</returns>
    format_description& add(_In_ const semantic_type semantic,
        _In_opt_z_ const char_type *semantic_name,
        _In_ const format_type format,
        _In_ const std::size_t group = 0,
        _In_ const std::uint16_t offset = 0,
        _In_ const std::uint16_t size = 0,
        _In_ const bool eval_offset = false);

    /// <summary>
    /// In-place constructs a new element with built-in semantics at the end
    /// of the specified group.
    /// </summary>
    /// <param name="semantic">The semantic of the element to be added.</param>
    /// <param name="group">The zero-based index of the group to add the element
    /// to. If this group does not yet exist, it will be created.</param>
    /// <param name="format">The format of the element.</param>
    /// <param name="offset">The offset of the element from the begin of the
    /// group. This can be zero if the element starts immediately after the
    /// previous one without any padding.</param>
    /// <param name="size">The size of the element. This can be zero if the
    /// <paramref name="format" /> is one with a known size.</param>
    /// <param name="eval_offset">If <c>true</c>, add an explicit offset to the
    /// element based on the current content of the group.</param>
    /// <returns><c>*this</c>.</returns>
    inline format_description& add(_In_ const semantic_type semantic,
            _In_ const format_type format,
            _In_ const std::size_t group = 0,
            _In_ const std::uint16_t offset = 0,
            _In_ const std::uint16_t size = 0,
            _In_ const bool eval_offset = false) {
        return this->add(semantic, nullptr, format, group, offset, size,
            eval_offset);
    }

    /// <summary>
    /// Adds a new element configured by the given initialiser to the format.
    /// </summary>
    /// <param name="initialiser">The initialiser for the new element.</param>
    /// <param name="eval_offset">If <c>true</c>, add an explicit offset to the
    /// element based on the current content of the group.</param>
    /// <returns><c>*this</c>.</returns>
    format_description& add(_In_ const initialiser_type& initialiser,
        _In_ const bool eval_offset = false);

    /// <summary>
    /// Gets the number of groups (arrays of structs) for each particle.
    /// </summary>
    /// <returns>The number of groups of elements.</returns>
    inline std::size_t count(void) const noexcept {
        return this->_groups.size();
    }

    /// <summary>
    /// Answer the overall size of a particle in bytes.
    /// </summary>
    /// <remarks>
    /// Please note that particles may not be laid out contiguously in memory,
    /// but this overall size returned by the method may be split into
    /// multiple streams (struct of arrays). However, this size can be used to
    /// compute the number of particles in a frame from the offsets in the
    /// <see cref="meta_data::frame_table" />.
    /// </remarks>
    /// <returns>The overall bytes allocate per particle.</returns>
    inline std::size_t size(void) const noexcept {
        return std::accumulate(
            this->_groups.begin(),
            this->_groups.end(),
            static_cast<std::size_t>(0),
            [](const std::size_t s, const group_type& g) {
                return s + g.size();
            });
    }

    /// <summary>
    /// Provides access to the <paramref name="i" />th group of elements.
    /// </summary>
    /// <param name="i">The zero-based index of the group to retrieve.
    /// </param>
    /// <returns>The <paramref name="i" />th group.</returns>
    inline const group_type& operator [](_In_ const std::size_t i) const {
        return this->_groups[i];
    }

    /// <summary>
    /// Provides access to the <paramref name="i" />th group of elements.
    /// </summary>
    /// <param name="i">The zero-based index of the group to retrieve.
    /// </param>
    /// <returns>The <paramref name="i" />th group.</returns>
    inline const group_type& operator ()(_In_ const std::size_t i) const {
        return this->_groups[i];
    }

    /// <summary>
    /// Provides access to the <paramref name="element" />th element of the
    /// <paramref name="group" />th group of elements.
    /// </summary>
    /// <param name="group">The zero-based index of the group from which to
    /// retrieve the element.</param>
    /// <param name="element">THe zero-based index of the element within the
    /// group.</param>
    /// <returns>A reference to the specified element.</returns>
    inline const element_type& operator ()(_In_ const std::size_t group,
            _In_ const std::size_t element) const {
        return this->_groups[group][element];
    }

private:

    std::vector<group_type> _groups;

};

FPD_NAMESPACE_END

#include "fpd/format_description.inl"
