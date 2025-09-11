// <copyright file="format_taits.h" company="Visualisierungsinstitut der Universität Stuttgart">
// Copyright © 2023 - 2025 Visualisierungsinstitut der Universität Stuttgart.
// Licensed under the MIT licence. See LICENCE file for details.
// </copyright>
// <author>Christoph Müller</author>

#if !defined(_FPD_FORMAT_TRAITS_H)
#define _FPD_FORMAT_TRAITS_H
#pragma once

#include <array>
#include <functional>
#include <stdexcept>
#include <string>
#include <type_traits>

#if defined(_WIN32)
#include <dxgi.h>
#endif /* defined(_WIN32) */

#include <gl/GL.h>

#if defined(FPD_WITH_VULKAN)
#include <vulkan/vulkan.h>
#endif /* defined(FPD_WITH_VULKAN) */

#include "fpd/dependent_bool.h"
#include "fpd/detector.h"
#include "fpd/format.h"
#include "fpd/literal.h"
#include "fpd/type_collapse.h"
#include "fpd/type_components.h"
#include "fpd/type_list.h"
#include "fpd/type_size.h"


FPD_DETAIL_NAMESPACE_BEGIN

/// <summary>
/// A template that allows for retrieving compile-time information
/// about a <see cref="format" />.
/// </summary>
/// <typeparam name="Format">The format to reflect on.</typeparam>
template<format Format> struct format_forward_traits { };


/// <summary>
/// A template that allows for retrieving the <paramref name="format" />
/// of a type at compile time.
/// </summary>
/// <typeparam name="TValue">The type to reflect on.</typeparam>
template<class TValue> struct format_reverse_traits {
    typedef TValue type;
    static constexpr const format format = format::unknown;
    template<class TChar>
    static inline constexpr const TChar *name(void) noexcept {
        return FPD_TPL_LITERAL(TChar, "unknown");
    }
};


// Helper macros for constructing the DXGI and Vulkan formats.
#define __FPD_MCAT0(l, r) l ## r
#define __FPD_MCAT(l, r) __FPD_MCAT0(l, r)
#define __FPD_VEC0(b, e, d) d
#define __FPD_VEC1(b, e, d) R ## b ## _ ## e
#define __FPD_VEC2(b, e, d) R ## b ## G ## b ## _ ## e
#define __FPD_VEC3(b, e, d) R ## b ## G ## b ## B ## b ## _ ## e
#define __FPD_VEC4(b, e, d) R ## b ## G ## b ## B ## b ## A ## b ## _ ## e
#define __FPD_VECN(c, b, e, d) __FPD_VEC ## c(b, e, d)
#define __FPD_VEC_OPTE(c, b, f, d) __FPD_VECN(c, b, f, d)
#define __FPD_VEC_OPTD(c, b, f, d) __FPD_VEC0(b, f, d)
#define __FPD_CMP0(x, y, z, w) D
#define __FPD_CMP1(x, y, z, w) x
#define __FPD_CMP2(x, y, z, w) y
#define __FPD_CMP3(x, y, z, w) z
#define __FPD_CMP4(x, y, z, w) w
#define __FPL_VEC_OPT(c, x, y, z, w)\
    __FPD_MCAT(__FPD_VEC_OPT, __FPD_CMP ## c(x, y, z, w))
#define __FPD_VEC(c, b, f, d, x, y, z, w)\
    __FPL_VEC_OPT(c, x, y, z, w)(c, b, f, d)

#if defined(_WIN32)
#define __FPD_DXGI_FMT(c, b, f, x, y, z, w) __FPD_MCAT(DXGI_FORMAT_,\
    __FPD_VEC(c, b, f, UNKNOWN, x, y, z, w))
#define _FPD_DXGI_FORMAT(c, b, f, x, y, z, w)\
static constexpr const DXGI_FORMAT dxgi_format =\
    __FPD_DXGI_FMT(c, b, f, x, y, z, w);
