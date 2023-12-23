// <copyright file="format_taits.h" company="Visualisierungsinstitut der Universität Stuttgart">
// Copyright © 2023 Visualisierungsinstitut der Universität Stuttgart.
// Licensed under the MIT licence. See LICENCE file for details.
// </copyright>
// <author>Christoph Müller</author>

#pragma once

#include <array>

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


#define _SPECIALISE_FWD_SCALAR_TRAITS(f, t)\
template<> struct format_forward_traits<format::f##x> {\
    typedef t type;\
    static constexpr const format format = format::f##x;\
    template<class TChar>\
    static inline constexpr const TChar *name(void) noexcept {\
        return FPD_TPL_LITERAL(TChar, #f##"x");\
    }\
};\
template<> struct format_reverse_traits<t> {\
    typedef t type;\
    static constexpr const format format = format::f##x;\
    template<class TChar>\
    static inline constexpr const TChar *name(void) noexcept {\
        return FPD_TPL_LITERAL(TChar, #f##"x");\
    }\
}


#define _SPECIALISE_FWD_VEC_TRAITS(f, s, t, d)\
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


#define _SPECIALISE_FWD_TRAITS(f, t)\
_SPECIALISE_FWD_SCALAR_TRAITS(f, t);\
_SPECIALISE_FWD_VEC_TRAITS(f, xy, t, 2);\
_SPECIALISE_FWD_VEC_TRAITS(f, xyz, t, 3);\
_SPECIALISE_FWD_VEC_TRAITS(f, xyzw, t, 4)


_SPECIALISE_FWD_TRAITS(int8, std::int8_t);
_SPECIALISE_FWD_TRAITS(uint8, std::uint8_t);
_SPECIALISE_FWD_TRAITS(int16, std::int16_t);
_SPECIALISE_FWD_TRAITS(uint16, std::uint16_t);
_SPECIALISE_FWD_TRAITS(int32, std::int32_t);
_SPECIALISE_FWD_TRAITS(uint32, std::uint32_t);
_SPECIALISE_FWD_TRAITS(int64, std::int64_t);
_SPECIALISE_FWD_TRAITS(uint64, std::uint64_t);
_SPECIALISE_FWD_TRAITS(float32, float);
_SPECIALISE_FWD_TRAITS(float64, double);


#undef _SPECIALISE_FWD_TRAITS
#undef _SPECIALISE_FWD_VEC_TRAITS
#undef _SPECIALISE_FWD_SCALAR_TRAITS

FPD_NAMESPACE_END
