// <copyright file="type_collapse.h" company="Visualisierungsinstitut der Universität Stuttgart">
// Copyright © 2025 Visualisierungsinstitut der Universität Stuttgart.
// Licensed under the MIT licence. See LICENCE file for details.
// </copyright>
// <author>Christoph Müller</author>

#if !defined(_FPD_TYPE_COLLAPSE_H)
#define _FPD_TYPE_COLLAPSE_H
#pragma once

#include "fpd/abi.h"


FPD_DETAIL_NAMESPACE_BEGIN

/// <summary>
/// Collapses any list of types to <see langword="void" />.
/// </summary>
/// <typeparam name="TTypes">A list of arbitrary types</typeparam>
template<class... TTypes> struct type_collapse final {
    typedef void type;
};

/// <summary>
/// Collapses any list of types to <see langword="void" />.
/// </summary>
/// <typeparam name="TTypes">A list of arbitrary types</typeparam>
template<class... TTypes>
using type_collapse_t = typename type_collapse<TTypes...>::type;

FPD_DETAIL_NAMESPACE_END

#endif /* !defined(_FPD_TYPE_COLLAPSE_H) */
