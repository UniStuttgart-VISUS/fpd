// <copyright file="header_segment.h" company="Visualisierungsinstitut der Universität Stuttgart">
// Copyright © 2023 Visualisierungsinstitut der Universität Stuttgart.
// Licensed under the MIT licence. See LICENCE file for details.
// </copyright>
// <author>Christoph Müller</author>

#pragma once

#include <cinttypes>

#include "fpd/abi.h"
#include "fpd/annotations.h"


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
    bounding_box = 0x05 << 25,
};

FPD_NAMESPACE_END
