// <copyright file="element_group.h" company="Visualisierungsinstitut der Universität Stuttgart">
// Copyright © 2023 - 2024 Visualisierungsinstitut der Universität Stuttgart.
// Licensed under the MIT licence. See LICENCE file for details.
// </copyright>
// <author>Christoph Müller</author>

#pragma once

#include <algorithm>
#include <cassert>
#include <iterator>
#include <string>
#include <vector>

#include "fpd/element.h"


FPD_NAMESPACE_BEGIN

/// <summary>
/// A element group is part of a <see cref="format_description" /> and holds
/// all <see cref="element" />s that are consecutively stored in
/// the file (array of structures).
/// </summary>
/// <typeparam name="TChar">The type of a character in a string.</typeparam>
template<class TChar> class element_group final {

public:

    /// <summary>
    /// The type of a character in a string.
    /// </summary>
    typedef TChar char_type;

    /// <summary>
    /// The type of a description element.
    /// </summary>
    typedef element<char_type> element_type;

    /// <summary>
    /// The type used to define the format of an element.
    /// </summary>
    typedef typename element_type::format_type format_type;

    /// <summary>
    /// The type of a compatible element initialiser that can be used to
    /// construct elements from an initialiser lsit.
    /// </summary>
    typedef typename element_type::initialiser_type initialiser_type;

    /// <summary>
    /// The type used to describe the semantic of an element.
    /// </summary>
    typedef typename element_type::semantic_type semantic_type;

    /// <summary>
    /// In-place constructs a new element at the end of the group.
    /// </summary>
    /// <param name="semantic">The semantic of the element to be added.</param>
    /// <param name="semantic_name">The human-readable name of the semantic to
    /// be added. This can be <c>nullptr</c> if the <paramref name="semantic" />
    /// is a built-in one.</param>
    /// <param name="format">The format of the element.</param>
    /// <param name="offset">The offset of the element from the begin of the
    /// group. This can be zero if the element starts immediately after the
    /// previous one without any padding.</param>
    /// <param name="size">The size of the element. This can be zero if the
    /// <paramref name="format" /> is one with a known size.</param>
    /// <param name="eval_offset">If <c>true</c>, add an explicit offset to the
    /// element based on the current content of the group.</param>
    inline void add(_In_ const semantic_type semantic,
            _In_opt_z_ const char_type *semantic_name,
            _In_ const format_type format,
            _In_ const std::uint16_t offset = 0,
            _In_ const std::uint16_t size = 0,
            _In_ const bool eval_offset = false) {
        this->_elements.emplace_back(semantic,
            semantic_name,
            format,
            ((offset > 0) || !eval_offset) ? offset: this->size(),
            size);
        assert(this->_elements.back().size() > 0);
    }

    /// <summary>
    /// In-place constructs a new element with built-in semantic at the end of
    /// the group.
    /// </summary>
    /// <param name="semantic">The semantic of the element to be added.</param>
    /// <param name="format">The format of the element.</param>
    /// <param name="offset">The offset of the element from the begin of the
    /// group. This can be zero if the element starts immediately after the
    /// previous one without any padding.</param>
    /// <param name="size">The size of the element. This can be zero if the
    /// <paramref name="format" /> is one with a known size.</param>
    /// <param name="eval_offset">If <c>true</c>, add an explicit offset to the
    /// element based on the current content of the group.</param>
    inline void add(_In_ const semantic_type semantic,
            _In_ const format_type format,
            _In_ const std::uint16_t offset = 0,
            _In_ const std::uint16_t size = 0,
            _In_ const bool eval_offset = false) {
        this->add(semantic, nullptr, offset, size, eval_offset);
    }

    /// <summary>
    /// Adds a new element with data from the given initialiser at the end of
    /// the group.
    /// </summary>
    /// <param name="initialiser">The initialiser to provide the data.</param>
    /// <param name="eval_offset">If <c>true</c>, add an explicit offset to the
    /// element based on the current content of the group.</param>
    void add(_In_ const initialiser_type& initialiser,
        _In_ const bool eval_offset = false);

    /// <summary>
    /// Moves the given element to the end of the group.
    /// </summary>
    /// <param name="element">The element to be added.</param>
    inline void add(_Inout_ element_type&& element) {
        assert(element.size() > 0);
        this->_elements.push_back(std::move(element));
    }

    /// <summary>
    /// Gets an iterator for the first element.
    /// </summary>
    /// <returns>An iterator for the first element.</returns>
    inline typename std::vector<element_type>::const_iterator begin(void) const {
        return this->_elements.begin();
    }

    /// <summary>
    /// Gets the number of consecutive elements in the stream that belong to
    /// a single particle.
    /// </summary>
    /// <returns>The number of elements in the stream.</returns>
    inline std::size_t count(void) const noexcept {
        return this->_elements.size();
    }

    /// <summary>
    /// Gets an iterator past the last element.
    /// </summary>
    /// <returns>An iterator past the last element.</returns>
    inline typename std::vector<element_type>::const_iterator end(void) const {
        return this->_elements.end();
    }

    /// <summary>
    /// First the first occurrence of the given semantic in the group.
    /// </summary>
    /// <param name="semantic">The semantic to search.</param>
    /// <returns>An iterator for the respective semantic or <see cref="end" />
    /// if the semantic does not exist in the group.</returns>
    inline typename std::vector<element_type>::const_iterator find(
            _In_ const semantic_type semantic) const {
        return std::find_if(this->_elements.begin(),
            this->_elements.end(),
            [semantic](const element_type& e) {
                return e.has_semantic(semantic);
            });
    }

    /// <summary>
    /// Answer the offset of the <paramref name="i" />th element in the group.
    /// </summary>
    /// <param name="i">The zero-based index of the element.</param>
    /// <returns>The offset of the element in bytes. Note that invalid indices
    /// will always yield <see cref="size" />, but the operation will not fail.
    /// </returns>
    std::uint16_t offset(_In_ const std::size_t i) const noexcept;

    /// <summary>
    /// Answer the offset of the given element in the group.
    /// </summary>
    /// <param name="e">The element to compute the offset of.</param>
    /// <returns>The fofset of the element in bytes.</returns>
    /// <exception cref="std::invalid_argument">If the <paramref name="e" /> was
    /// not found in the group.</exception>
    std::uint16_t offset(_In_ const element_type& e) const;

    /// <summary>
    /// Removes the <paramref name="i" />th element from the group.
    /// </summary>
    /// <param name="i">The index of the element to be removed.</param>
    inline void remove(_In_ const std::size_t i) {
        this->_elements.erase(std::advance(this->_elements.begin(), i));
    }

    /// <summary>
    /// Computes the current size of the element group.
    /// </summary>
    /// <returns>The current size of the group in bytes.</returns>
    inline std::uint16_t size(void) const noexcept {
        return this->offset(this->_elements.size());
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

    /// <summary>
    /// Answer whether the group contains at least one element.
    /// </summary>
    /// <returns><c>true</c> if the group is not empty, <c>false</c> otherwise.
    /// </returns>
    inline operator bool(void) const noexcept {
        return !this->_elements.empty();
    }

private:

    std::vector<element_type> _elements;
};

FPD_NAMESPACE_END

#include "fpd/element_group.inl"
