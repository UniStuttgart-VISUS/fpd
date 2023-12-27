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
/// <typeparam name="TFormat">The type to reflect on.</typeparam>
template<class TFormat> struct format_reverse_traits { };


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


_SPECIALISE_SCALAR_FMT_TRAITS(unknown, void *);
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

    template<class TChar, format Format, format... Formats>
    inline const TChar *const to_string(
            _In_ const format_list<Format, Formats...>,
            _In_ const format format) {
        if (format == Format) {
            return format_forward_traits<Format>::name<TChar>();
        } else {
            return to_string<TChar>(format_list<Formats...>(), format);
        }
    }

    template<class TChar>
    inline const TChar *const to_string(
            _In_ const format_list<>,
            _In_ const format) {
        throw std::logic_error("The specified format could not be converted to "
            "a string. Please ensure that the format is a valid enumeration "
            "member and that all enumeration members are registered in the "
            "formats list.");
    }

} /* namespace detail */


/// <summary>
/// Answer a human-readable version of the given <paramref name="format" />.
/// </summary>
/// <typeparam name="TChar">The character type of the string to create.
/// </typeparam>
/// <param name="format">The format to convert to a string.</param>
/// <returns>The string representation of the given format.</returns>
template<class TChar>
inline const TChar *const to_string(_In_ const format format) {
    return detail::to_string<TChar>(formats(), format);
}

FPD_NAMESPACE_END
