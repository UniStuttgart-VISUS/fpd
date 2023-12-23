// <copyright file="literal.h" company="Visualisierungsinstitut der Universität Stuttgart">
// Copyright © 2023 Visualisierungsinstitut der Universität Stuttgart.
// Licensed under the MIT licence. See LICENCE file for details.
// </copyright>
// <author>Christoph Müller</author>

#pragma once

#include "fpd/abi.h"
#include "fpd/annotations.h"


FPD_NAMESPACE_BEGIN

namespace detail {

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

        static constexpr char_type select(_In_ const char n,
                _In_ const wchar_t w) noexcept {
            return n;
        }

        _When_((n == nullptr) || (w == nullptr), _Ret_maybenull_z_)
        _When_((n != nullptr) && (w != nullptr), _Ret_valid_)
        static constexpr const char_type *select(
                _In_opt_z_ const char *n,
                _In_opt_z_ const wchar_t *w) noexcept {
            return n;
        }
    };

    /// <summary>
    /// Specialisation for Unicode characters.
    /// </summary>
    template<> struct literal_selector<wchar_t> {
        typedef wchar_t char_type;

        static constexpr char_type select(_In_ const char n,
                _In_ const wchar_t w) noexcept {
            return w;
        }

        _When_((n == nullptr) || (w == nullptr), _Ret_maybenull_z_)
        _When_((n != nullptr) && (w != nullptr), _Ret_valid_)
        static constexpr const char_type *select(
                _In_opt_z_ const char *n,
                _In_opt_z_ const wchar_t *w) noexcept {
            return w;
        }
    };
}

FPD_NAMESPACE_END


/// <summary>
/// Generates a literal string <paramref name="l" />, which is either ASCII or
/// Unicode depending on the template parameter <paramref name="T" />.
/// </summary>
/// <param name="T">The type of the character to select.</param>
/// <param name="l">The string to be selected.</param>
#define FPD_TPL_LITERAL(T, l) fpd::detail::literal_selector<T>::select(l, L##l)
