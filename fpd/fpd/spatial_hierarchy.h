// <copyright file="spatial_hierarchy.h" company="Visualisierungsinstitut der Universität Stuttgart">
// Copyright © 2024 Visualisierungsinstitut der Universität Stuttgart.
// Licensed under the MIT licence. See LICENCE file for details.
// </copyright>
// <author>Christoph Müller</author>

#pragma once

#include <cinttypes>

#include "fpd/abi.h"


FPD_NAMESPACE_BEGIN

/// <summary>
/// Possible kinds of spatial hierarchies that can be present in the data.
/// </summary>
/// <remarks>
/// The raw particles are always stored without additional data attached. If a
/// hierarchy requires additional meta data to interpret these data (locations
/// of the boundaries of the hierarchy, for instance), these are stored in the
/// header along with the type of the hierarchy.
/// </remarks>
enum class spatial_hierarchy : std::uint32_t {

    /// <summary>
    /// The data are unorganised.
    /// </summary>
    none = 0x00000000,

    /// <summary>
    /// The data are organised in a P-kd tree.
    /// </summary>
    pkd_tree = 0x00000001,

    /// <summary>
    /// The data are organised in an axis-aligned octree.
    /// </summary>
    octree = 0x00000002
};

FPD_NAMESPACE_END
