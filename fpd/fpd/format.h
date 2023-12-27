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
/// <remarks>
/// <para>When adding new elements to this enumeration, do not forget to
/// register them in the declaration of the <see cref="formats" />
/// template below.</para>
/// </remarks>
enum class format : std::uint32_t {

    /// <summary>
    /// These are untyped data that will be processed as a byte, so in any
    /// place where the size of the type is required, the caller must provide
    /// the number of bytes, because the API cannot derive it.
    /// </summary>
    unknown = 0,

    /// <summary>
    /// An 8-bit signed scalar.
    /// </summary>
    int8x = 1,

    /// <summary>
    /// An 8-bit signed scalar.
    /// </summary>
    int8 = int8x,

    /// <summary>
    /// An 8-bit signed 2-vector.
    /// </summary>
    int8xy = 2,

    /// <summary>
    /// An 8-bit signed 3-vector.
    /// </summary>
    int8xyz = 3,

    /// <summary>
    /// An 8-bit signed 4-vector.
    /// </summary>
    int8xyzw = 4,

    /// <summary>
    /// An 8-bit unsigned scalar.
    /// </summary>
    uint8x = 5,

    /// <summary>
    /// An 8-bit unsigned scalar.
    /// </summary>
    uint8 = uint8x,

    /// <summary>
    /// An 8-bit unsigned 2-vector.
    /// </summary>
    uint8xy = 6,

    /// <summary>
    /// An 8-bit unsigned 3-vector.
    /// </summary>
    uint8xyz = 7,

    /// <summary>
    /// An 8-bit unsigned 4-vector.
    /// </summary>
    uint8xyzw = 8,

    /// <summary>
    /// A 16-bit signed scalar.
    /// </summary>
    int16x = 9,

    /// <summary>
    /// A 16-bit signed scalar.
    /// </summary>
    int16 = int16x,

    /// <summary>
    /// n 16-bit signed 2-vector.
    /// </summary>
    int16xy = 10,

    /// <summary>
    /// A 16-bit signed 3-vector.
    /// </summary>
    int16xyz = 11,

    /// <summary>
    /// A 16-bit signed 4-vector.
    /// </summary>
    int16xyzw = 12,

    /// <summary>
    /// A 16-bit unsigned scalar.
    /// </summary>
    uint16x = 13,

    /// <summary>
    /// A 16-bit unsigned scalar.
    /// </summary>
    uint16 = uint16x,

    /// <summary>
    /// A 16-bit unsigned 2-vector.
    /// </summary>
    uint16xy = 14,

    /// <summary>
    /// A 16-bit unsigned 3-vector.
    /// </summary>
    uint16xyz = 15,

    /// <summary>
    /// A 16-bit unsigned 4-vector.
    /// </summary>
    uint16xyzw = 16,

    /// <summary>
    /// A 32-bit signed scalar.
    /// </summary>
    int32x = 17,

    /// <summary>
    /// A 32-bit signed scalar.
    /// </summary>
    int32 = int32x,

    /// <summary>
    /// A 32-bit signed 2-vector.
    /// </summary>
    int32xy = 18,

    /// <summary>
    /// A 32-bit signed 3-vector.
    /// </summary>
    int32xyz = 19,

    /// <summary>
    /// A 32-bit signed 4-vector.
    /// </summary>
    int32xyzw = 20,

    /// <summary>
    /// A 32-bit unsigned scalar.
    /// </summary>
    uint32x = 21,

    /// <summary>
    /// A 32-bit unsigned scalar.
    /// </summary>
    uint32 = uint32x,

    /// <summary>
    /// A 32-bit unsigned 2-vector.
    /// </summary>
    uint32xy = 22,

    /// <summary>
    /// A 32-bit unsigned 3-vector.
    /// </summary>
    uint32xyz = 23,

    /// <summary>
    /// A 32-bit unsigned 4-vector.
    /// </summary>
    uint32xyzw = 24,

    /// <summary>
    /// A 64-bit signed scalar.
    /// </summary>
    int64x = 25,

    /// <summary>
    /// A 64-bit signed scalar.
    /// </summary>
    int64 = int64x,

    /// <summary>
    /// A 64-bit signed 2-vector.
    /// </summary>
    int64xy = 26,

    /// <summary>
    /// A 64-bit signed 3-vector.
    /// </summary>
    int64xyz = 27,

    /// <summary>
    /// A 64-bit signed 4-vector.
    /// </summary>
    int64xyzw = 28,

    /// <summary>
    /// A 64-bit unsigned scalar.
    /// </summary>
    uint64x = 29,

    /// <summary>
    /// A 64-bit unsigned scalar.
    /// </summary>
    uint64 = uint64x,

    /// <summary>
    /// A 64-bit unsigned 2-vector.
    /// </summary>
    uint64xy = 30,

    /// <summary>
    /// An 6-bit unsigned 3-vector.
    /// </summary>
    uint64xyz = 31,

    /// <summary>
    /// An 6-bit unsigned 4-vector.
    /// </summary>
    uint64xyzw = 32,

    /// <summary>
    /// A 32-bit floating-point scalar.
    /// </summary>
    float32x = 33,

    /// <summary>
    /// A 32-bit floating-point scalar.
    /// </summary>
    float32 = float32x,

    /// <summary>
    /// A 32-bit floating-point 2-vector.
    /// </summary>
    float32xy = 34,

    /// <summary>
    /// A 32-bit floating-point 3-vector.
    /// </summary>
    float32xyz = 35,

    /// <summary>
    /// A 32-bit floating-point 4-vector.
    /// </summary>
    float32xyzw = 36,

    /// <summary>
    /// A 32-bit floating-point scalar.
    /// </summary>
    float64x = 37,

    /// <summary>
    /// A 32-bit floating-point scalar.
    /// </summary>
    float64 = float64x,

    /// <summary>
    /// A 32-bit floating-point 2-vector.
    /// </summary>
    float64xy = 38,

    /// <summary>
    /// A 32-bit floating-point 3-vector.
    /// </summary>
    float64xyz = 39,

    /// <summary>
    /// A 32-bit floating-point 4-vector.
    /// </summary>
    float64xyzw = 40
};


/// <summary>
/// A compile-time list of <see cref="format" />s.
/// </summary>
/// <typeparam name="Formats">The formats in the list.</typeparam>
template<format... Formats> struct format_list final { };


/// <summary>
/// A compile-time list of all built-in formats.
/// </summary>
typedef format_list<
    format::unknown,
    format::int8x,
    format::int8xy,
    format::int8xyz,
    format::int8xyzw,
    format::uint8x,
    format::uint8xy,
    format::uint8xyz,
    format::uint8xyzw,
    format::int16x,
    format::int16xy,
    format::int16xyz,
    format::int16xyzw,
    format::uint16x,
    format::uint16xy,
    format::uint16xyz,
    format::uint16xyzw,
    format::int32x,
    format::int32xy,
    format::int32xyz,
    format::int32xyzw,
    format::uint32x,
    format::uint32xy,
    format::uint32xyz,
    format::uint32xyzw,
    format::int64x,
    format::int64xy,
    format::int64xyz,
    format::int64xyzw,
    format::uint64x,
    format::uint64xy,
    format::uint64xyz,
    format::uint64xyzw,
    format::float32x,
    format::float32xy,
    format::float32xyz,
    format::float32xyzw,
    format::float64x,
    format::float64xy,
    format::float64xyz,
    format::float64xyzw> formats;

FPD_NAMESPACE_END
