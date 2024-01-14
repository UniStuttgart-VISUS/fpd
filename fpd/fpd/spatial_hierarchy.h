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
    /// The data are organised in a P-kd tree as it used by OSPRay.
    /// </summary>
    /// <remarks>
    /// OSPRay stores the P-kd tree as a sequence of particles such that the
    /// left subtree is located at 2 * current index + 1 and the right subtree
    /// is located at 2 * current index + 2. The root node of the tree is at
    /// the begin of the particle stream. The subdivision axes may be chosen
    /// round-robin or based on the extents that the left and right subtree
    /// would form. In the latter case, the axis is encoded in the two least
    /// significant bits of the configured coordinate.
    /// <remarks>
    ospray_pkd_tree = 0x00000001,

    fpd_pkd_tree = 0x00000002,

    /// <summary>
    /// The data are organised in an axis-aligned octree.
    /// </summary>
    octree = 0x00000008
};

FPD_NAMESPACE_END
