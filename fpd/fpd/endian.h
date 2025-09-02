// <copyright file="endian.h" company="Visualisierungsinstitut der Universität Stuttgart">
// Copyright © 2023 - 2025 Visualisierungsinstitut der Universität Stuttgart.
// Licensed under the MIT licence. See LICENCE file for details.
// </copyright>
// <author>Christoph Müller</author>

#if !defined(_FPD_ENDIAN_H)
#define _FPD_ENDIAN_H
#pragma once

#include <cassert>
#include <type_traits>

#include "fpd/swap_bytes.h"


FPD_NAMESPACE_BEGIN

/// <summary>
/// Specifies the endians the library supports.
/// </summary>
enum class endian : std::uint16_t {

    /// <summary>
    /// The endian is unknown, which usually means that the library will be
    /// unable to process data.
    /// </summary>
    undefined = 0x000,

    /// <summary>
    /// Little endian representation with least significant byte coming
    /// first in memory.
    /// </summary>
    little = 0x0102,

    /// <summary>
    /// Big endian representation with most significant byte coming first in
    /// memory.
    /// </summary>
    big = 0x0201,

    /// <summary>
    /// Endian of the system the code is running on.
    /// </summary>
    system = '\1\2'
};


/// <summary>
/// Answer wether the given <paramref name="endian" /> requires a byte-order
/// conversion on the current system.
/// </summary>
/// <param name="endian">The reference endian, which determines whether the
/// bytes must be swapped (if this value is not <see cref="endian::system" />)
/// or not (if this value is <see cref="endian::undefined" /> or
/// not <see cref="endian::system" />).</param>
/// <returns><c>true</c> if an endian conversion is required, <c>false</c>
/// otherwise.</returns>
inline constexpr bool requires_conversion(_In_ const endian endian) noexcept {
    return ((endian != endian::undefined) && (endian != endian::system));
}


/// <summary>
/// Swaps the byte order of <paramref name="value" /> if
/// <paramref name="endian" /> is valid and not equal to
/// <paramref name="system" />.
/// </summary>
/// <typeparam name="TValue">The type of the value to be swapped, which must
/// be a type for which an implementation of <see cref="swap_bytes" />
/// exists.</typeparam>
/// <param name="value">The value to be swapped.</param>
/// <param name="endian">The reference endian, which determines whether the
/// bytes will be swapped (if this value is not <see cref="endian::system" />)
/// or not (if this value is <see cref="endian::undefined" /> or
/// not <see cref="endian::system" />).</param>
/// <returns>The potentially swapped value.</returns>
template<class TValue>
TValue swap_endian(_In_ const TValue value, _In_ const endian endian) noexcept {
    if (requires_conversion(endian)) {
        return swap_bytes(value);
    } else {
        return value;
    }
}


/// <summary>
/// Swaps the byte order of <paramref name="cnt" /> values at
/// <paramref name="src" /> if <paramref name="endian" /> is valid and not equal
/// to <paramref name="system" />.
/// </summary>
/// <remarks>
/// It is strongly recommended to use the in-place variant for large amounts of
/// data as this does not do anything if the byte order is already as expected.
/// </remarks>
/// <typeparam name="TValue">The type of the value to be swapped, which must
/// be a type for which an implementation of <see cref="swap_bytes" />
/// exists.</typeparam>
/// <param name="dst">A pointer to a buffer of <paramref name="cnt" />
/// elements to receive the swapped bytes. If no endian conversion is necessary,
/// this will receive a <see cref="memcpy" /> of <paramref name="src" />.
/// </param>
/// <param name="src">A pointer to a buffer of <paramref name="cnt" />
/// elements to swap the bytes of.</param>
/// <param name="cnt">The number of values to swap.</param>
/// <param name="endian">The reference endian, which determines whether the
/// bytes will be swapped (if this value is not <see cref="endian::system" />)
/// or not (if this value is <see cref="endian::undefined" /> or
/// not <see cref="endian::system" />).</param>
template<class TValue> void swap_endian(_Out_writes_(cnt) TValue *dst,
        _In_reads_(cnt) const TValue *src,
        _In_ const std::size_t cnt,
        _In_ const endian endian) noexcept {
    if (requires_conversion(endian)) {
        swap_bytes(dst, src, cnt);
    } else {
        ::memcpy(dst, src, cnt * sizeof(TValue));
    }
}


