// <copyright file="detector.h" company="Visualisierungsinstitut der Universität Stuttgart">
// Copyright © 2025 Visualisierungsinstitut der Universität Stuttgart.
// Licensed under the MIT licence. See LICENCE file for details.
// </copyright>
// <author>Christoph Müller</author>

#if !defined(_FPD_DETECTOR_H)
#define _FPD_DETECTOR_H
#pragma once

#include "fpd/type_collapse.h"


FPD_DETAIL_NAMESPACE_BEGIN

/// <summary>
/// Detector for a method as described on
/// https://stackoverflow.com/questions/87372/check-if-a-class-has-a-member-function-of-a-given-signature.
/// </summary>
/// <typeparam name="TDetect">The type of the method or field detector, which
/// must be a template that receives a type and that can only be instantiated if
/// the method to be tested exists.</typeparam>
/// <typeparam name="TVoid">A type that collapses to <see langword="void" /> if
/// the more specialised variant of the type can be instantiated.</typeparam>
/// <typeparam name="TArgs">The arguments that are required to instantiate
/// <typeparamref name="TFunc" />.</typeparam>
template<template<class...> class TDetect, class TVoid, class... TArgs>
struct detector final : std::false_type { };

/// <summary>
/// Specialisation for types that have the method.
/// </summary>
/// <typeparam name="TDetect">The type of the method or field detector, which
/// must be a template that receives a type and that can only be instantiated if
/// the method to be tested exists.</typeparam>
/// <typeparam name="TArgs">The arguments that are required to instantiate
/// <typeparamref name="TFunc" />.</typeparam>
template<template<class...> class TDetect, class... TArgs>
struct detector<TDetect, type_collapse_t<TDetect<TArgs...>>, TArgs...> final
    : std::true_type { };

/// <summary>
/// Detector for a function or field on a type.
/// </summary>
/// <typeparam name="TDetect">The type of the method or field detector, which
/// must be a template that receives a type and that can only be instantiated if
/// the method to be tested exists.</typeparam>
/// <typeparam name="TArgs">The arguments that are required to instantiate
/// <typeparamref name="TFunc" />.</typeparam>
template<template<class...> class TDetect, class... TArgs>
constexpr auto detector_v = detector<TDetect, void, TArgs...>::value;

FPD_DETAIL_NAMESPACE_END

#endif /* !defined(_FPD_DETECTOR_H) */
