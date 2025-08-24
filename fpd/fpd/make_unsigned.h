// <copyright file="make_unsigned.h" company="Visualisierungsinstitut der Universität Stuttgart">
// Copyright © 2023 - 2025 Visualisierungsinstitut der Universität Stuttgart.
// Licensed under the MIT licence. See LICENCE file for details.
// </copyright>
// <author>Christoph Müller</author>

#if !defined(_FPD_MAKE_UNSIGNED_H)
#define _FPD_MAKE_UNSIGNED_H
#pragma once

#include <cinttypes>
#include <type_traits>

#include "fpd/abi.h"


FPD_DETAIL_NAMESPACE_BEGIN

/// <summary>
/// Derives an unsigned integral type with the same size as
/// <typeparamref name="TType" />.
/// </summary>
/// <typeparam name="TType">The type to get the equivalently sized unsigned
/// integer for.</typeparam>
/// <typeparam name="IsIntegral">Indicates whether <typeparamref name="TType" />
/// is an integral type.</typeparam>
template<class TType, bool IsIntegral = std::is_integral_v<TType>>
struct make_unsigned final : std::make_unsigned<TType> { };

/// <summary>
/// Specialisation for <see langword="float" />.
/// </summary>
template<> struct make_unsigned<float, false> final {
    static_assert(sizeof(float) == sizeof(std::uint32_t),
        "float is assume to be be 32-bit.");
    typedef std::uint32_t type;
};

/// <summary>
/// Specialisation for <see langword="double" />.
/// </summary>
template<> struct make_unsigned<double, false> final {
    static_assert(sizeof(double) == sizeof(std::uint64_t),
        "float is assume to be be 64-bit.");
    typedef std::uint64_t type;
};


/// <summary>
/// Derives an unsigned integral type with the same size as
/// <typeparamref name="TType" />.
/// </summary>
/// <typeparam name="TType">The type to get the equivalently sized unsigned
/// integer for.</typeparam>
template<class TType>
using make_unsigned_t = typename make_unsigned<TType>::type;

FPD_DETAIL_NAMESPACE_END

#endif /* !defined(_FPD_MAKE_UNSIGNED_H) */
