// <copyright file="invoke.h" company="Visualisierungsinstitut der Universität Stuttgart">
// Copyright © 2024 Visualisierungsinstitut der Universität Stuttgart.
// Licensed under the MIT licence. See LICENCE file for details.
// </copyright>
// <author>Christoph Müller</author>

#pragma once

#include <functional>
#include <type_traits>

#include "fpd/abi.h"
#include "fpd/annotations.h"


FPD_NAMESPACE_BEGIN

namespace detail {

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
                -> decltype(owner.*func(std::forward<TArgs>(args)...)) {
            return owner.*func(std::forward<TArgs>(args)...);
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
                -> decltype(owner->*func(std::forward<TArgs>(args)...)) {
            return owner->*func(std::forward<TArgs>(args)...);
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
                -> decltype(owner.get().*func(std::forward<TArgs>(args)...)) {
            return owner.get().*func(std::forward<TArgs>(args)...);
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
    /// Invokes the given functor or pointer-to-member function with
    /// <paramref name="args" />.
    /// </summary>
    template<class TReturn, class TDecl, class TArg, class... TArgs>
    inline auto invoke(_In_ TReturn TDecl:: *func, _In_ TArg&& arg,
            _In_ TArgs&&... args) 
        -> decltype(invoker<
            std::is_base_of<TDecl, typename std::decay<TArg>::type>::value,
            is_reference_wrapper<typename std::decay<TArg>::type>::value,
            std::is_function<decltype(func)>::value>::invoke(func,
                std::forward<TArg>(arg),
                std::forward<TArgs>(args)...)) {
        return invoker<
            std::is_base_of<TDecl, typename std::decay<TArg>::type>::value,
            is_reference_wrapper<typename std::decay<TArg>::type>::value,
            std::is_function<decltype(func)>::value>::invoke(func,
                std::forward<TArg>(arg),
                std::forward<TArgs>(args)...);
    }

    /// <summary>
    /// Invokes the given C-style function pointer with
    /// <paramref name="args" />.
    /// </summary>
    template<class TFunc, class... TArgs>
    inline auto invoke(_In_ TFunc&& func, _In_ TArgs&&... args)
            -> decltype(func(std::forward<TArgs>(args)...)) {
        return func(std::forward<TArgs>(args)...);
    }

} /* namespace detail */


/// <summary>
/// Invokes <paramref name="func" /> with the given argument list
/// <paramref name="args" />.
/// </summary>
/// <typeparam name="TFunc"></typeparam>
/// <typeparam name="TArgs"></typeparam>
/// <param name="func"></param>
/// <param name="args"></param>
/// <returns></returns>
template<class TFunc, class... TArgs>
inline auto invoke(_In_ TFunc&& func, _In_ TArgs&&... args)
        -> decltype(detail::invoke(std::forward<TFunc>(func),
        std::forward<TArgs>(args)...)) {
    return detail::invoke(std::forward<TFunc>(func),
        std::forward<TArgs>(args)...);
}

FPD_NAMESPACE_END
