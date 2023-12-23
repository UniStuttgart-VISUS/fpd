// <copyright file="swap_bytes.h" company="Visualisierungsinstitut der Universität Stuttgart">
// Copyright © 2023 Visualisierungsinstitut der Universität Stuttgart.
// Licensed under the MIT licence. See LICENCE file for details.
// </copyright>
// <author>Christoph Müller</author>

#pragma once

#include <cassert>
#include <cinttypes>

#include "fpd/abi.h"
#include "fpd/annotations.h"


FPD_NAMESPACE_BEGIN

/// <summary>
/// Swaps the byte order of a <see cref="std::uint16_t" />.
/// </summary>
/// <param name="v">The value to swap the byte order of.</param>
/// <returns>The value when the byte order is swapped.</returns>
inline constexpr std::uint16_t swap_bytes(_In_ const std::uint16_t v) noexcept {
    return (((v & 0xFF00) >> 8) | ((v & 0x00FF) << 8));
}


/// <summary>
/// Swaps the byte order of a <see cref="std::int16_t" />.
/// </summary>
/// <param name="v">The value to swap the byte order of.</param>
/// <returns>The value when the byte order is swapped.</returns>
inline constexpr std::int16_t swap_bytes(_In_ const std::int16_t v) noexcept {
    return static_cast<std::int16_t>(swap_bytes(static_cast<std::uint16_t>(v)));
}


/// <summary>
/// Swaps the byte order of a <see cref="std::uint32_t" />.
/// </summary>
/// <param name="v">The value to swap the byte order of.</param>
/// <returns>The value when the byte order is swapped.</returns>
inline constexpr std::uint32_t swap_bytes(_In_ const std::uint32_t v) noexcept {
    return ((v & 0xFF000000) >> 24)
        | ((v & 0x00FF0000) >> 8)
        | ((v & 0x0000F00) << 8)
        | ((v & 0x000000FF) << 24);
}


/// <summary>
/// Swaps the byte order of a <see cref="std::int32_t" />.
/// </summary>
/// <param name="v">The value to swap the byte order of.</param>
/// <returns>The value when the byte order is swapped.</returns>
inline constexpr std::int32_t swap_bytes(_In_ const std::int32_t v) noexcept {
    return static_cast<std::int32_t>(swap_bytes(static_cast<std::uint32_t>(v)));
}


/// <summary>
/// Swaps the byte order of a <see cref="std::uint64_t" />.
/// </summary>
/// <param name="v">The value to swap the byte order of.</param>
/// <returns>The value when the byte order is swapped.</returns>
inline constexpr std::uint64_t swap_bytes(_In_ const std::uint64_t v) noexcept {
    return ((v & 0xFF00000000000000ull) >> 56)
        | ((v & 0x00FF000000000000ull) >> 40)
        | ((v & 0x0000FF0000000000ull) >> 24)
        | ((v & 0x000000FF00000000ull) >> 8)
        | ((v & 0x00000000FF000000ull) << 8)
        | ((v & 0x0000000000FF0000ull) << 24)
        | ((v & 0x000000000000FF00ull) << 40)
        | ((v & 0x00000000000000FFull) << 56);
}


/// <summary>
/// Swaps the byte order of a <see cref="std::int64_t" />.
/// </summary>
/// <param name="v">The value to swap the byte order of.</param>
/// <returns>The value when the byte order is swapped.</returns>
inline constexpr std::int64_t swap_bytes(_In_ const std::int64_t v) noexcept {
    return static_cast<std::int64_t>(swap_bytes(static_cast<std::uint64_t>(v)));
}

FPD_NAMESPACE_END
