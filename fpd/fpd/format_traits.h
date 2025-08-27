// <copyright file="format_taits.h" company="Visualisierungsinstitut der Universität Stuttgart">
// Copyright © 2023 - 2025 Visualisierungsinstitut der Universität Stuttgart.
// Licensed under the MIT licence. See LICENCE file for details.
// </copyright>
// <author>Christoph Müller</author>

#if !defined(_FPD_FORMAT_TRAITS_H)
#define _FPD_FORMAT_TRAITS_H
#pragma once

#include <array>
#include <stdexcept>
#include <string>

#if defined(_WIN32)
#include <dxgi.h>
#endif /* defined(_WIN32) */

#include <gl/GL.h>

#if defined(FPD_WITH_VULKAN)
#include <vulkan/vulkan.h>
#endif /* defined(FPD_WITH_VULKAN) */

#include "fpd/format.h"
#include "fpd/literal.h"
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
#define __FPD_MEXP(x) x
#define __FPD_MTER0(t, f) f
#define __FPD_MTER1(t, f) t
#define __FPD_MTER2(t, f) t
#define __FPD_MTER3(t, f) t
#define __FPD_MTER4(t, f) t
#define __FPD_MTER(c, t, f) __FPD_MTER ## c(t, f)
#define __FPD_SUFFIX(s) _ ## s

#define __RGBA_VEC0(w, f) f
#define __RGBA_VEC1(w, f) R ## w ## _ ## f
#define __RGBA_VEC2(w, f) R ## w ## G ## w ## _ ## f
#define __RGBA_VEC3(w, f) R ## w ## G ## w ## B ## w ## _ ## f
#define __RGBA_VEC4(w, f) R ## w ## G ## w ## B ## w ## A ## w ## _ ## f
#define __RGBA_VEC(c, w, f) __RGBA_VEC ## c(w, f)

#if defined(_WIN32)
#define __FPD_DXGI_SFX(c, t) __FPD_MTER(c, t, UNKNOWN)
#define __FPD_DXGI_VEC(c, w, t) __RGBA_VEC(c, w, __FPD_DXGI_SFX(c, t))
#define __FPD_DXGI_FMT(c, w, t) __FPD_MCAT(DXGI_FORMAT_, __FPD_DXGI_VEC(c, w, t))
#define _FPD_DXGI_FORMAT(c, w, t) static constexpr const DXGI_FORMAT \
    dxgi_format = DXGI_FORMAT::__FPD_DXGI_FMT(c, w, t);
#else /* defined(_WIN32) */
#define _FPD_DXGI_FORMAT(f, c, w)
#endif /* defined(_WIN32) */

#define _FPD_GL_FORMAT(g) static constexpr const GLenum opengl_format = g;

#if defined(FPD_WITH_VULKAN)
#define __FPD_VK_SFX(c, t) __FPD_MTER(c, t, UNDEFINED)
#define __FPD_VK_VEC(c, w, t) __RGBA_VEC(c, w, __FPD_VK_SFX(c, t))
#define __FPD_VK_FMT(c, w, t) __FPD_MCAT(VK_FORMAT_, __FPD_VK_VEC(c, w, t))
#define _FPD_VK_FORMAT(c, w, t) static constexpr const VkFormat \
    vulkan_format = VkFormat::__FPD_VK_FMT(c, w, t);
#else /* defined(FPD_WITH_VULKAN) */
#define _FPD_VK_FORMAT(c, w, t)
#endif /* defined(FPD_WITH_VULKAN) */

