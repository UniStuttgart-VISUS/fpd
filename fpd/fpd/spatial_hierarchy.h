// <copyright file="spatial_hierarchy.h" company="Visualisierungsinstitut der Universität Stuttgart">
// Copyright © 2024 - 2025 Visualisierungsinstitut der Universität Stuttgart.
// Licensed under the MIT licence. See LICENCE file for details.
// </copyright>
// <author>Christoph Müller</author>

#if !defined(_FPD_SPATIAL_HIERARCHY_H)
#define _FPD_SPATIAL_HIERARCHY_H
#pragma once

#include <cinttypes>

#include "fpd/enum_dispatch_list.h"


FPD_NAMESPACE_BEGIN

/// <summary>
/// Possible kinds of spatial hierarchies that can be present in the data.
/// </summary>
/// <remarks>
/// <para>The raw particles are always stored without additional data attached.
/// If a hierarchy requires additional meta data to interpret these data
/// (locations of the boundaries of the hierarchy, for instance), these are
/// stored in the header along with the type of the hierarchy.</para>
/// <para>Adding a new kind of hierarchy involves several steps: add it to the
/// enumeration with a unique value; add it to the dispatch list
/// <see cref="spatial_hierarchies" /> below the enum; add a specialisation of
/// the <see cref="hierarchy_meta_data" /> template; add support for measuring,
/// reading and writing the specialisation in <see cref="meta_data" />.</para>
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

    ///// <summary>
    ///// The data are organised in an axis-aligned octree.
    ///// </summary>
    //octree = 0x00000008
};

FPD_NAMESPACE_END


FPD_DETAIL_NAMESPACE_BEGIN

/// <summary>
/// A specialised <see cref="enum_dispatch_list" /> for types of
/// <see cref="spatial_hierarchy" />.
/// </summary>
/// <typeparam name="Hierarchies">The possible types of spatial hierarchy that
/// can be dispatched.</typeparam>
template<spatial_hierarchy... Hierarchies>
using spatial_hierarchy_dispatch_list = enum_dispatch_list<spatial_hierarchy,
    Hierarchies...>;


/// <summary>
/// A dispatching list for <see spatial_hierarchy /> values.
/// </summary>
typedef spatial_hierarchy_dispatch_list<
    spatial_hierarchy::none,
    spatial_hierarchy::ospray_pkd_tree> spatial_hierarchies;

FPD_DETAIL_NAMESPACE_END

#endif /* !defined(_FPD_SPATIAL_HIERARCHY_H) */
