// <copyright file="dependent_bool.h" company="Visualisierungsinstitut der Universität Stuttgart">
// Copyright © 2025 Visualisierungsinstitut der Universität Stuttgart.
// Licensed under the MIT licence. See LICENCE file for details.
// </copyright>
// <author>Christoph Müller</author>

#if !defined(_FPD_DEPENDENT_BOOL_H)
#define _FPD_DEPENDENT_BOOL_H
#pragma once

#include <type_traits>

#include "fpd/abi.h"


FPD_DETAIL_NAMESPACE_BEGIN

/// <summary>
/// Derives <see cref="std::false_type" /> for any type
/// <typeparamref name="TType" />.
/// </summary>
/// <typeparam name="TType">An arbitrary type.</typeparam>
template<class TType> struct dependent_false : std::false_type { };


/// <summary>
/// Derives <see cref="std::false_type" /> for any type
/// <typeparamref name="TType" />.
/// </summary>
/// <typeparam name="TType">An arbitrary type.</typeparam>
template<class TType>
constexpr bool dependent_false_v = dependent_false<TType>::value;


/// <summary>
/// Derives <see cref="std::true_type" /> for any type
/// <typeparamref name="TType" />.
/// </summary>
/// <typeparam name="TType">An arbitrary type.</typeparam>
template<class TType> struct dependent_true : std::true_type {};


/// <summary>
/// Derives <see cref="std::true_type" /> for any type
/// <typeparamref name="TType" />.
/// </summary>
/// <typeparam name="TType">An arbitrary type.</typeparam>
template<class TType>
constexpr bool dependent_true_v = dependent_true<TType>::value;

FPD_DETAIL_NAMESPACE_END

#endif /* !defined(_FPD_DEPENDENT_BOOL_H) */