#else /* defined(_WIN32) */
#define _FPD_DXGI_FORMAT(c, b, f, x, y, z, w)
#endif /* defined(_WIN32) */

#define _FPD_GL_FORMAT(g) static constexpr const GLenum opengl_format = g;

#if defined(FPD_WITH_VULKAN)
#define __FPD_VK_FMT(c, b, f, x, y, z, w) __FPD_MCAT(VK_FORMAT_,\
    __FPD_VEC(c, b, f, UNDEFINED, x, y, z, w))
#define _FPD_VK_FORMAT(c, b, f, x, y, z, w)\
static constexpr const VkFormat vulkan_format =\
    __FPD_VK_FMT(c, b, f, x, y, z, w);
#else /* defined(FPD_WITH_VULKAN) */
#define _FPD_VK_FORMAT(c, b, f, x, y, z, w)
#endif /* defined(FPD_WITH_VULKAN) */


#define _FPD_SCA_TRAITS(f, t, b, d, d1, d2, d3, d4, g,v, v1, v2, v3, v4)\
template<> struct format_forward_traits<format::f> {\
    typedef t scalar;\
    typedef t type;\
    _FPD_DXGI_FORMAT(1, b, d, d1, d2, d3, d4)\
    static constexpr const format format = format::f;\
    _FPD_GL_FORMAT(g)\
    _FPD_VK_FORMAT(1, b, v, v1, v2, v3, v4)\
    template<class TChar>\
    static inline constexpr const TChar *name(void) noexcept {\
        return FPD_TPL_LITERAL(TChar, #f);\
    }\
};\
template<> struct format_reverse_traits<t> {\
    typedef t scalar;\
    typedef t type;\
    _FPD_DXGI_FORMAT(1, b, d, d1, d2, d3, d4)\
    static constexpr const format format = format::f;\
    _FPD_GL_FORMAT(g)\
    _FPD_VK_FORMAT(1, b, v, v1, v2, v3, v4)\
    template<class TChar>\
    static inline constexpr const TChar *name(void) noexcept {\
        return FPD_TPL_LITERAL(TChar, #f);\
    }\
}

#define _FPD_VEC_TRAITS(f, t, b, c, s, d, d1, d2, d3, d4, g, v, v1, v2, v3, v4)\
template<> struct format_forward_traits<format::f##s> {\
    typedef t scalar;\
    typedef std::array<t, c> type;\
    _FPD_DXGI_FORMAT(c, b, d, d1, d2, d3, d4)\
    static constexpr const format format = format::f ## s;\
    _FPD_GL_FORMAT(g)\
    _FPD_VK_FORMAT(c, b, v, v1, v2, v3, v4)\
    template<class TChar>\
    static inline constexpr const TChar *name(void) noexcept {\
        return FPD_TPL_LITERAL(TChar, #f #s); \
    }\
};\
template<> struct format_reverse_traits<std::array<t, c>> {\
    typedef t scalar;\
    typedef std::array<t, c> type;\
    _FPD_DXGI_FORMAT(c, b, d, d1, d2, d3, d4)\
    static constexpr const format format = format::f ## s;\
    _FPD_GL_FORMAT(g)\
    _FPD_VK_FORMAT(c, b, v, v1, v2, v3, v4)\
    template<class TChar>\
    static inline constexpr const TChar *name(void) noexcept {\
        return FPD_TPL_LITERAL(TChar, #f #s); \
    }\
}

#define _FPD_FMT_TRAITS(f, t, b, d, d1, d2, d3, d4, g, v, v1, v2, v3, v4)\
_FPD_SCA_TRAITS(f##x, t, b, d, d1, d2, d3, d4, g, v, v1, v2, v3, v4);\
_FPD_VEC_TRAITS(f, t, b, 2, xy, d, d1, d2, d3, d4, g, v, v1, v2, v3, v4);\
_FPD_VEC_TRAITS(f, t, b, 3, xyz, d, d1, d2, d3, d4, g, v, v1, v2, v3, v4);\
_FPD_VEC_TRAITS(f, t, b, 4, xyzw, d, d1, d2, d3, d4, g, v, v1, v2, v3, v4)

_FPD_SCA_TRAITS(unknown, void, 0, 0, D, D, D, D, GL_NONE, 0, D, D, D, D);
_FPD_FMT_TRAITS(int8, std::int8_t, 8, SINT, E, E, D, E, GL_BYTE, SINT, E, E, E, E);
_FPD_FMT_TRAITS(uint8, std::uint8_t, 8, UINT, E, E, D, E, GL_UNSIGNED_BYTE, UINT, E, E, E, E);
_FPD_FMT_TRAITS(int16, std::int16_t, 16, SINT, E, E, D, E, GL_SHORT, SINT, E, E, E, E);
_FPD_FMT_TRAITS(uint16, std::uint16_t, 16, UINT, E, E, D, E, GL_UNSIGNED_SHORT, UINT, E, E, E, E);
_FPD_FMT_TRAITS(int32, std::int32_t, 32, SINT, E, E, E, E, GL_INT, SINT, E, E, E, E);
_FPD_FMT_TRAITS(uint32, std::uint32_t, 32, UINT, E, E, E, E, GL_UNSIGNED_INT, UINT, E, E, E, E);
_FPD_FMT_TRAITS(int64, std::int64_t, 64, SINT, D, D, D, D, GL_NONE, SINT, E, E, E, E);
_FPD_FMT_TRAITS(uint64, std::uint64_t, 64, UINT, D, D, D, D, GL_NONE, UINT, E, E, E, E);
_FPD_FMT_TRAITS(float32, float, 32, FLOAT, E, E, E, E, GL_FLOAT, SFLOAT, E, E, E, E);
_FPD_FMT_TRAITS(float64, double, 64, FLOAT, D, D, D, D, GL_DOUBLE, SFLOAT, E, E, E, E);

//static_assert(detector_v<_vk_has_float32x, VkFormat>, "bla222");
//static_assert(detector_v<dxgi_has_float32x, DXGI_FORMAT>, "bla222");


/// <summary>
/// Resolves the scalar type used by a specified format.
/// </summary>
template<format Format>
using scalar_t = typename format_forward_traits<Format>::scalar;


/// <summary>
/// Resolves the type of a specified format (forward resolution).
/// </summary>
template<format Format>
using format_t = typename format_forward_traits<Format>::type;


/// <summary>
/// Resolves the format of a specified type (reverse resolution).
/// </summary>
template<class TValue>
constexpr auto format_v = format_reverse_traits<TValue>::format;


/// <summary>
/// Filter a <see cref="format_list" /> by a <typeparamref name="TPredicate" />
/// on the type resolved by <see cref="format_forward_traits" />.
/// </summary>
template<class TList, template<class> class TPredicate>
struct filter_format_list;

/// <summary>
/// Recursion stop for <see cref="filter_format_list" />.
/// </summary>
template<template<class> class TPredicate>
struct filter_format_list<format_list<>, TPredicate> final {
    typedef format_list<> type;
};

/// <summary>
/// Specialisation of <paramref name="filter_format_list" /> for actual type
/// lists.
/// </summary>
template<template<class> class TPredicate, format Format, format... Formats>
struct filter_format_list<format_list<Format, Formats...>, TPredicate> final {
    typedef filter_format_list<format_list<Formats...>, TPredicate> tail;
    typedef format_forward_traits<Format> traits;
    typedef typename std::conditional<
        TPredicate<typename traits::type>::value,
        format_list_cat_t<typename tail::type, Format>,
        typename tail::type>::type type;
};


/// <summary>
/// Resolves the format with the specified scalar type and component count.
/// </summary>
template<class TScalar, std::size_t Components>
struct format_from_type final {
    typedef format_reverse_traits<std::array<TScalar, Components>> traits;
    static constexpr const format value = traits::format;
};

/// <summary>
/// Specialisation for scalar formats.
/// </summary>
template<class TScalar>
struct format_from_type<TScalar, 1> final {
    typedef format_reverse_traits<TScalar> traits;
    static constexpr const format value = traits::format;
};

/// <summary>
/// Resolves the format with the specified scalar type and component count.
/// </summary>
template<class TScalar, std::size_t Components>
constexpr auto format_from_type_v
    = format_from_type<TScalar, Components>::value;


/// <summary>
/// Defines a list of formats that can be expanded to dynamically dispatch to
/// a per-format handler.
/// </summary>
template<class TList> struct format_type_list;

/// <summary>
/// Specialisation for actual lists of formats.
/// </summary>
template<format... Formats>
struct format_type_list<format_list<Formats ...>> final {
    template<format F> using traits = format_t<F>;
    typedef type_list<traits<Formats>...> type;
};


/// <summary>
/// Recursion stop for <see cref="index_of" />.
/// </summary>
constexpr inline std::size_t index_of(
        _In_ const format,
        _In_ format_list<>,
        _In_ const std::size_t offset) noexcept {
    return offset;
}

/// <summary>
/// Find the position of <paramref name="format" /> in the given format list.
/// </summary>
template<format Format, format... Formats>
constexpr inline std::size_t index_of(
        _In_ const format format,
        _In_ format_list<Format, Formats...>,
        _In_ const std::size_t offset) noexcept {
    if (format == Format) {
        return offset;
    } else {
        return index_of(format, format_list<Formats...>(), offset + 1);
    }
}


/// <summary>
/// Checks whether <typeparamref name="Format" /> is in the given list of
/// formats.
/// </summary>
template<class TList, format Format> struct in_format_list;

/// <summary>
/// Checks whether the <typeparamref name="Head" /> of the list is the
/// <typeparamref name="Needle" /> we are looking for.
/// </summary>
template<format Head, format... Tail, format Needle>
struct in_format_list<format_list<Head, Tail...>, Needle> final {
    static constexpr const bool value = (Head == Needle)
        || in_format_list<format_list<Tail...>, Needle>::value;
};

/// <summary>
/// Recursion stop for the case where <typeparamref name="Needle" /> was not
/// found.
/// </summary>
template<format Needle>
struct in_format_list<format_list<>, Needle> final : std::false_type { };


/// <summary>
/// Checks whether <typeparamref name="Format" /> is in the given list of
/// formats.
/// </summary>
template<class TList, format Format>
constexpr bool in_format_list_v = in_format_list<TList, Format>::value;


/// <summary>
/// Dispatches a runtime-defined format to a callback template.
/// </summary>
template<class TReturn, class TCallback, format... Formats>
TReturn dispatch_format(_In_ const format format,
        _In_ const TCallback& callback,
        _In_ const format_list<Formats...>) {
    typedef std::function<TReturn()> dispatch_type;
    const std::array<dispatch_type, sizeof...(Formats)> Dispatchers = {
            ([&callback](void) {
                typedef typename format_forward_traits<Formats>::type *tag;
                return callback(static_cast<tag>(nullptr));
            })...
        };

    const auto idx = index_of(format, format_list<Formats...>(), 0);
    if (idx >= Dispatchers.size()) {
        throw std::logic_error("The specified format is not in the list "
            "of supported formats. Please ensure that the format is a valid "
            "enumeration member and that all enumeration members are "
            "registered in the formats list.");
    }

    return Dispatchers[idx]();
}


/// <summary>
/// Recursion stop for <see cref="is_floating_point" />.
/// </summary>
inline constexpr bool is_floating_point(_In_ const format_list<>,
        _In_ const format) noexcept {
    return false;
}

/// <summary>
/// Determines whether a format uses floating-point scalars.
/// </summary>
template<format Format, format... Formats>
inline constexpr std::size_t is_floating_point(
        _In_ const format_list<Format, Formats...>,
        _In_ const format format) noexcept {
    if (format == Format) {
        return std::is_floating_point_v<scalar_t<Format>>;
    } else {
        return is_floating_point(format_list<Formats...>(), format);
    }
}


/// <summary>
/// Recursion stop for <see cref="make_format" />.
/// </summary>
inline constexpr format make_format(
        _In_ const format_list<>,
        _In_ const format,
        _In_ const std::size_t) noexcept {
    return format::unknown;
}

/// <summary>
/// Creates a new format using the same scalar type as
/// <paramref name="format" />, but another number of
/// <paramref name="components" />.
/// </summary>
template<format Format, format... Formats>
inline constexpr format make_format(
        _In_ const format_list<Format, Formats...>,
        _In_ const format format,
        _In_ const std::size_t components) noexcept {
    if (format == Format) {
        typedef scalar_t<Format> value_type;

        switch (components) {
            case 1: return format_from_type_v<value_type, 1>;
            case 2: return format_from_type_v<value_type, 2>;
            case 3: return format_from_type_v<value_type, 3>;
            case 4: return format_from_type_v<value_type, 4>;
            default: return format::unknown;
        }

    } else {
        return make_format(format_list<Formats...>(), format, components);
    }
}


/// <summary>
/// Recursion stop for <see cref="to_components" />.
/// </summary>
inline constexpr std::size_t to_components(_In_ const format_list<>,
        _In_ const format) noexcept {
    return 0;
}

/// <summary>
/// Determines the number of components of a format.
/// </summary>
template<format Format, format... Formats>
inline constexpr std::size_t to_components(
        _In_ const format_list<Format, Formats...>,
        _In_ const format format) noexcept {
    if (format == Format) {
        return type_components<format_t<Format>>::value;
    } else {
        return to_components(format_list<Formats...>(), format);
    }
}


#if defined(_WIN32)
/// <summary>
/// Recursion stop for <see cref="to_dxgi" />.
/// </summary>
inline constexpr DXGI_FORMAT to_dxgi(_In_ const format_list<>,
        _In_ const format) noexcept {
    return DXGI_FORMAT_UNKNOWN;
}

/// <summary>
/// Determines the DXGI format for the given <paramref name="format" />.
/// </summary>
template<format Format, format... Formats>
inline constexpr DXGI_FORMAT to_dxgi(
        _In_ const format_list<Format, Formats...>,
        _In_ const format format) noexcept {
    if (format == Format) {
        return format_forward_traits<Format>::dxgi_format;
    } else {
        return to_dxgi(format_list<Formats...>(), format);
    }
}
#endif /* defined(_WIN32) */

/// <summary>
/// Recursion stop for <see cref="to_floating_point" />.
/// </summary>
inline format to_floating_point(_In_ const format_list<>, _In_ const format) {
    throw std::logic_error("The specified format is unknown. Please ensure "
        "that the format is a valid enumeration member and that all "
        "enumeration members are registered in the formats list.");
}

/// <summary>
/// Determines a similarly sized floating-point format for the given format.
/// </summary>
template<format Format, format... Formats>
inline format to_floating_point(
        _In_ const format_list<Format, Formats...>,
        _In_ const format format) {
    if (format == Format) {
        typedef scalar_t<Format> value_type;

        if (std::is_floating_point_v<value_type>) {
            // The format is already a floating-point format, so we can use it
            // directly.
            return Format;

        } else {
            // If we have an integral type, use float for everything up to its
            // size and double otherwise.
            if FPD_IF_CONSTEXPR (sizeof(value_type) <= sizeof(float)) {
                return format_from_type_v<float,
                    type_components<format_t<Format>>::value>;
            } else {
                return format_from_type_v<double,
                    type_components<format_t<Format>>::value>;
            }
        }

    } else {
        return to_floating_point(format_list<Formats...>(), format);
    }
}


/// <summary>
/// Recursion stop for <see cref="to_opengl" />.
/// </summary>
inline constexpr GLenum to_opengl(_In_ const format_list<>,
        _In_ const format) noexcept {
    return GL_NONE;
}

/// <summary>
/// Determines the OpenGL scalar format for the given
/// <paramref name="format" />.
/// </summary>
template<format Format, format... Formats>
inline constexpr GLenum to_opengl(
        _In_ const format_list<Format, Formats...>,
        _In_ const format format) noexcept {
    if (format == Format) {
        return format_forward_traits<Format>::opengl_format;
    } else {
        return to_opengl(format_list<Formats...>(), format);
    }
}


/// <summary>
/// Recursion stop for <see cref="to_scalar_size" />.
/// </summary>
inline constexpr std::size_t to_scalar_size(_In_ const format_list<>,
        _In_ const format) {
    return 0;
}

/// <summary>
/// Determines the size of a scalar of the given <paramref name="format" />.
/// </summary>
template<format Format, format... Formats>
inline constexpr std::size_t to_scalar_size(
        _In_ const format_list<Format, Formats...>,
        _In_ const format format) {
    if (format == Format) {
        return type_size<scalar_t<Format>>::value;
    } else {
        return to_scalar_size(format_list<Formats...>(), format);
    }
}


/// <summary>
/// Recursion stop for <see cref="to_size" />.
/// </summary>
inline constexpr std::size_t to_size(_In_ const format_list<>,
        _In_ const format) {
    return 0;
}

/// <summary>
/// Determines the size of the given <paramref name="format" />.
/// </summary>
template<format Format, format... Formats>
inline constexpr std::size_t to_size(
        _In_ const format_list<Format, Formats...>,
        _In_ const format format) {
    if (format == Format) {
        return type_size<format_t<Format>>::value;
    } else {
        return to_size(format_list<Formats...>(), format);
    }
}


/// <summary>
/// Recursion stop for <see cref="to_string" />.
/// </summary>
template<class TChar> inline const TChar *const to_string(
        _In_ const format_list<>,
        _In_ const format) {
    throw std::logic_error("The specified format could not be converted to "
        "a string. Please ensure that the format is a valid enumeration "
        "member and that all enumeration members are registered in the "
        "formats list.");
}

/// <summary>
/// Resolves the human-readable description of the given
/// <paramref name="format" />.
/// </summary>
template<class TChar, format Format, format... Formats>
inline const TChar *const to_string(
        _In_ const format_list<Format, Formats...>,
        _In_ const format format) {
    if (format == Format) {
        return format_forward_traits<Format>::template name<TChar>();
    } else {
        return to_string<TChar>(format_list<Formats...>(), format);
    }
}


#if defined(FPD_WITH_VULKAN)
/// <summary>
/// Recursion stop for <see cref="to_vulkan" />.
/// </summary>
inline constexpr VkFormat to_vulkan(_In_ const format_list<>,
        _In_ const format) noexcept {
    return VK_FORMAT_UNDEFINED;
}

/// <summary>
/// Determines the Vulkan format for the given <paramref name="format" />.
/// </summary>
template<format Format, format... Formats>
inline constexpr VkFormat to_vulkan(
        _In_ const format_list<Format, Formats...>,
        _In_ const format format) noexcept {
    if (format == Format) {
        return format_forward_traits<Format>::vulkan_format;
    } else {
        return to_vulkan(format_list<Formats...>(), format);
    }
}
#endif /* defined(FPD_WITH_VULKAN) */

FPD_DETAIL_NAMESPACE_END


FPD_NAMESPACE_BEGIN

/// <summary>
/// A list of all the types that are represented by the <see cref="format" />s
/// in the <see cref="formats" /> list.
/// </summary>
typedef typename detail::format_type_list<formats>::type format_types;


/// <summary>
/// Answer whether <typeparamref name="Format" /> is in the given
/// <typeparamref name="TList" /> of formats.
/// </summary>
/// <typeparam name="TList"></typeparam>
/// <typeparam name="Format"></typeparam>
template<class TList, format Format> struct is_supported_format final
    : std::bool_constant<detail::in_format_list_v<TList, Format>> { };


/// <summary>
/// Answer whether the <see cref="format" /> of <typeparamref name="TValue" />
/// is in the given <typeparamref name="TList" /> of formats.
/// </summary>
/// <typeparam name="TList"></typeparam>
/// <typeparam name="TValue"></typeparam>
template<class TList, class TValue> struct is_supported_type final
    : std::bool_constant<detail::in_format_list_v<TList,
    detail::format_v<std::decay_t<TValue>>>> { };


/// <summary>
/// A filtered format list that only holds scalar formats.
/// </summary>
typedef typename detail::filter_format_list<formats, std::is_arithmetic>::type
    scalar_formats;


/// <summary>
/// Dispatches a runtime-defined <paramref name="format" /> to a callback
/// template that receives a pointer to the type used by the format as
/// parameter.
/// </summary>
/// <typeparam name="TReturn">The return type of the callback.</typeparam>
/// <typeparam name="TCallback">The callback template. This must be a lambda
/// with a single <see langword="auto" /> parameter that will be resolved to a
/// pointer to the type representing values of <paramref name="format" />.
/// </typeparam>
/// <param name="format">The format to dispatch to.</param>
/// <param name="callback">The callback to be invoked.</param>
/// <returns>The value returned by the callback.</returns>
/// <exception cref="std::logic_error">If an invalid <paramref name="format" />
/// was provided or if the type traits have not been setup properly. The latter
/// is a bug in the implementation of the library.</exception>
template<class TReturn, class TCallback>
inline TReturn dispatch_format(_In_ const format format,
        _In_ const TCallback callback) {
    return detail::dispatch_format<TReturn>(format, callback, formats());
}


/// <summary>
/// Answer whether the given <paramref name="format" /> uses floating-point
/// scalar values.
/// </summary>
/// <param name="format">The format to check for floating-point scalars.</param>
/// <returns><see langword="true "/> if the format is a floating-point format,
/// <see langword="false" /> otherwise.</returns>
inline constexpr bool is_floating_point(_In_ const format format) noexcept {
    return detail::is_floating_point(formats(), format);
}


/// <summary>
/// Creates a format using the same scalar type as <parmaref name="scalar" />,
/// but which has the given number of <paramref name="components" />.
/// </summary>
/// <param name="scalar">The scalar format to be used. Note that this can be a
/// vector format, too, in which case the format of a component will be used.
/// </param>
/// <param name="components">The number of components of the new format, which
/// must be within [1, 4].</param>
/// <returns>A new format with the specified number of components. If the
/// resulting format would be invalid (for instance vectors with zero components
/// or more than four), the result will be <see cref="format::unknonw" />.
/// </returns>
inline constexpr format make_format(_In_ const format scalar,
        _In_ const std::size_t components) noexcept {
    return detail::make_format(formats(), scalar, components);
}


/// <summary>
/// Answer a runtime list of the given compile-time list of formats.
/// </summary>
/// <typeparam name="Formats">A list of formats.</param>
/// <returns>A runtime list containing <typeparamref name="Formats" />.
/// </returns>
template<format... Formats>
constexpr inline std::array<format, sizeof...(Formats)> to_array(
        _In_ const format_list<Formats...>) noexcept {
    return std::array<format, sizeof...(Formats)> { Formats... };
}


/// <summary>
/// Answer the number of components the given format has.
/// </summary>
/// <param name="format">The format to determine the number of components
/// of.</param>
/// <returns>The number of components used by the given format.</returns>
inline constexpr std::size_t to_components(_In_ const format format) noexcept {
    return detail::to_components(formats(), format);
}


#if defined(_WIN32)
/// <summary>
/// Determines the matching DXGI format for <paramref name="format" />.
/// </summary>
/// <param name="format">The format to determine the DXGI format for.</param>
/// <returns>The matching buffer format or <c>DXGI_FORMAT_UNKNOWN</c> if no
/// matching format could be found.</returns>
inline constexpr DXGI_FORMAT to_dxgi(_In_ const format format) noexcept {
    return detail::to_dxgi(formats(), format);
}
#endif /* defined(_WIN32) */


/// <summary>
/// If <paramref name="format" /> does not use a floating-point scalar type,
/// create a similarly sized format using floating-point scalars.
/// </summary>
/// <param name="format">The format to create a similarly sized floating-point
/// format for.</param>
/// <returns>A floating-point format.</returns>
inline format to_floating_point(_In_ const format format) {
    return detail::to_floating_point(formats(), format);
}


/// <summary>
/// Make a similarly sized floating-point format as <paramref name="format" />
/// with the specified number of components.
/// </summary>
/// <param name="format">The format to create a similarly sized floating-point
/// format for.</param>
/// <param name="components">The number of components the resulting format will
/// have, which must be within [1, 4].</param>
/// <returns>The floating-point vector format if it is supported, or
/// <see cref="format::unknown" /> if the number of components are illegal.
/// </returns>
inline format to_floating_point(_In_ const format format,
        _In_ const std::size_t components) {
    return make_format(to_floating_point(format), components);
}


/// <summary>
/// Determines the matching OpenGL scalar format for <paramref name="format" />.
/// </summary>
/// <remarks>
/// Note that OpenGL does not use vector formats like DXGI or Vulkan, so you
/// need to determine the number of components separately using
/// <see cref="to_components" />.
/// </remarks>
/// <param name="format">The format to determine the OpenGL format for.</param>
/// <returns>The matching scalar format or <c>GL_NONE</c> if no matching format
/// could be found.</returns>
inline constexpr GLenum to_opengl(_In_ const format format) noexcept {
    return detail::to_opengl(formats(), format);
}


/// <summary>
/// Answer the size in bytes of a scalar of the given
/// <paramref name="format" />.
/// </summary>
/// <remarks>
/// If <paramref name="format" /> is a scalar type, this is equivalent to
/// calling <see cref="to_size" />. Otherwise, the function will return the
/// size of a single component.
/// </remarks>
/// <param name="format">The format to get the scalar size of.</param>
/// <returns>The size of the scalar in bytes.</returns>
inline constexpr std::size_t to_scalar_size(_In_ const format format) {
    typedef format_list_cat_t<formats, format::unknown> dispatch_type;
    return detail::to_scalar_size(dispatch_type(), format);
}


/// <summary>
/// Answer the size in bytes of the given <paramref name="format" />.
/// </summary>
/// <param name="format">The format to get the size of.</param>
/// <returns>The size of the format in bytes.</returns>
inline constexpr std::size_t to_size(_In_ const format format) {
    typedef format_list_cat_t<formats, format::unknown> dispatch_type;
    return detail::to_size(dispatch_type(), format);
}


/// <summary>
/// Answer a human-readable version of the given <paramref name="format" />.
/// </summary>
/// <typeparam name="TChar">The character type of the string to create.
/// </typeparam>
/// <param name="format">The format to convert to a string.</param>
/// <returns>The string representation of the given format.</returns>
/// <exception cref="std::logic_error">If an invalid <paramref name="format" />
/// was provided or if the type traits have not been setup properly. The latter
/// is a bug in the implementation of the library.</exception>
template<class TChar>
inline const TChar *const to_string(_In_ const format format) {
    typedef format_list_cat_t<formats, format::unknown> dispatch_type;
    return detail::to_string<TChar>(dispatch_type(), format);
}


#if defined(FPD_WITH_VULKAN)
/// <summary>
/// Determines the matching Vulkan format for <paramref name="format" />.
/// </summary>
/// <param name="format">The format to determine the Vulkan format for.</param>
/// <returns>The matching buffer format or <c>VK_FORMAT_UNDEFINED</c> if no
/// matching format could be found.</returns>
inline constexpr VkFormat to_vulkan(_In_ const format format) noexcept {
    return detail::to_vulkan(formats(), format);
}
#endif /* defined(FPD_WITH_VULKAN) */

FPD_NAMESPACE_END

#endif /* !defined(_FPD_FORMAT_TRAITS_H) */
