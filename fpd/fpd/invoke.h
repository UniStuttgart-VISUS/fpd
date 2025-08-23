// <copyright file="invoke.h" company="Visualisierungsinstitut der Universität Stuttgart">
// Copyright © 2024 - 2025 Visualisierungsinstitut der Universität Stuttgart.
// Licensed under the MIT licence. See LICENCE file for details.
// </copyright>
// <author>Christoph Müller</author>

#if !defined(_FPD_INVOKE_H)
#define _FPD_INVOKE_H
#pragma once

#include <functional>
#include <type_traits>

#include "fpd/abi.h"
#include "fpd/annotations.h"

FPD_DETAIL_NAMESPACE_BEGIN

/// <summary>
/// The invoker that allows for dispatching the call to the correct
/// implementation via template specialisations.
/// </summary>
/// <typeparam name="IsRef"></typeparam>
/// <typeparam name="IsRefWrapper"></typeparam>
template<bool IsRef, bool IsRefWrapper> struct invoker;

/// <summary>
/// Specialisation for &quot;normal&quot; pointer-to-member with the target
/// being a reference.
/// </summary>
template<> struct invoker<true, false> {
    template<class TReturn, class TDecl, class TOwner, class... TArgs>
    inline static auto invoke(_In_ TReturn TDecl:: *func,
            _In_ TOwner&& owner,
            _In_ TArgs&&... args)
            -> decltype((owner.*func)(std::forward<TArgs>(args)...)) {
        return (owner.*func)(std::forward<TArgs>(args)...);
    }
};

/// <summary>
/// Specialisation for &quot;normal&quot; pointer-to-member with the target
/// being a pointer.
/// </summary>
template<> struct invoker<false, false> {
    template<class TReturn, class TDecl, class TOwner, class... TArgs>
    inline static auto invoke(_In_ TReturn TDecl:: *func,
            _In_ TOwner&& owner,
            _In_ TArgs&&... args)
            -> decltype((owner->*func)(std::forward<TArgs>(args)...)) {
        return (owner->*func)(std::forward<TArgs>(args)...);
    }
};

/// <summary>
/// Specialisation for pointer-to-member of object in reference wrapper.
/// </summary>
template<bool IsRef> struct invoker<IsRef, true> {
    template<class TReturn, class TDecl, class TOwner, class... TArgs>
    inline static auto invoke(_In_ TReturn TDecl:: *func,
            _In_ TOwner&& owner,
            _In_ TArgs&&... args)
            -> decltype((owner.get().*func)(std::forward<TArgs>(args)...)) {
        return (owner.get().*func)(std::forward<TArgs>(args)...);
    }
};

/// <summary>
/// A template to derive whether <typeparamref name="TType" /> is an
/// instance of <see cref="std::reference_wrapper" />.
/// </summary>
template<class TType>
struct is_reference_wrapper final : std::false_type { };

/// <summary>
/// More specialised variant for actual
/// <see cref="std::reference_wrapper" />s.
/// </summary>
template<class TType>
struct is_reference_wrapper<std::reference_wrapper<TType>> final
    : std::true_type { };

/// <summary>
/// Answer whether <typeparam name="TArg" /> is a callable passed
/// by reference.
/// </summary>
template<class TDecl, class TArg>
struct is_ref_callable final : std::bool_constant<
    std::is_base_of_v<TDecl, std::decay_t<TArg>>> { };

/// <summary>
/// Answer whether <typeparam name="TArg" /> is a callable passed
/// in a <see cref="std::reference_wrapper" />.
/// </summary>
template<class TArg>
struct is_refwrap_callable final : std::bool_constant<
    is_reference_wrapper<std::decay_t<TArg>>::value> { };

/// <summary>
/// Invokes the given functor or pointer-to-member function with
/// <paramref name="args" />.
/// </summary>
template<class TReturn, class TDecl, class TArg, class... TArgs>
inline auto invoke(_In_ TReturn TDecl:: *func,
        _In_ TArg&& arg,
        _In_ TArgs&&... args)
    -> decltype(invoker<is_ref_callable<TDecl, TArg>::value,
        is_refwrap_callable<TArg>::value>::invoke(func,
            std::forward<TArg>(arg), std::forward<TArgs>(args)...)) {
    return invoker<is_ref_callable<TDecl, TArg>::value,
        is_refwrap_callable<TArg>::value>::invoke(
            func, std::forward<TArg>(arg), std::forward<TArgs>(args)...);
}

/// <summary>
/// Invokes the given C-style function pointer with
/// <paramref name="args" />.
/// </summary>
template<class TFunc, class... TArgs>
inline auto invoke(_In_ TFunc &&func, _In_ TArgs&&... args)
        -> decltype(func(std::forward<TArgs>(args)...)) {
    return func(std::forward<TArgs>(args)...);
}

FPD_DETAIL_NAMESPACE_END


FPD_NAMESPACE_BEGIN

/// <summary>
/// Invokes <paramref name="func" /> with the given argument list
/// <paramref name="args" />.
/// </summary>
/// <typeparam name="TFunc">The type of the function, functor or lambda to be
/// invoked.</typeparam>
/// <typeparam name="TArgs">The type of the arguments passed to
/// <paramref name="func" />.</typeparam>
/// <param name="func">The callable to be invoked. This must be a valid function
/// pointer, functor or lambda.</param>
/// <param name="args">The argument list of the call.</param>
/// <returns>Whatever <paramref name="func" /> returns when called with the
/// given argument list.</returns>
template<class TFunc, class... TArgs>
inline auto invoke(_In_ TFunc&& func, _In_ TArgs&&... args)
        -> decltype(detail::invoke(std::forward<TFunc>(func),
        std::forward<TArgs>(args)...)) {
    return detail::invoke(std::forward<TFunc>(func),
        std::forward<TArgs>(args)...);
}

FPD_NAMESPACE_END

#endif /* !defined(_FPD_INVOKE_H) */
