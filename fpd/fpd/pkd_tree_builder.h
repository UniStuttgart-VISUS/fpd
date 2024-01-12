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

    /// <summary>
    /// Generic implementation of comparison operators for template-based
    /// dispatching.
    /// </summary>
    /// <typeparam name="TValue">The type of the values to be compared.
    /// </typeparam>
    template<class TValue> struct compare_as final {

        static inline constexpr bool eq(_In_ const void *lhs,
                _In_ const void *rhs) noexcept {
            assert(lhs != nullptr);
            assert(rhs != nullptr);
            auto l = reinterpret_cast<const TValue *>(lhs);
            auto r = reinterpret_cast<const TValue *>(rhs);
            return (*l == *r);
        }

        static inline constexpr bool ge(_In_ const void *lhs,
                _In_ const void *rhs) noexcept {
            assert(lhs != nullptr);
            assert(rhs != nullptr);
            auto l = reinterpret_cast<const TValue *>(lhs);
            auto r = reinterpret_cast<const TValue *>(rhs);
            return (*l >= *r);
        }

        static inline constexpr bool gt(_In_ const void *lhs,
                _In_ const void *rhs) noexcept {
            assert(lhs != nullptr);
            assert(rhs != nullptr);
            auto l = reinterpret_cast<const TValue *>(lhs);
            auto r = reinterpret_cast<const TValue *>(rhs);
            return (*l > *r);
        }

        static inline constexpr bool le(_In_ const void *lhs,
                _In_ const void *rhs) noexcept {
            assert(lhs != nullptr);
            assert(rhs != nullptr);
            auto l = reinterpret_cast<const TValue *>(lhs);
            auto r = reinterpret_cast<const TValue *>(rhs);
            return (*l <= *r);
        }

        static inline constexpr bool lt(_In_ const void *lhs,
                _In_ const void *rhs) noexcept {
            assert(lhs != nullptr);
            assert(rhs != nullptr);
            auto l = reinterpret_cast<const TValue *>(lhs);
            auto r = reinterpret_cast<const TValue *>(rhs);
            return (*l < *r);
        }

        static inline constexpr bool ne(_In_ const void *lhs,
                _In_ const void *rhs) noexcept {
            assert(lhs != nullptr);
            assert(rhs != nullptr);
            auto l = reinterpret_cast<const TValue *>(lhs);
            auto r = reinterpret_cast<const TValue *>(rhs);
            return (*l != *r);
        }

        compare_as(void) = delete;
    };

    typedef std::pair<std::size_t, std::size_t> particle_range;

    /// <summary>
    /// Advances the given position pointer by <paramref name="cnt" />
    /// particles.
    /// </summary>
    inline void *advance(_In_ std::uint8_t *positions,
            _In_ const std::size_t cnt = 1) noexcept {
        assert(position != nullptr);
        assert(this->_stride.second > 0);
        return position + (cnt * this->_stride.second);
    }

    /// <summary>
    /// Advances the given pointer to the <paramref name="dimension" />th
    /// component of the vector.
    /// </summary>
    inline void *dimension(_In_ std::uint8_t *position,
            _In_ const std::size_t dimension) noexcept {
        assert(position != nullptr);
        assert(dimension < this->_dimensions);
        assert(this->_scalar > 0);
        return position + dimension * this->_scalar;
    }

    /// <summary>
    /// Answer a pointer to the first position in the given particle list.
    /// </summary>
    inline void *positions(_In_ std::uint8_t *particles,
            _In_ const std::size_t cnt) noexcept {
        assert(particles != nullptr);
        assert(this->_stride.first > 0);
        assert(this->_stride.second > 0);
        return particles + (cnt * this->_stride.first + this->_stride.second);
    }

    /// <summary>
    /// Sets the <see cref="compare_as" /> operations if
    /// <paramref name="format" /> if <typeparamref name="Format" />.
    /// </summary>
    template<format Format, format... Formats>
    void resolve_compare_as(_In_ format_list<Format, Formats...>,
        _In_ const format format);

    /// <summary>
    /// Recursion stop for <see cref="resolve_compare_as" />.
    /// </summary>
    void resolve_compare_as(_In_ format_list<>, _In_ const format format);

    bounding_box _bbox;
    std::size_t _dimensions;
    bool (*_eq)(_In_ const void *lhs, _In_ const void *rhs);
    format _format;
    bool (*_ge)(_In_ const void *lhs, _In_ const void *rhs);
    std::size_t _group;
    bool (*_gt)(_In_ const void *lhs, _In_ const void *rhs);
    bool (*_le)(_In_ const void *lhs, _In_ const void *rhs);
    std::mutex _lock;
    bool (*_lt)(_In_ const void *lhs, _In_ const void *rhs);
    bool (*_ne)(_In_ const void *lhs, _In_ const void *rhs);
    std::size_t _offset;
    std::queue<particle_range> _queue;
    std::size_t _scalar;
    std::pair<std::size_t, std::size_t> _stride;
};

FPD_NAMESPACE_END

#include "fpd/pkd_tree_builder.inl"
