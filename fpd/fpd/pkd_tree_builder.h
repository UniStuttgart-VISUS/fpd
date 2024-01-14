// <copyright file="pkd_tree_builder.h" company="Visualisierungsinstitut der Universität Stuttgart">
// Copyright © 2024 Visualisierungsinstitut der Universität Stuttgart.
// Licensed under the MIT licence. See LICENCE file for details.
// </copyright>
// <author>Christoph Müller</author>

#pragma once

#include <cstdlib>
#include <limits>
#include <mutex>
#include <queue>
#include <stdexcept>
#include <thread>
#include <utility>

#include "fpd/bounding_box.h"
#include "fpd/format_description.h"


FPD_NAMESPACE_BEGIN

/// <summary>
/// Utility class for reorganising particle data in a P-kd tree.
/// </summary>
/// <typeparam name="TChar">The character type used in the format description
/// that points us to the location of the positions.</typeparam>
template<class TChar> class pkd_tree_builder final {

public:

    /// <summary>
    /// The type of the format description, which allows for identifying the
    /// location of the positions.
    /// </summary>
    typedef format_description<TChar> format_description_type;

    /// <summary>
    /// Initialises a new instance.
    /// </summary>
    pkd_tree_builder(void) noexcept;

    /// <summary>
    /// In-place builds the pkd-tree.
    /// </summary>
    /// <param name="particles">A pointer to <paramref name="cnt" /> particles
    /// of the configured format.</param>
    /// <param name="cnt">The number of particles in designated by
    /// <paramref name="particles" />.</param>
    /// <returns><c>*this</c>.</returns>
    /// <exception cref="std::invalid_argument">If <paramref name="particles" />
    /// is <c>nullptr</c> while <paramref name="cnt" /> is larger than zero.
    /// </param>
    pkd_tree_builder& build(_In_ void *particles, _In_ const std::size_t cnt);

    /// <summary>
    /// Specifies the format of the particles to be re-organised.
    /// </summary>
    /// <param name="desc">The format description</param>
    /// <returns><c>*this</c>.</returns>
    /// <exception cref="std::invalid_argument">If <parmaref name="desc" /> does
    /// not hold any <see cref="semantic::position" />.</exception>
    pkd_tree_builder& for_format(_In_ const format_description_type& desc);

    /// <summary>
    /// Sets the overall bounding box of the particles.
    /// </summary>
    /// <remarks>
    /// If a valid bounding box is set, the axes for splitting the data will be
    /// computed based on the maximum space to be subdivided. Otherwise, i.e. if
    /// an invalid bounding box is set, the axes axes will be chosen round-robin
    /// starting with the x-axis.
    /// </remarks>
    /// <param name="bbox">The bounding box of the particles.</param>
    /// <returns><c>*this</c>.</returns>
    inline pkd_tree_builder& within_bounding_box(
            _In_ const bounding_box& bbox) {
        this->_bbox = bbox;
    }

    /// <summary>
    /// Instructs the builder to split round robin starting at the first (x)
    /// axis. The particle data will remain unchanged in this case.
    /// </summary>
    /// <returns></returns>
    inline pkd_tree_builder& with_round_robin_splits(void) noexcept {
        this->_axis_dimension = (std::numeric_limits<std::size_t>::max)();
        return *this;
    }

    /// <summary>
    /// Instructs the builder to choose the split axes based on the largest
    /// bounding box to be formed for the subtrees and to embed this axis in the
    /// <paramref name="dimension" />th dimension of the position by changing
    /// the two least significant bits.
    /// </summary>
    /// <remarks>
    /// A valid bounding box for all particles in the frame must be provided if
    /// this option is chosen. The build will fail if this is not the case.
    /// </remarks>
    /// <param name="dimension">The dimension to embed the split axis in. If
    /// this is outside the range of the dimensions the positions have (possibly
    /// including the radius), the builder will create round-robin splits.</param>
    /// <returns><c>*this</c>.</returns>
    inline pkd_tree_builder& with_splits_embedded_in(
            _In_ const std::size_t dimension) noexcept {
        this->_axis_dimension = dimension;
        return *this;
    }

private:

    typedef std::pair<std::size_t, std::size_t> particle_range;

    /// <summary>
    /// Builds an OSPray style P-kd tree.
    /// </summary>
    /// <typeparam name="TScalar"></typeparam>
    /// <param name="particles"></param>
    /// <param name="indices"></param>
    /// <param name="node"></param>
    /// <param name="bounds"></param>
    /// <param name="depth"></param>
    template<class TScalar> void build_ospray(_In_ void *particles,
        _In_ std::vector<std::size_t>& indices,
        _In_ const std::size_t node,
        _In_ const bounding_box bounds,
        _In_ const std::size_t depth) const;

    /// <summary>
    /// Gets the <paramref name="c" />th component of the
    /// <paramref name="i" />th position indexed by <paramref name="indices" />.
    /// </summary>
    template<class TScalar> inline TScalar& component(_In_ void *positions,
            _In_ std::size_t i,
            _In_ const std::vector<std::size_t>& indices,
            _In_ std::size_t c) {
        assert(i < indices.size());
        assert(c < this->_components);
        auto position = this->position(position, i, indices);
        auto components = static_cast<TScalar *>(position);
        return components[c];
    }

    /// <summary>
    /// Embeds the given <paramref name="axis" /> in the least significant bits
    /// of the <see cref="_axis_dimension" />th component of
    /// <paramref name="position" />.
    /// </summary>
    template<class TScalar> void embed_axis(_In_ TScalar *position,
        _In_ const std::size_t axis) const noexcept;

    /// <summary>
    /// Answer the <paramref name="i" />th position pointer.
    /// </summary>
    inline void *position(_In_ std::uint8_t *positions,
            _In_ const std::size_t i) noexcept {
        assert(positions != nullptr);
        assert(this->_stride.second > 0);
        return positions + (i * this->_stride.second);
    }

    /// <summary>
    /// Answer the position of the <paramref name="i" />th index in
    /// <paramref name="indices" />.
    /// </summary>
    inline void *position(_In_ std::uint8_t *positions,
            _In_ const std::size_t i,
            _In_ const std::vector<std::size_t>& indices) noexcept {
        assert(i < indices.size());
        return this->position(position, indices[i]);
    }

    /// <summary>
    /// Answer a pointer to the first position in the given particle list.
    /// </summary>
    inline void *positions(_In_ std::uint8_t *particles,
            _In_ const std::size_t cnt) noexcept {
        assert(particles != nullptr);
        assert(this->_stride.first > 0);
        return particles + (cnt * this->_stride.first + this->_stride.second);
    }

    ///// <summary>
    ///// Sets the <see cref="compare_as" /> operations if
    ///// <paramref name="format" /> if <typeparamref name="Format" />.
    ///// </summary>
    //template<format Format, format... Formats>
    //void resolve_compare_as(_In_ format_list<Format, Formats...>,
    //    _In_ const format format);

    ///// <summary>
    ///// Recursion stop for <see cref="resolve_compare_as" />.
    ///// </summary>
    //void resolve_compare_as(_In_ format_list<>, _In_ const format format);

    std::size_t _axis_component;
    std::size_t _components;
    bounding_box _bbox;
    std::size_t _dimensions;
    format _format;
    std::size_t _group;
    std::mutex _lock;
    std::size_t _offset;
    std::queue<particle_range> _queue;
    std::size_t _scalar;
    std::pair<std::size_t, std::size_t> _stride;
};

FPD_NAMESPACE_END

#include "fpd/pkd_tree_builder.inl"