#define _SPECIALISE_SCALAR_FMT_TRAITS(f, t, b, df, dc, g, vf, vc)\
template<> struct format_forward_traits<format::f> {\
    typedef t scalar;\
    typedef t type;\
    _FPD_DXGI_FORMAT(dc, b, df)\
    static constexpr const format format = format::f;\
    _FPD_GL_FORMAT(g)\
    _FPD_VK_FORMAT(vc, b, vf)\
    template<class TChar>\
    static inline constexpr const TChar *name(void) noexcept {\
        return FPD_TPL_LITERAL(TChar, #f);\
    }\
};\
template<> struct format_reverse_traits<t> {\
    typedef t scalar;\
    typedef t type;\
    _FPD_DXGI_FORMAT(dc, b, df)\
    static constexpr const format format = format::f;\
    _FPD_GL_FORMAT(g)\
    _FPD_VK_FORMAT(vc, b, vf)\
    template<class TChar>\
    static inline constexpr const TChar *name(void) noexcept {\
        return FPD_TPL_LITERAL(TChar, #f);\
    }\
}

// TODO: need to filter out unsupported DXGI 3-component formats.
#define _SPECIALISE_VEC_FMT_TRAITS(f, s, t, b, d, df, dc, g, vf, vc)\
template<> struct format_forward_traits<format::f##s> {\
    typedef t scalar;\
    typedef std::array<t, d> type;\
    /*_FPD_DXGI_FORMAT(__FPD_MTER(dc, d, 0), b, df)*/\
    static constexpr const format format = format::f##s;\
    _FPD_GL_FORMAT(g)\
    _FPD_VK_FORMAT(__FPD_MTER(vc, d, 0), b, vf)\
    template<class TChar>\
    static inline constexpr const TChar *name(void) noexcept {\
        return FPD_TPL_LITERAL(TChar, #f#s); \
    }\
};\
template<> struct format_reverse_traits<std::array<t, d>> {\
    typedef t scalar;\
    typedef std::array<t, d> type;\
    /*_FPD_DXGI_FORMAT(__FPD_MTER(dc, d, 0), b, df)*/\
    static constexpr const format format = format::f##s;\
    _FPD_GL_FORMAT(g)\
    _FPD_VK_FORMAT(__FPD_MTER(vc, d, 0), b, vf)\
    template<class TChar>\
    static inline constexpr const TChar *name(void) noexcept {\
        return FPD_TPL_LITERAL(TChar, #f#s); \
    }\
}

#define _SPECIALISE_FMT_TRAITS(f, t, b, df, dc, g, vf, vc)\
_SPECIALISE_SCALAR_FMT_TRAITS(f##x, t, b, df, dc, g, vf, vc);\
_SPECIALISE_VEC_FMT_TRAITS(f, xy, t, b, 2, df, dc, g, vf, vc);\
_SPECIALISE_VEC_FMT_TRAITS(f, xyz, t, b, 3, df, dc, g, vf, vc);\
_SPECIALISE_VEC_FMT_TRAITS(f, xyzw, t, b, 4, df, dc, g, vf, vc)

_SPECIALISE_SCALAR_FMT_TRAITS(unknown, void, 0,
    UNKNOWN, 0,
    GL_NONE,
    UNDEFINED, 0);
_SPECIALISE_FMT_TRAITS(int8, std::int8_t, 8,
    SINT, 1,
    GL_BYTE,
    SINT, 1);
_SPECIALISE_FMT_TRAITS(uint8, std::uint8_t, 8,
    UINT, 1,
    GL_UNSIGNED_BYTE,
    UINT, 1);
_SPECIALISE_FMT_TRAITS(int16, std::int16_t, 16,
    SINT, 1,
    GL_SHORT,
    SINT, 1);
_SPECIALISE_FMT_TRAITS(uint16, std::uint16_t, 16,
    UINT, 1,
    GL_UNSIGNED_SHORT,
    UINT, 1);
_SPECIALISE_FMT_TRAITS(int32, std::int32_t, 32,
    SINT, 1,
    GL_INT,
    SINT, 1);
_SPECIALISE_FMT_TRAITS(uint32, std::uint32_t, 32,
    UINT, 1,
    GL_UNSIGNED_INT,
    UINT, 1);
_SPECIALISE_FMT_TRAITS(int64, std::int64_t, 64,
    SINT, 0,
    GL_NONE,
    SINT, 1);
_SPECIALISE_FMT_TRAITS(uint64, std::uint64_t, 64,
    UINT, 0,
    GL_NONE,
    UINT, 1);
_SPECIALISE_FMT_TRAITS(float32, float, 32,
    FLOAT, 1,
    GL_FLOAT,
    SFLOAT, 1);
_SPECIALISE_FMT_TRAITS(float64, double, 64,
    FLOAT, 0,
    GL_DOUBLE,
    SFLOAT, 1);

#undef _SPECIALISE_FMT_TRAITS
#undef _SPECIALISE_VEC_FMT_TRAITS
#undef _SPECIALISE_SCALAR_FMT_TRAITS


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


/// <summary>
/// Recursion stop for <see cref="to_scalar_size" />.
/// </summary>
inline std::size_t to_scalar_size(_In_ const format_list<>,
        _In_ const format) {
    throw std::logic_error("The size of the specified format could not be "
        "derived. Please ensure that the format is a valid enumeration "
        "member and that all enumeration members are registered in the "
        "formats list.");
}

/// <summary>
/// Determines the size of a scalar of the given <paramref name="format" />.
/// </summary>
template<format Format, format... Formats>
inline std::size_t to_scalar_size(
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
inline std::size_t to_size(_In_ const format_list<>, _In_ const format) {
    throw std::logic_error("The size of the specified format could not be "
        "derived. Please ensure that the format is a valid enumeration "
        "member and that all enumeration members are registered in the "
        "formats list.");
}

/// <summary>
/// Determines the size of the given <paramref name="format" />.
/// </summary>
template<format Format, format... Formats>
inline std::size_t to_size(
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

FPD_DETAIL_NAMESPACE_END


FPD_NAMESPACE_BEGIN

/// <summary>
/// A list of all the types that are represented by the <see cref="format" />s
/// in the <see cref="formats" /> list.
/// </summary>
typedef detail::format_type_list<formats>::type format_types;


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
inline std::size_t to_components(_In_ const format format) noexcept {
    return detail::to_components(formats(), format);
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
/// <exception cref="std::logic_error">If an invalid <paramref name="format" />
/// was provided or if the type traits have not been setup properly. The latter
/// is a bug in the implementation of the library.</exception>
inline std::size_t to_scalar_size(_In_ const format format) {
    typedef format_list_cat<formats, format::unknown> dispatch_type;
    return detail::to_scalar_size(dispatch_type(), format);
}


/// <summary>
/// Answer the size in bytes of the given <paramref name="format" />.
/// </summary>
/// <param name="format">The format to get the size of.</param>
/// <returns>The size of the format in bytes.</returns>
/// <exception cref="std::logic_error">If an invalid <paramref name="format" />
/// was provided or if the type traits have not been setup properly. The latter
/// is a bug in the implementation of the library.</exception>
inline std::size_t to_size(_In_ const format format) {
    typedef format_list_cat<formats, format::unknown> dispatch_type;
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
    typedef format_list_cat<formats, format::unknown> dispatch_type;
    return detail::to_string<TChar>(dispatch_type(), format);
}

FPD_NAMESPACE_END

#endif /* !defined(_FPD_FORMAT_TRAITS_H) */
