// <copyright file="format_taits.h" company="Visualisierungsinstitut der Universität Stuttgart">
// Copyright © 2023 Visualisierungsinstitut der Universität Stuttgart.
// Licensed under the MIT licence. See LICENCE file for details.
// </copyright>
// <author>Christoph Müller</author>

#pragma once

#include <array>
#include <stdexcept>
#include <string>

#if defined(_WIN32)
#include <dxgi.h>
#endif /* defined(_WIN32) */

#include "fpd/format.h"
#include "fpd/literal.h"
#include "fpd/type_components.h"
#include "fpd/type_list.h"
#include "fpd/type_size.h"


FPD_NAMESPACE_BEGIN

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


#define _SPECIALISE_SCALAR_FMT_TRAITS(f, t)\
template<> struct format_forward_traits<format::f> {\
    typedef t type;\
    static constexpr const format format = format::f;\
    template<class TChar>\
    static inline constexpr const TChar *name(void) noexcept {\
        return FPD_TPL_LITERAL(TChar, #f);\
    }\
};\
template<> struct format_reverse_traits<t> {\
    typedef t type;\
    static constexpr const format format = format::f;\
    template<class TChar>\
    static inline constexpr const TChar *name(void) noexcept {\
        return FPD_TPL_LITERAL(TChar, #f);\
    }\
}

#define _SPECIALISE_VEC_FMT_TRAITS(f, s, t, d)\
template<> struct format_forward_traits<format::f##s> {\
    typedef std::array<t, d> type;\
    static constexpr const format format = format::f##s;\
    template<class TChar>\
    static inline constexpr const TChar *name(void) noexcept {\
        return FPD_TPL_LITERAL(TChar, #f#s); \
    }\
};\
template<> struct format_reverse_traits<std::array<t, d>> {\
    typedef std::array<t, d> type;\
    static constexpr const format format = format::f##s;\
    template<class TChar>\
    static inline constexpr const TChar *name(void) noexcept {\
        return FPD_TPL_LITERAL(TChar, #f#s); \
    }\
}


#define _SPECIALISE_FMT_TRAITS(f, t)\
_SPECIALISE_SCALAR_FMT_TRAITS(f##x, t);\
_SPECIALISE_VEC_FMT_TRAITS(f, xy, t, 2);\
_SPECIALISE_VEC_FMT_TRAITS(f, xyz, t, 3);\
_SPECIALISE_VEC_FMT_TRAITS(f, xyzw, t, 4)


_SPECIALISE_SCALAR_FMT_TRAITS(unknown, void);
_SPECIALISE_FMT_TRAITS(int8, std::int8_t);
_SPECIALISE_FMT_TRAITS(uint8, std::uint8_t);
_SPECIALISE_FMT_TRAITS(int16, std::int16_t);
_SPECIALISE_FMT_TRAITS(uint16, std::uint16_t);
_SPECIALISE_FMT_TRAITS(int32, std::int32_t);
_SPECIALISE_FMT_TRAITS(uint32, std::uint32_t);
_SPECIALISE_FMT_TRAITS(int64, std::int64_t);
_SPECIALISE_FMT_TRAITS(uint64, std::uint64_t);
_SPECIALISE_FMT_TRAITS(float32, float);
_SPECIALISE_FMT_TRAITS(float64, double);


#undef _SPECIALISE_FMT_TRAITS
#undef _SPECIALISE_VEC_FMT_TRAITS
#undef _SPECIALISE_SCALAR_FMT_TRAITS


namespace detail {

    template<class TList> struct format_type_list;

    template<format... Formats>
    struct format_type_list<format_list<Formats ...>> final {
        template<format F>
        using traits = typename format_forward_traits<F>::type;
        typedef type_list<traits<Formats>...> type;
    };

    template<class TList, format Format> struct in_format_list;

    template<format Head, format... Tail, format Needle>
    struct in_format_list<format_list<Head, Tail...>, Needle> final {
        static constexpr const bool value = (Head == Needle)
            || in_format_list<format_list<Tail...>, Needle>::value;
    };

    template<format Needle>
    struct in_format_list<format_list<>, Needle> final : std::false_type { };

    inline constexpr std::size_t to_components(_In_ const format_list<>,
            _In_ const format) noexcept {
        return 0;
    }

    template<format Format, format... Formats>
    inline std::size_t to_components(
            _In_ const format_list<Format, Formats...>,
            _In_ const format format) {
        if (format == Format) {
            return type_components<format_forward_traits<Format>::type>::value;
        } else {
            return to_components(format_list<Formats...>(), format);
        }
    }

    inline std::size_t to_size(_In_ const format_list<>, _In_ const format) {
        throw std::logic_error("The size of the specified format could not be "
            "derived. Please ensure that the format is a valid enumeration "
            "member and that all enumeration members are registered in the "
            "formats list.");
    }

    template<format Format, format... Formats>
    inline std::size_t to_size(
            _In_ const format_list<Format, Formats...>,
            _In_ const format format) {
        if (format == Format) {
            return type_size<format_forward_traits<Format>::type>::value;
        } else {
            return to_size(format_list<Formats...>(), format);
        }
    }

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

    template<class TChar> inline const TChar *const to_string(
            _In_ const format_list<>,
            _In_ const format) {
        throw std::logic_error("The specified format could not be converted to "
            "a string. Please ensure that the format is a valid enumeration "
            "member and that all enumeration members are registered in the "
            "formats list.");
    }

} /* namespace detail */


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
    : std::integral_constant<bool, detail::in_format_list<TList,
    Format>::value> { };


/// <summary>
/// Answer whether the <see cref="format" /> of <typeparamref name="TValue" />
/// is in the given <typeparamref name="TList" /> of formats.
/// </summary>
/// <typeparam name="TList"></typeparam>
/// <typeparam name="TValue"></typeparam>
template<class TList, class TValue> struct is_supported_type final
    : std::integral_constant<bool, detail::in_format_list<TList,
    format_reverse_traits<typename std::decay<TValue>::type>::format>::value> { };


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
/// Answer the size in bytes of the given <paramref name="format" />.
/// </summary>
/// <param name="format">The format to get the size of.</param>
/// <returns>The size of the forma in bytes.</returns>
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
