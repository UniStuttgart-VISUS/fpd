// <copyright file="type_size.h" company="Visualisierungsinstitut der Universität Stuttgart">
// Copyright © 2023 - 2025 Visualisierungsinstitut der Universität Stuttgart.
// Licensed under the MIT licence. See LICENCE file for details.
// </copyright>
// <author>Christoph Müller</author>

#if !defined(_FPD_TYPE_SIZE_H)
#define _FPD_TYPE_SIZE_H
#pragma once

#include <cstdlib>
#include <type_traits>

#include "fpd/abi.h"


FPD_NAMESPACE_BEGIN

/// <summary>
/// Determines the size of <typeparamref name="TValue" /> at compile time.
/// </summary>
/// <remarks>
/// The sole purpose of this template is handling the special case of
/// <c>void</c>, which should yield zero.
/// </remarks>
/// <typeparam name="TValue">The type to determine the size of.</typeparam>
template<class TValue> struct type_size final
    : public std::integral_constant<std::size_t, sizeof(TValue)> { };

/// <summary>
/// Determines the size of <c>void</c> as zero.
/// </summary>
template<> struct type_size<void> final
    : public std::integral_constant<std::size_t, 0> { };


/// <summary>
/// Determines the size of <typeparamref name="TValue" /> at compile time.
/// </summary>
/// <remarks>
/// The sole purpose of this template is handling the special case of
/// <c>void</c>, which should yield zero.
/// </remarks>
/// <typeparam name="TValue">The type to determine the size of.</typeparam>
template<class TValue>
constexpr std::size_t type_size_v = type_size<TValue>::value;

FPD_NAMESPACE_END

#endif /* !defined(_FPD_TYPE_SIZE_H) */
