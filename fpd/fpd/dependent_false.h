// <copyright file="dependent_false.h" company="Visualisierungsinstitut der Universität Stuttgart">
// Copyright © 2025 Visualisierungsinstitut der Universität Stuttgart.
// Licensed under the MIT licence. See LICENCE file for details.
// </copyright>
// <author>Christoph Müller</author>

#if !defined(_FPD_DEPENDENT_FALSE_H_)
#define _FPD_DEPENDENT_FALSE_H_
#pragma once

#include <type_traits>

#include "fpd/abi.h"


FPD_NAMESPACE_BEGIN

/// <summary>
/// Derives <see cref="std::false_type" /> for any type
/// <typeparamref name="TType" />.
/// </summary>
/// <typeparam name="TType">A type</typeparam>
template<class TType> struct dependent_false : std::false_type { };


/// <summary>
/// Derives <see cref="std::false_type" /> for any type
/// <typeparamref name="TType" />.
/// </summary>
/// <typeparam name="TType">A type</typeparam>
template<class TType>
constexpr bool dependent_false_v = dependent_false<TType>::value;

FPD_NAMESPACE_END

#endif /* !defined(_FPD_DEPENDENT_FALSE_H_) */
