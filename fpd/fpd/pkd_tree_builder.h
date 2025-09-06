// <copyright file="pkd_tree_builder.h" company="Visualisierungsinstitut der Universität Stuttgart">
// Copyright © 2024 Visualisierungsinstitut der Universität Stuttgart.
// Licensed under the MIT licence. See LICENCE file for details.
// </copyright>
// <author>Christoph Müller</author>

#pragma once

#include <algorithm>
#include <cinttypes>
#include <cstdlib>
#include <limits>
#include <stdexcept>
#include <thread>
#include <utility>
#include <vector>

#include "fpd/bounding_box.h"
#include "fpd/format_description.h"
#include "fpd/make_unsigned.h"


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
    /// Configures the builder to work only serially.
    /// </summary>
    /// <returns>*<see langword="this" />.</returns>
    pkd_tree_builder& always_serially(void) noexcept {
        this->_parallel_threshold = (std::numeric_limits<std::size_t>::max)();
        return *this;
    }

    /// <summary>
    /// In-place builds the pkd-tree.
    /// </summary>
    /// <param name="particles">A pointer to <paramref name="cnt" /> particles
    /// of the configured format.</param>
    /// <param name="cnt">The number of particles in designated by
    /// <paramref name="particles" />.</param>
    /// <returns>*<see langword="this" />.</returns>
    /// <exception cref="std::invalid_argument">If <paramref name="particles" />
    /// is <c>nullptr</c> while <paramref name="cnt" /> is larger than zero.
    /// </param>
    pkd_tree_builder& build(_In_ void *particles, _In_ const std::size_t cnt);

    /// <summary>
    /// Specifies the format of the particles to be re-organised.
    /// </summary>
    /// <param name="desc">The format description</param>
    /// <returns>*<see langword="this" />.</returns>
    /// <exception cref="std::invalid_argument">If <parmaref name="desc" /> does
    /// not hold any <see cref="semantic::position" />.</exception>
    pkd_tree_builder& for_format(_In_ const format_description_type& desc);

    /// <summary>
    /// Configures the builder to make the left and the right subtree in
    /// parallel if the height of the tree exceed the specified
    /// <paramref name="depth" />.
    /// </summary>
    /// <param name="depth">The minimum depth required for parallel
    /// processing.</param>
    /// <returns>*<see langword="this" />.</returns>
    pkd_tree_builder& parallel_above(_In_ const std::size_t depth) noexcept {
        this->_parallel_threshold = depth;
        return *this;
    }

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
    /// <returns>*<see langword="this" />.</returns>
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
    /// <returns>*<see langword="this" />.</returns>
    inline pkd_tree_builder& with_splits_embedded_in(
            _In_ const std::size_t dimension) noexcept {
        this->_axis_dimension = dimension;
        return *this;
    }

private:

    /// <summary>
    /// Cache for all relevant information about a group of elements in the
    /// configured format. This are mainly the offset of the group (the 
    /// aggregated size of all preceeding groups) and the size of an element in
    /// the group. Futhermore, once the builder received actual particle data,
    /// we cache the start of the stream here to speed up subsequent pointer
    /// arithmetics.
    /// </summary>
    struct stream {
        std::size_t offset;
        std::uint8_t *start;
        std::size_t stride;

        inline stream(_In_ const std::size_t offset = 0)
            : offset(offset), start(nullptr), stride(0) { }

        /// <summary>
        /// Gets the begin of the <paramref name="particle" />th particle of the
        /// group described by this stream.
        /// </summary>
        template<class TPointer> inline TPointer *operator()(
                _In_ TPointer *particles,
                _In_ std::size_t cnt,
                _In_ const std::size_t particle = 0) const noexcept {
            static_assert(sizeof(TPointer) == 1, "The input pointer must be "
                "byte-sized in order for the pointer arithmetics to work.");
            assert(particles != nullptr);
            assert(this->stride > 0);
            auto retval = particles;
            retval += cnt * this->offset;
            retval += particle * this->stride;
            return retval;
        }

        /// <summary>
        /// Gets the begin of the <paramref name="particle" />th particle from
        /// <see cref="start" />.
        /// </summary>
        inline std::uint8_t *operator ()(
                _In_ std::size_t cnt,
                _In_ const std::size_t particle) const noexcept {
            assert(this->start != nullptr);
            return this->operator ()(this->start, cnt, particle);
        }
    };

    /// <summary>
    /// Builds an OSPray-style P-kd tree.
    /// </summary>
    /// <typeparam name="TScalar"></typeparam>
    /// <param name="node"></param>
    /// <param name="bounds"></param>
    /// <param name="depth"></param>
    template<class TScalar> void build_ospray(
        _In_ const std::size_t node,
        _In_ const bounding_box /* [sic] */ bounds,
        _In_ const std::size_t depth) const;

    /// <summary>
    /// Embeds the given <paramref name="axis" /> in the three least significant
    /// bits of the <see cref="_axis_dimension" />th component of
    /// <paramref name="position" />.
    /// </summary>
    template<class TScalar> void embed_axis(_In_ TScalar *position,
        _In_ const std::size_t axis) const noexcept;

    /// <summary>
    /// Answer the position of the <paramref name="i" />th particle.
    /// </summary>
    inline void *position(_In_ const std::size_t i) const noexcept {
        assert(this->_group < this->_streams.size());
        assert(this->_streams[this->_group].start != nullptr);
        assert(i < this->_particles);
        // Get the start of the stream holding the position.
        auto retval = this->_streams[this->_group](this->_particles, i);
        // Offset to the location of the position within the stream.
        retval += this->_offset;
        return retval;
    }

    /// <summary>
    /// Answer the position of the <paramref name="i" />th particle.
    /// </summary>
    template<class TScalar>
    inline TScalar *position(_In_ const std::size_t i) const noexcept {
        auto retval = this->position(i);
        return static_cast<TScalar *>(retval);
    }

    /// <summary>
    /// Asnwer the <paramref name="c" />th component of the position of the
    /// <paramref name="i" />th particle.
    /// </summary>
    template<class TScalar> inline TScalar position(
            _In_ const std::size_t i,
            _In_ const std::size_t c) const noexcept {
        assert(c < this->_components);
        return this->position<TScalar>(i)[c];
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

    /// <summary>
    /// Swaps the particles at the specifiied indices.
    /// </summary>
    void swap_particles(_In_ const std::size_t lhs,
        _In_ const std::size_t rhs) const noexcept;

    std::size_t _axis_component;
    std::size_t _components;
    bounding_box _bbox;
    std::size_t _dimensions;
    format _format;
    std::size_t _group;
    std::size_t _levels;
    std::size_t _offset;
    std::size_t _parallel_threshold;
    std::size_t _particles;
    std::size_t _scalar;
    std::vector<stream> _streams;
};

FPD_NAMESPACE_END

#include "fpd/pkd_tree_builder.inl"
