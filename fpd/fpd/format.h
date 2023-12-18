// <copyright file="format.h" company="Visualisierungsinstitut der Universität Stuttgart">
// Copyright © 2023 Visualisierungsinstitut der Universität Stuttgart.
// Licensed under the MIT licence. See LICENCE file for details.
// </copyright>
// <author>Christoph Müller</author>

#pragma once

#include <cinttypes>

#include "fpd/abi.h"


FPD_NAMESPACE_BEGIN

/// <summary>
/// Defines possible scalar and vector formats.
/// </summary>
enum class format : std::uint32_t {

    /// <summary>
    /// An 8-bit signed scalar.
    /// </summary>
    int8x,

    /// <summary>
    /// An 8-bit signed 2-vector.
    /// </summary>
    int8xy,

    /// <summary>
    /// An 8-bit signed 3-vector.
    /// </summary>
    int8xyz,

    /// <summary>
    /// An 8-bit signed 4-vector.
    /// </summary>
    int8xyzw,

    /// <summary>
    /// An 8-bit unsigned scalar.
    /// </summary>
    uint8x,

    /// <summary>
    /// An 8-bit unsigned 2-vector.
    /// </summary>
    uint8xy,

    /// <summary>
    /// An 8-bit unsigned 3-vector.
    /// </summary>
    uint8xyz,

    /// <summary>
    /// An 8-bit unsigned 4-vector.
    /// </summary>
    uint8xyzw,

    /// <summary>
    /// A 16-bit signed scalar.
    /// </summary>
    int16x,

    /// <summary>
    /// n 16-bit signed 2-vector.
    /// </summary>
    int16xy,

    /// <summary>
    /// A 16-bit signed 3-vector.
    /// </summary>
    int16xyz,

    /// <summary>
    /// A 16-bit signed 4-vector.
    /// </summary>
    int16xyzw,

    /// <summary>
    /// A 16-bit unsigned scalar.
    /// </summary>
    uint16x,

    /// <summary>
    /// A 16-bit unsigned 2-vector.
    /// </summary>
    uint16xy,

    /// <summary>
    /// A 16-bit unsigned 3-vector.
    /// </summary>
    uint16xyz,

    /// <summary>
    /// A 16-bit unsigned 4-vector.
    /// </summary>
    uint16xyzw,

    /// <summary>
    /// A 32-bit signed scalar.
    /// </summary>
    int32x,

    /// <summary>
    /// A 32-bit signed 2-vector.
    /// </summary>
    int32xy,

    /// <summary>
    /// A 32-bit signed 3-vector.
    /// </summary>
    int32xyz,

    /// <summary>
    /// A 32-bit signed 4-vector.
    /// </summary>
    int32xyzw,

    /// <summary>
    /// A 32-bit unsigned scalar.
    /// </summary>
    uint32x,

    /// <summary>
    /// A 32-bit unsigned 2-vector.
    /// </summary>
    uint32xy,

    /// <summary>
    /// A 32-bit unsigned 3-vector.
    /// </summary>
    uint32xyz,

    /// <summary>
    /// A 32-bit unsigned 4-vector.
    /// </summary>
    uint32xyzw,

    /// <summary>
    /// A 64-bit signed scalar.
    /// </summary>
    int64x,

    /// <summary>
    /// A 64-bit signed 2-vector.
    /// </summary>
    int64xy,

    /// <summary>
    /// A 64-bit signed 3-vector.
    /// </summary>
    int64xyz,

    /// <summary>
    /// A 64-bit signed 4-vector.
    /// </summary>
    int64xyzw,

    /// <summary>
    /// A 64-bit unsigned scalar.
    /// </summary>
    uint64x,

    /// <summary>
    /// A 64-bit unsigned 2-vector.
    /// </summary>
    uint64xy,

    /// <summary>
    /// An 6-bit unsigned 3-vector.
    /// </summary>
    uint64xyz,

    /// <summary>
    /// An 6-bit unsigned 4-vector.
    /// </summary>
    uint64xyzw,

    /// <summary>
    /// A 32-bit floating-point scalar.
    /// </summary>
    float32x,

    /// <summary>
    /// A 32-bit floating-point 2-vector.
    /// </summary>
    float32xy,

    /// <summary>
    /// A 32-bit floating-point 3-vector.
    /// </summary>
    float32xyz,

    /// <summary>
    /// A 32-bit floating-point 4-vector.
    /// </summary>
    float32xyzw,

    /// <summary>
    /// A 32-bit floating-point scalar.
    /// </summary>
    float64x,

    /// <summary>
    /// A 32-bit floating-point 2-vector.
    /// </summary>
    float64xy,

    /// <summary>
    /// A 32-bit floating-point 3-vector.
    /// </summary>
    float64xyz,

    /// <summary>
    /// A 32-bit floating-point 4-vector.
    /// </summary>
    float64xyzw
};

FPD_NAMESPACE_END
