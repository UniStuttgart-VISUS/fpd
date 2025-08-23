// <copyright file="type_components.h" company="Visualisierungsinstitut der Universität Stuttgart">
// Copyright © 2023 - 2025 Visualisierungsinstitut der Universität Stuttgart.
// Licensed under the MIT licence. See LICENCE file for details.
// </copyright>
// <author>Christoph Müller</author>

#if !defined(_FPD_TYPE_COMPONENTS_H)
#define _FPD_TYPE_COMPONENTS_H
#pragma once

#include <array>
#include <cstdlib>
#include <type_traits>

#include "fpd/abi.h"


FPD_NAMESPACE_BEGIN

/// <summary>
/// Determines the number of components <typeparamref name="TValue" />
/// has, which is typically 1, zero for <c>void</c> and the size of an
/// <see cref="std::array" />.
/// </summary>
/// <typeparam name="TValue">The type to determine the components of.
/// </typeparam>
template<class TValue> struct type_components final
    : public std::integral_constant<std::size_t, 1> { };

/// <summary>
/// Determines the number of components of <c>void</c> as zero.
/// </summary>
template<> struct type_components<void> final
    : public std::integral_constant<std::size_t, 0> { };

/// <summary>
/// Determines the number of components of <c>void</c> as zero.
/// </summary>
template<class TValue, std::size_t N>
struct type_components<std::array<TValue, N>> final
    : public std::integral_constant<std::size_t, N> { };

/// <summary>
/// Determines the number of components <typeparamref name="TValue" />
/// has, which is typically 1, zero for <c>void</c> and the size of an
/// <see cref="std::array" />.
/// </summary>
/// <typeparam name="TValue">The type to determine the components of.
/// </typeparam>
template<class TValue>
constexpr std::size_t type_components_v = type_components<TValue>::value;

FPD_NAMESPACE_END

#endif /* !defined(_FPD_TYPE_COMPONENTS_H) */
