// <copyright file="null_coalesce.h" company="Visualisierungsinstitut der Universität Stuttgart">
// Copyright © 2023 - 2025 Visualisierungsinstitut der Universität Stuttgart.
// Licensed under the MIT licence. See LICENCE file for details.
// </copyright>
// <author>Christoph Müller</author>

#if !defined(_FPD_NULL_COALESCE_H)
#define _FPD_NULL_COALESCE_H
#pragma once

#include "fpd/abi.h"
#include "fpd/literal.h"


FPD_NAMESPACE_BEGIN

/// <summary>
/// Return <paramref name="lhs" /> if it is non-<c>nullptr</c> or
/// <paramref name="rhs" /> otherwise.
/// </summary>
/// <remarks>
/// This function mimics the behaviour of the <c>??</c> operator in C#.
/// </remarks>
/// <typeparam name="TValue">The type of the pointers to coalesce.</typeparam>
/// <param name="lhs">The left-hand side operand, which is returned preferably.
/// </param>
/// <param name="rhs">The right-hand side operation, which is returned if
/// <paramref name="lhs" /> is <c>nullptr</c>.</param>
/// <returns>The first non-<c>nullptr</c> parameters.</returns>
template<class TValue> inline constexpr _Ret_maybenull_ TValue *null_coalesce(
        _In_opt_ TValue *lhs, _In_opt_ TValue *rhs) noexcept {
    return (lhs != nullptr) ? lhs : rhs;
}

/// <summary>
/// Return <paramref name="str" /> if it is non-<c>nullptr</c> or an empty
/// string otherwise.
/// </summary>
/// <typeparam name="TChar">The character type of the string.</typeparam>
/// <param name="str">The string to be returned preferably.</param>
/// <returns><c>str</c> if it is valid, an empty string otherwise.</returns>
template<class TChar> inline constexpr _Ret_valid_ const TChar *null_coalesce(
        _In_opt_z_ const TChar *str) noexcept {
    return null_coalesce(str, FPD_TPL_LITERAL(TChar, ""));
}

FPD_NAMESPACE_END

#endif /* !defined(_FPD_NULL_COALESCE_H) */