/// <summary>
/// Swaps the byte order of <paramref name="cnt" /> <paramref name="values" />
/// in place if <paramref name="endian" /> is valid and not equal to
/// <paramref name="system" />.
/// </summary>
/// <remarks>
/// This function does nothing if <paramref name="endian" /> is
/// <see cref="endian::system" /> (or if it is <see cref="endian::undefined" />,
/// which should never be the case).
/// </remarks>
/// <typeparam name="TValue">The type of the value to be swapped, which must
/// be a type for which an implementation of <see cref="swap_bytes" />
/// exists.</typeparam>
/// <param name="values">An array of <paramref name="cnt" /> values to swap
/// the byte order of. If no conversion is required, this array will
/// remain untouched.</param>
/// <param name="cnt">The number of values to swap.</param>
/// <param name="endian">The reference endian, which determines whether the
/// bytes will be swapped (if this value is not <see cref="endian::system" />)
/// or not (if this value is <see cref="endian::undefined" /> or
/// not <see cref="endian::system" />).</param>
template<class TValue> void swap_endian(_Inout_updates_(cnt) TValue *values,
        _In_ const std::size_t cnt,
        _In_ const endian endian) noexcept {
    if (requires_conversion(endian)) {
        swap_bytes(values, cnt);
    }
}


/// <summary>
/// Interprets <paramref name="value" /> as <typeparamref name="TValue" /> and
/// swaps its byte order if <paramref name="endian" /> is valid and not equal to
/// <paramref name="system" />.
/// </summary>
/// <typeparam name="TValue">The type of the value to be swapped, which must
/// be a type for which an implementation of <see cref="swap_bytes" />
/// exists.</typeparam>
/// <param name="value">A pointer to a variable of type
/// <typeparamref name="TValue" />.</param>
/// <param name="endian">The reference endian, which determines whether the
/// bytes will be swapped (if this value is not <see cref="endian::system" />)
/// or not (if this value is <see cref="endian::undefined" /> or
/// not <see cref="endian::system" />).</param>
/// <returns>The (potentially byte-swapped) value.</returns>
template<class TValue>
TValue swap_endian_as(_In_ const void *value, _In_ const endian endian) {
    assert(value != nullptr);
    auto retval = *static_cast<const TValue *>(value);

    if (requires_conversion(endian)) {
        retval = swap_bytes(retval);
    }

    return retval;
}


/// <summary>
/// Reverses the byte order of <paramref name="cnt" /> bytes starting at
/// <paramref name="value" /> if <paramref name="endian" /> is valid and not
/// equal to the system endian.
/// </summary>
/// <typeparam name="TValue">A pointer type to a byte-sized value.</typeparam>
/// <param name="value">A pointer to <parmaref name="cnt" /> valid bytes.
/// </param>
/// <param name="cnt">The number of bytes to swap.</param>
/// <param name="endian">The endian the input is currently in.</param>
template<class TValue>
std::enable_if_t<sizeof(TValue) == 1> swap_endian_as(
        _Inout_updates_(cnt) TValue *value,
        _In_ const std::size_t cnt,
        _In_ const endian endian) {
    assert(value != nullptr);
    if (requires_conversion(endian)) {
        swap_bytes(value, 1, cnt, cnt);
    }
}


/// <summary>
/// Reverses the byte order of <paramref name="cnt" /> numbers of
/// <paramref name="size" /> bytes when copying from <paramref name="src" />
/// to <parmaref name="dst" /> such that the destination data are in system
/// endian.
/// </summary>
/// <typeparam name="TValue">A pointer type to a byte-sized value.</typeparam>
/// <param name="value">A pointer to <parmaref name="cnt" /> elements of size
/// <paramref name="size" /> each.</param>
/// <param name="cnt">The number of elements to swap.</param>
/// <param name="size">The size of each element in bytes.</param>
/// <param name="endian">The endian the input is currently in.</param>
/// <returns></returns>
template<class TValue>
std::enable_if_t<sizeof(TValue) == 1> swap_endian_as(
        _Out_writes_bytes_(cnt * size) TValue *dst,
        _In_reads_bytes_(cnt * size) const TValue *src,
        _In_ const std::size_t cnt,
        _In_ const std::size_t size,
        _In_ const endian endian) {
    ::memcpy(dst, src, cnt * size);
    if (requires_conversion(endian)) {
        swap_bytes(dst, cnt, size, size);
    }
}


/// <summary>
/// Swaps the bytes of the given enumeration type as its underlying type.
/// </summary>
/// <typeparam name="TValue">The type of the enumeration.</typeparam>
/// <param name="value">A pointer to an enumeration value from
/// <typeparamref name="TValue" />.</param>
/// <param name="endian">The reference endian, which determines whether the
/// bytes will be swapped (if this value is not <see cref="endian::system" />)
/// or not (if this value is <see cref="endian::undefined" /> or
/// not <see cref="endian::system" />).</param>
/// <returns>The (potentially byte-swapped) value.</returns>
template<class TValue>
inline TValue swap_enum_endian(_In_ const void *value,
        _In_ const endian endian) {
    typedef typename std::underlying_type<TValue>::type int_type;
    return static_cast<TValue>(swap_endian_as<int_type>(value, endian));
}

FPD_NAMESPACE_END

#endif /* !defined(_FPD_ENDIAN_H) */
