// <copyright file="literal.h" company="Visualisierungsinstitut der Universität Stuttgart">
// Copyright © 2023 - 2025 Visualisierungsinstitut der Universität Stuttgart.
// Licensed under the MIT licence. See LICENCE file for details.
// </copyright>
// <author>Christoph Müller</author>

#if !defined(_FPD_LITERAL_H)
#define _FPD_LITERAL_H
#pragma once

#include "fpd/abi.h"
#include "fpd/annotations.h"


FPD_NAMESPACE_BEGIN

namespace detail {

    /// <summary>
    /// The type of a UTF-8 code unit.
    /// </summary>
    /// <remarks>
    /// If the compiler supports <c>char8_t</c>, this type will be
    /// <c>char8_t</c>, otherwise, it will be <c>char</c>.
    /// </remarks>
#if defined(__cpp_char8_t)
    typedef char8_t utf8_type;
#else /* defined(__cpp_char8_t) */
    typedef char utf8_type;
#endif /* defined(__cpp_char8_t) */

    /// <summary>
    /// A helper class for selecting a literal string based on a template
    /// parameter.
    /// </summary>
    /// <remarks>
    /// This must be used by means of the <c>FPD_TPL_LITERAL</c> macro.
    /// </remarks>
    /// <typeparam name="TChar">The type of a character.</typeparam>
    template<class TChar> struct literal_selector {
        typedef TChar char_type;
    };

    /// <summary>
    /// Specialisation for ASCII characters.
    /// </summary>
    template<> struct literal_selector<char> {
        typedef char char_type;

        static constexpr char_type select(
                _In_ const char c,
                _In_ const wchar_t,
                _In_ const char16_t,
                _In_ const utf8_type) noexcept {
            return c;
        }

        _When_((s == nullptr), _Ret_maybenull_z_)
        _When_((s != nullptr), _Ret_valid_)
        static constexpr const char_type *select(
                _In_opt_z_ const char *s,
                _In_opt_z_ const wchar_t *,
                _In_opt_z_ const char16_t *,
                _In_opt_z_ const utf8_type *) noexcept {
            return s;
        }
    };

    /// <summary>
    /// Specialisation for Unicode characters.
    /// </summary>
    template<> struct literal_selector<wchar_t> {
        typedef wchar_t char_type;

        static constexpr char_type select(
                _In_ const char,
                _In_ const wchar_t c,
                _In_ const char16_t,
                _In_ const utf8_type) noexcept {
            return c;
        }

        _When_((s == nullptr), _Ret_maybenull_z_)
        _When_((s != nullptr), _Ret_valid_)
        static constexpr const char_type *select(
                _In_opt_z_ const char *,
                _In_opt_z_ const wchar_t *s,
                _In_opt_z_ const char16_t *,
                _In_opt_z_ const utf8_type *) noexcept {
            return s;
        }
    };

    /// <summary>
    /// Specialisation for UTF-16 characters.
    /// </summary>
    template<> struct literal_selector<char16_t> {
        typedef char16_t char_type;

        static constexpr char_type select(
                _In_ const char,
                _In_ const wchar_t,
                _In_ const char16_t c,
                _In_ const utf8_type) noexcept {
            return c;
        }

        _When_((s == nullptr), _Ret_maybenull_z_)
        _When_((s != nullptr), _Ret_valid_)
        static constexpr const char_type *select(
                _In_opt_z_ const char *,
                _In_opt_z_ const wchar_t *,
                _In_opt_z_ const char16_t *s,
                _In_opt_z_ const utf8_type *) noexcept {
            return s;
        }
    };

#if defined(__cpp_char8_t)
    /// <summary>
    /// Specialisation for UTF-8 characters.
    /// </summary>
    template<> struct literal_selector<utf8_type> {
        typedef utf8_type char_type;

        static constexpr char_type select(
                _In_ const char,
                _In_ const wchar_t,
                _In_ const char16_t,
                _In_ const utf8_type c) noexcept {
            return c;
        }

        _When_((s == nullptr), _Ret_maybenull_z_)
        _When_((s != nullptr), _Ret_valid_)
        static constexpr const char_type *select(
                _In_opt_z_ const char *,
                _In_opt_z_ const wchar_t *,
                _In_opt_z_ const char16_t *,
                _In_opt_z_ const utf8_type *s) noexcept {
            return s;
        }
    };
#endif /* defined(__cpp_char8_t) */

} /* namespace detail */

FPD_NAMESPACE_END


/// <summary>
/// Generates a literal string <paramref name="l" />, which is either ASCII or
/// Unicode depending on the template parameter <paramref name="T" />.
/// </summary>
/// <param name="T">The type of the character to select.</param>
/// <param name="l">The string to be selected.</param>
#define FPD_TPL_LITERAL(T, l) \
fpd::detail::literal_selector<T>::select(l, L##l, u##l, u8##l)

#endif /* !defined(_FPD_LITERAL_H) */
