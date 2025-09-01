// <copyright file="mmpld.h" company="Visualisierungsinstitut der Universität Stuttgart">
// Copyright © 2025 Visualisierungsinstitut der Universität Stuttgart.
// Licensed under the MIT licence. See LICENCE file for details.
// </copyright>
// <author>Christoph Müller</author>

#if !defined(_FPD_MMPLD_H)
#define _FPD_MMPLD_H
#pragma once

#if defined(FPD_WITH_MMPLD)

#include <mmpld.h>

#include "fpd/format.h"


FPD_NAMESPACE_BEGIN

/// <summary>
/// Converts an MMPLD colour type to the corresponding FPD format.
/// </summary>
/// <param name="colour">The MMPLD colour type to convert.</param>
/// <returns>The corresponding FPD format, or <see cref="format::unknown" />
/// if no corresponding format could be found.</returns>
inline format from_mmpld(_In_ const mmpld::colour_type colour) noexcept {
    switch (colour) {
        case mmpld::colour_type::rgb8: return format::uint8xyz;
        case mmpld::colour_type::rgba8: return format::uint8xyzw;
        case mmpld::colour_type::intensity: return format::float32;
        case mmpld::colour_type::rgb32: return format::float32xyz;
        case mmpld::colour_type::rgba32: return format::float32xyzw;
        case mmpld::colour_type::rgba16: return format::uint16xyzw;
        case mmpld::colour_type::intensity64: return format::float64;
        default: return format::unknown;
    }
}

/// <summary>
/// Converts an MMPLD vertex type to the corresponding FPD format.
/// </summary>
/// <param name="vertex">The MMPLD vertex type to convert.</param>
/// <returns>The corresponding FPD format, or <see cref="format::unknown" />
/// if no corresponding format could be found.</returns>
inline format from_mmpld(_In_ const mmpld::vertex_type vertex) noexcept {
    switch (vertex) {
        case mmpld::vertex_type::float_xyz: return format::float32xyz;
        case mmpld::vertex_type::float_xyzr: return format::float32xyzw;
        case mmpld::vertex_type::short_xyz: return format::int16xyz;
        case mmpld::vertex_type::double_xyz: return format::float64xyz;
        default: return format::unknown;
    }
}

FPD_NAMESPACE_END

#endif defined(FPD_WITH_MMPLD)

#endif /* !defined(_FPD_MMPLD_H) */
