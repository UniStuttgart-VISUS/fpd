// <copyright file="hierarchy_meta_data.h" company="Visualisierungsinstitut der Universität Stuttgart">
// Copyright © 2025 Visualisierungsinstitut der Universität Stuttgart.
// Licensed under the MIT licence. See LICENCE file for details.
// </copyright>
// <author>Christoph Müller</author>

#if !defined(_FPD_HIERARCHY_META_DATA_H)
#define _FPD_HIERARCHY_META_DATA_H
#pragma once

#include <cinttypes>
#include <limits>

#include "fpd/spatial_hierarchy.h"


FPD_NAMESPACE_BEGIN

/// <summary>
/// The interface for meta data about the spatial hierarchy.
/// </summary>
class hierarchy_meta_data_base {

public:

    /// <summary>
    /// Finalises the instance.
    /// </summary>
    virtual ~hierarchy_meta_data_base(void) = default;

    /// <summary>
    /// Answer the method that was used to establish the hierarchy.
    /// </summary>
    /// <returns>The type of the hierarchy.</returns>
    virtual spatial_hierarchy type(void) const noexcept = 0;

protected:

    /// <summary>
    /// Initialises a new instance.
    /// </summary>
    hierarchy_meta_data_base(void) = default;
};


/// <summary>
/// Implements the <see cref="hierarchy_meta_data_base" /> interface. This
/// default implementation only carries the information about the type of
/// the hierarchy. If additional information is required, this class needs
/// tp be specifialised for the respective <see cref="spatial_hierarchy" />.
/// </summary>
/// <typeparam name="Hierarchy">The type of the spatial hierarchy.</typeparam>
template<spatial_hierarchy Hierarchy>
class hierarchy_meta_data final : public hierarchy_meta_data_base {

public:

    /// <inheritdoc />
    spatial_hierarchy type(void) const noexcept override {
        return Hierarchy;
    }
};


/// <summary>
/// Specialisation for OSPRay-style P-kd trees.
/// </summary>
template<> class hierarchy_meta_data<spatial_hierarchy::ospray_pkd_tree> final
        : public hierarchy_meta_data_base {

public:

    /// <summary>
    /// Initialsies a new instance.
    /// </summary>
    inline hierarchy_meta_data(void) noexcept
        : _axis_component((std::numeric_limits<std::uint8_t>::max)()) { }

    /// <summary>
    /// Initialises a new instance.
    /// </summary>
    /// <param name="axis_component">The zero-based index of the component
    /// holding the split axis.</param>
    explicit inline hierarchy_meta_data(
            _In_ const std::uint8_t axis_component) noexcept
        : _axis_component(axis_component) { }

    /// <summary>
    /// Answer the axis where the splits are embedded. If this number is larger
    /// than the number of components of the position, the splits are performed
    /// round-robin starting at the x-axis.
    /// </summary>
    /// <returns>The index of the component holding the split axis.</returns>
    inline std::uint8_t axis_component(void) const noexcept {
        return _axis_component;
    }

    /// <inheritdoc />
    spatial_hierarchy type(void) const noexcept override {
        return spatial_hierarchy::ospray_pkd_tree;
    }

private:

    std::uint8_t _axis_component;

    template<class> friend class FPD_NAMESPACE::meta_data;
};


FPD_NAMESPACE_END

#endif /* !defined(_FPD_HIERARCHY_META_DATA_H) */
