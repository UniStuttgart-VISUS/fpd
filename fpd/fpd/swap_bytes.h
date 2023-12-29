// <copyright file="swap_bytes.h" company="Visualisierungsinstitut der Universität Stuttgart">
// Copyright © 2023 Visualisierungsinstitut der Universität Stuttgart.
// Licensed under the MIT licence. See LICENCE file for details.
// </copyright>
// <author>Christoph Müller</author>

#pragma once

#include <cassert>
#include <cinttypes>
#include <memory>

#include "fpd/abi.h"
#include "fpd/annotations.h"


FPD_NAMESPACE_BEGIN

/// <summary>
/// Does nothing to <paramref name="v" />.
/// </summary>
/// <param name="v">The value to swap the byte order of.</param>
/// <returns><paramref name="v" />.</returns>
inline constexpr std::uint8_t swap_bytes(_In_ const std::uint8_t v) noexcept {
    return v;
}


/// <summary>
/// Does nothing to <paramref name="v" />.
/// </summary>
/// <param name="v">The value to swap the byte order of.</param>
/// <returns><paramref name="v" />.</returns>
inline constexpr std::int8_t swap_bytes(_In_ const std::int8_t v) noexcept {
    return v;
}


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
        | ((v & 0x0000FF00) << 8)
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


/// <summary>
/// Swaps the byte order of a UTF-16 code unit.
/// </summary>
/// <param name="v">The value to swap the byte order of.</param>
/// <returns>The value when the byte order is swapped.</returns>
inline constexpr char16_t swap_bytes(_In_ const char16_t v) noexcept {
    return static_cast<char16_t>(swap_bytes(static_cast<std::uint16_t>(v)));
}


namespace detail {

    /// <summary>
    /// Swaps <paramref name="v" /> as if it were of type
    /// <typeparamref name="TUnderlying" />, for which an explicit
    /// implementation of <see cref="swap_bytes" /> must exist.
    /// </summary>
    /// <remarks>
    /// Unfortunately, this cannot be <c>constepxr</c> as we are using a
    /// <c>reinterpret_cast</c>.
    /// </remarks>
    /// <typeparam name="TUnderlying">The type used to swap the bytes, which
    /// must be an integral numeric type of the same size as
    /// <typeparamref name="TValue" />.</typeparam>
    /// <typeparam name="TValue">The type of the value to swap the byte order
    /// of.</typeparam>
    /// <param name="v">The value to swap the byte order of.</param>
    /// <returns>The value when the byte order is swapped.</returns>
    template<class TUnderlying, class TValue>
    inline TValue swap_as(_In_ const TValue v) noexcept {
        static_assert(std::is_integral<TUnderlying>::value, "The underlying "
            "type that is being swapped must be integral.");
        static_assert(sizeof(TUnderlying) == sizeof(TValue), "The underlying "
            "integral type must have the same size as the actual value.");
        auto bytes = reinterpret_cast<const TUnderlying *>(std::addressof(v));
        auto retval = FPD_NAMESPACE::swap_bytes(*bytes);
        return *reinterpret_cast<TValue *>(std::addressof(retval));
    }

    /// <summary>
    /// Loops over <paramref name="cnt" /> <paramref name="src" /> of the
    /// numeric type <typeparamref name="TValue" /> and swaps their byte order.
    /// </summary>
    /// <typeparam name="TValue">The type of the numbers to swap.</typeparam>
    /// <param name="dst">A pointer to a buffer of <paramref name="cnt" />
    /// elements to receive the swapped bytes.</param>
    /// <param name="src">A pointer to a buffer of <paramref name="cnt" />
    /// elements to swap the bytes of.</param>
    /// <param name="cnt">The number of values to swap.</param>
    template<class TValue> void swap_bytes_basic(
            _Out_writes_(cnt) TValue *dst,
            _In_reads_(cnt) const TValue *src,
            _In_ const std::size_t cnt) {
        assert(dst != nullptr);
        assert(src != nullptr);
        // Loop unrolling using Duff's device.
        auto i = (cnt + 7) / 8;

        switch (cnt % 8) {
            case 0:
                do {
                    *dst++ = swap_bytes(*src++);
                    case 7:
                        *dst++ = swap_bytes(*src++);
                    case 6:
                        *dst++ = swap_bytes(*src++);
                    case 5:
                        *dst++ = swap_bytes(*src++);
                    case 4:
                        *dst++ = swap_bytes(*src++);
                    case 3:
                        *dst++ = swap_bytes(*src++);
                    case 2:
                        *dst++ = swap_bytes(*src++);
                    case 1:
                        *dst++ = swap_bytes(*src++);
                } while (--i > 0);
        }
    }

} /* namespace detail */


/// <summary>
/// Swaps the byte order of a 32-bit floating point number.
/// </summary>
/// <remarks>
/// Unfortunately, the byte order of <c>float</c> is not defined, but usually,
/// it is the same as for integral values. Based on this assumption, the
/// implementation swaps the four bytes as if it were a
/// <see cref="std::uint32_t" />.
/// </remarks>
/// <param name="v">The value to swap the byte order of.</param>
/// <returns>The value when the byte order is swapped.</returns>
inline float swap_bytes(_In_ const float v) noexcept {
    return detail::swap_as<std::uint32_t>(v);
}


/// <summary>
/// Swaps the byte order of a 64-bit floating point number.
/// </summary>
/// <remarks>
/// Unfortunately, the byte order of <c>double</c> is not defined, but usually,
/// it is the same as for integral values. Based on this assumption, the
/// implementation swaps the four bytes as if it were a
/// <see cref="std::uint64_t" />.
/// </remarks>
/// <param name="v">The value to swap the byte order of.</param>
/// <returns>The value when the byte order is swapped.</returns>
inline double swap_bytes(_In_ const double v) noexcept {
    return detail::swap_as<std::uint64_t>(v);
}


/// <summary>
/// Loops over <paramref name="cnt" /> <paramref name="src" /> of the
/// numeric type <typeparamref name="TValue" /> and swaps their byte order.
/// </summary>
/// <typeparam name="TValue">The type of the numbers to swap.</typeparam>
/// <param name="dst">A pointer to a buffer of <paramref name="cnt" />
/// elements to receive the swapped bytes.</param>
/// <param name="src">A pointer to a buffer of <paramref name="cnt" />
/// elements to swap the bytes of.</param>
/// <param name="cnt">The number of values to swap.</param>
template<class TValue> inline void swap_bytes(
        _Out_writes_(cnt) TValue *dst,
        _In_reads_(cnt) const TValue *src,
        _In_ const std::size_t cnt) {
    detail::swap_bytes_basic(dst, src, cnt);
}


/// <summary>
/// Loops over <paramref name="cnt" /> <paramref name="values" /> of the
/// numeric type <typeparamref name="TValue" /> and swaps their byte order
/// in place.
/// </summary>
/// <typeparam name="TValue">The type of the numbers to swap.</typeparam>
/// <param name="values">An array of <paramref name="cnt" /> values to swap
/// the byte order of.</param>
/// <param name="cnt">The number of values to swap.</param>
template<class TValue> inline void swap_bytes(
        _Inout_updates_(cnt) TValue *values,
        _In_ const std::size_t cnt) {
    detail::swap_bytes_basic(values, values, cnt);
}

FPD_NAMESPACE_END
