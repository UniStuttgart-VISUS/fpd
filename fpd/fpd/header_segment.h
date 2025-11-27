// <copyright file="header_segment.h" company="Visualisierungsinstitut der Universität Stuttgart">
// Copyright © 2023 - 2025 Visualisierungsinstitut der Universität Stuttgart.
// Licensed under the MIT licence. See LICENCE file for details.
// </copyright>
// <author>Christoph Müller</author>

#if !defined(_FPD_HEADER_SEGMENT_H)
#define _FPD_HEADER_SEGMENT_H
#pragma once

#include <cinttypes>

#include "fpd/abi.h"


FPD_NAMESPACE_BEGIN

/// <summary>
/// Defines the known types of header segments.
/// </summary>
/// <remarks>
/// The specification requires the least-significant three bytes of this
/// enumeration to be all-zero such that these bits can be filled with the size
/// of the header block.
/// </remarks>
enum class header_segment : std::uint32_t {

    /// <summary>
    /// The frame table, which is an array of <see cref="std::uint64_t" /> that
    /// represent the absolute offset of each frame from the begiin of the file.
    /// </summary>
    frame_table = 0x01 << 24,

    /// <summary>
    /// The format description block holds the elements of a
    /// <see cref="format_description" />, which describes how the particles in
    /// the file should be interpreted.
    /// </summary>
    format_description = 0x02 << 24,

    /// <summary>
    /// The user semantics segment holds an array of zero-terminated UTF-16
    /// strings that describe all user-defined semantics used in the file.
    /// </summary>
    user_semantics = 0x03 << 24,

    /// <summary>
    /// The description segment holds a single UTF-16 string that holds optional
    /// human-readable information about the data in the file.
    /// </summary>
    description = 0x04 << 24,

    /// <summary>
    /// The bounding box segment holds the global bounding box and potentially
    /// additional per-frame bounding boxes.
    /// </summary>
    bounding_box = 0x05 << 24,

    /// <summary>
    /// The global particle properties segments holds a table of
    /// <see cref="semantic" />s that are the same for all particles in the file.
    /// </summary>
    global_particle_properties = 0x06 << 24,

    /// <summary>
    /// Specifies the spatial hierarchy of the data.
    /// </summary>
    /// <remarks>
    /// The content of the section depends of the first 32-bit value, which
    /// determines the type of the hierarchy.
    /// </remarks>
    spatial_hierarchy = 0x07 << 24,

    /// <summary>
    /// Specifies whether the particle data are compressed.
    /// </summary>
    compression = 0x08 << 24,

    /// <summary>
    /// Specifies the offset of a footer segment from the begin of the file.
    /// </summary>
    /// <remarks>
    /// The segment contains a single 64-bit value, which is the offset of the
    /// first footer block from the begin of the file.
    /// </remarks>
    footer = 0x09 << 24
};

FPD_NAMESPACE_END

#endif /* !defined(_FPD_HEADER_SEGMENT_H) */
