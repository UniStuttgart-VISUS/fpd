// <copyright file="enum_dispatch_list.h" company="Visualisierungsinstitut der Universität Stuttgart">
// Copyright © 2025 Visualisierungsinstitut der Universität Stuttgart.
// Licensed under the MIT licence. See LICENCE file for details.
// </copyright>
// <author>Christoph Müller</author>

#if !defined(_FPD_ENUM_DISPATCH_LIST_H)
#define _FPD_ENUM_DISPATCH_LIST_H
#pragma once

#include "fpd/abi.h"


FPD_DETAIL_NAMESPACE_BEGIN

/// <summary>
/// A compile-time list of enum values that can be used for dispatching runtime
/// enum values to compile-time constructs.
/// </summary>
/// <typeparam name="TEnum">The type of the enum.</typeparam>
/// <typeparam name="Values">The enum values in the list.</typeparam>
template<class TEnum, TEnum... Values> struct enum_dispatch_list final { };


/// <summary>
/// Recursion stop for <see cref="index_of" />.
/// </summary>
template<class TEnum>
constexpr inline std::size_t index_of(
        _In_ const TEnum,
        _In_ enum_dispatch_list<TEnum>,
        _In_ const std::size_t offset) noexcept {
    return offset;
}

/// <summary>
/// Find the position of <paramref name="value" /> in the given
/// <see cref="enum_dispatch_list" />.
/// </summary>
template<class TEnum, TEnum Value, TEnum... Values>
constexpr inline std::size_t index_of(
        _In_ const TEnum value,
        _In_ enum_dispatch_list<TEnum, Value, Values...>,
        _In_ const std::size_t offset) noexcept {
    if (value == Value) {
        return offset;

    } else {
        typedef enum_dispatch_list<TEnum, Values...> tail;
        return index_of(value, tail(), offset + 1);
    }
}

FPD_DETAIL_NAMESPACE_END

#endif /* !defined(_FPD_ENUM_DISPATCH_LIST_H) */
