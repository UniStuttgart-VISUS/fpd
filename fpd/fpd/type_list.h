// <copyright file="type_list.h" company="Visualisierungsinstitut der Universität Stuttgart">
// Copyright © 2023 - 2025 Visualisierungsinstitut der Universität Stuttgart.
// Licensed under the MIT licence. See LICENCE file for details.
// </copyright>
// <author>Christoph Müller</author>

#if !defined(_FPD_TYPE_LIST_H)
#define _FPD_TYPE_LIST_H
#pragma once

#include "fpd/type_size.h"


FPD_NAMESPACE_BEGIN

/// <summary>
/// Represents a compile-time list of types.
/// </summary>
/// <typeparam name="Types">The types in the list.</typeparam>
template<class... Types> struct type_list final {
    type_list(void) = delete;
};


/// <summary>
/// Appends a new type to the given list of types.
/// </summary>
/// <typeparam name="TList">The current types in the list.</typeparam>
/// <typeparam name="TType">The type to be appended.</typeparam>
template<class TList, class TType>
struct type_list_cat;

/// <summary>
/// Specialisation for a specific type list.
/// </summary>
template<class... TTypes, class TType>
struct type_list_cat<type_list<TTypes...>, TType> final {
    typedef type_list<TTypes..., TType> type;
};


/// <summary>
/// Appends a new type to the given list of types.
/// </summary>
/// <typeparam name="TList">The current types in the list.</typeparam>
/// <typeparam name="TType">The type to be appended.</typeparam>
template<class TList, class TType>
using type_list_cat_t = typename type_list_cat<TList, TType>::type;

FPD_NAMESPACE_END


FPD_DETAIL_NAMESPACE_BEGIN

template<class TList> struct type_list_max_sizeof;

template<> struct type_list_max_sizeof<type_list<>> final {
    static constexpr const std::size_t value = 0;
};

template<class TType, class... TTypes>
struct type_list_max_sizeof<type_list<TType, TTypes...>> final {
    static constexpr const std::size_t value = (std::max)(
        type_size<TType>::value,
        type_list_max_sizeof<type_list<TTypes...>>::value);
};

FPD_DETAIL_NAMESPACE_END


FPD_NAMESPACE_BEGIN

/// <summary>
/// Answer the size of the largest type in the given list.
/// </summary>
/// <typeparam name="TList">The list to determine the size of.</typeparam>
template<class TList> struct type_list_max_sizeof {
    static constexpr const std::size_t value
        = detail::type_list_max_sizeof<TList>::value;
};

/// <summary>
/// Answer the size of the largest type in the given list.
/// </summary>
/// <typeparam name="TList">The list to determine the size of.</typeparam>
template<class TList>
constexpr std::size_t type_list_max_sizeof_v
    = type_list_max_sizeof<TList>::value;

FPD_NAMESPACE_END


FPD_DETAIL_NAMESPACE_BEGIN

template<class TList> struct type_list_size;

template<class... TTypes>
struct type_list_size<type_list<TTypes...>> final {
    static constexpr const std::size_t value = sizeof...(TTypes);
};

FPD_DETAIL_NAMESPACE_END


FPD_NAMESPACE_BEGIN

/// <summary>
/// Answer the number of elements in the given type list.
/// </summary>
/// <typeparam name="TList">The list to count the types of.</typeparam>
template<class TList> struct type_list_size {
    static constexpr const std::size_t value
        = detail::type_list_size<TList>::value;
};

/// <summary>
/// Answer the number of elements in the given type list.
/// </summary>
/// <typeparam name="TList">The list to count the types of.</typeparam>
template<class TList>
constexpr std::size_t type_list_size_v = type_list_size<TList>::value;

FPD_NAMESPACE_END


FPD_DETAIL_NAMESPACE_BEGIN

template<class TList> struct type_list_sizeof;

template<> struct type_list_sizeof<type_list<>> final {
    static constexpr const std::size_t value = 0;
};

template<class TType, class... TTypes>
struct type_list_sizeof<type_list<TType, TTypes...>> final {
    static constexpr const std::size_t value
        = sizeof(TType)
        + type_list_sizeof<type_list<TTypes...>>::value;
};

FPD_DETAIL_NAMESPACE_END


FPD_NAMESPACE_BEGIN

/// <summary>
/// Answer the aggregate size of the types in the given type list.
/// </summary>
/// <typeparam name="TList">The list to determine the size of.</typeparam>
template<class TList> struct type_list_sizeof {
    static constexpr const std::size_t value
        = detail::type_list_sizeof<TList>::value;
};

/// <summary>
/// Answer the aggregate size of the types in the given type list.
/// </summary>
/// <typeparam name="TList">The list to determine the size of.</typeparam>
template<class TList>
constexpr std::size_t type_list_sizeof_v = type_list_sizeof<TList>::value;

FPD_NAMESPACE_END

#endif /* !defined(_FPD_TYPE_LIST_H) */
