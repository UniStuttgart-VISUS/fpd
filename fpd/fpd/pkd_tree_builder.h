// <copyright file="pkd_tree_builder.h" company="Visualisierungsinstitut der Universität Stuttgart">
// Copyright © 2024 Visualisierungsinstitut der Universität Stuttgart.
// Licensed under the MIT licence. See LICENCE file for details.
// </copyright>
// <author>Christoph Müller</author>

#pragma once

#include <cstdlib>
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

private:

    typedef std::pair<std::size_t, std::size_t> particle_range;

    /// <summary>
    /// Advances the given position pointer by <paramref name="cnt" />
    /// particles.
    /// </summary>
    static inline void *advance(_In_ std::uint8_t *positions,
            _In_ const std::size_t cnt = 1) noexcept {
        return position + (cnt * this->_stride.second);
    }

    /// <summary>
    /// Answer a pointer to the first position in the given particle list.
    /// </summary>
    static inline void *positions(_In_ std::uint8_t *particles,
            _In_ const std::size_t cnt) noexcept {
        return particles + (cnt * this->_stride.first + this->_stride.second);
    }

    bounding_box _bbox;
    std::size_t _dimensions;
    format _format;
    std::size_t _group;
    std::mutex _lock;
    std::size_t _offset;
    std::queue<particle_range> _queue;
    std::pair<std::size_t, std::size_t> _stride;
};

FPD_NAMESPACE_END

#include "fpd/pkd_tree_builder.inl"
