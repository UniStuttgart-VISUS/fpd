// <copyright file="with_encoding.h" company="Visualisierungsinstitut der Universität Stuttgart">
// Copyright © 2024 - 2025 Visualisierungsinstitut der Universität Stuttgart.
// Licensed under the MIT licence. See LICENCE file for details.
// </copyright>
// <author>Christoph Müller</author>

#if !defined(_FPD_WITH_ENCODING_H)
#define _FPD_WITH_ENCODING_H
#pragma once

#include "fpd/encoding.h"


FPD_DETAIL_NAMESPACE_BEGIN

/// <summary>
/// Attaches a code page to a class if the character type is an MBCS type.
/// </summary>
/// <typeparam name="TChar">The type of a character in a string.</typeparam>
template<class TChar> class with_encoding;


/// <summary>
/// Specialisation for <c>char</c>, which requires a code page to be attached
/// to a string in order to convert from and to Unicode.
/// </summary>
template<> class with_encoding<char> {

public:

    /// <summary>
    /// Indicates that the class actually stores a code page.
    /// </summary>
    static constexpr const bool has_code_page = true;

    /// <summary>
    /// Answer the configured code page for narrow strings.
    /// </summary>
    /// <returns>The configured code page.</returns>
    inline _Ret_maybenull_z_ encoding code_page(void) const noexcept {
        return this->_code_page;
    }

    /// <summary>
    /// Sets the code page for narrow strings.
    /// </summary>
    /// <param name="code_page">The new code page for narrow strings.</param>
    inline void code_page(_In_opt_z_ const encoding code_page) noexcept {
        this->_code_page = code_page;
    }

protected:

    /// <summary>
    /// Initialises a new instance.
    /// </summary>
    /// <param name="code_page">The initial value of the code page.</param>
    inline with_encoding(_In_opt_z_ const encoding code_page) noexcept
        : _code_page(code_page) { }

private:

    encoding _code_page;
};


/// <summary>
/// Specialisation for <see langword="wchar_t" />, which can be UTF-16 or UTF-32
/// based on the platform.
/// </summary>
template<> class with_encoding<wchar_t> {

public:

    /// <summary>
    /// Indicates that the class does not really store a code page because the
    /// encoding is determined by the type itself.
    /// </summary>
    static constexpr const bool has_code_page = false;

    /// <summary>
    /// Answer whether the encoding is UTF-16 or UTF-32 on the current platform.
    /// </summary>
    /// <returns>The encoding of <see langword="wchar_t" />.</returns>
    inline constexpr _Ret_z_ encoding code_page(void) const noexcept {
        static_assert((sizeof(wchar_t) == 2) || (sizeof(wchar_t) == 4), "The "
            "wide character type is expected to be UTF-16 or UTF-32.");
        return (sizeof(wchar_t) == 4) ? encodings::utf32 : encodings::utf16;
    }

    /// <summary>
    /// Does nothing.
    /// </summary>
    inline void code_page(_In_opt_z_ const encoding) noexcept { }

protected:

    /// <summary>
    /// Initialises a new instance.
    /// </summary>
    /// <param name="code_page">The initial value of the code page.</param>
    inline with_encoding(_In_opt_z_ const encoding) noexcept { }
};


/// <summary>
/// Specialisation for <see langword="char16_t" />.
/// </summary>
template<> class with_encoding<char16_t> {

public:

    /// <summary>
    /// Indicates that the class does not really store a code page because the
    /// encoding is determined by the type itself.
    /// </summary>
    static constexpr const bool has_code_page = false;

    /// <summary>
    /// Answers UTF-16.
    /// </summary>
    /// <returns>The identifier for UTF-16.</returns>
    inline constexpr _Ret_z_ encoding code_page(void) const noexcept {
        return encodings::utf16;
    }

    /// <summary>
    /// Does nothing.
    /// </summary>
    inline void code_page(_In_opt_z_ const encoding) noexcept { }

protected:

    /// <summary>
    /// Initialises a new instance.
    /// </summary>
    /// <param name="code_page">The initial value of the code page.</param>
    inline with_encoding(_In_opt_z_ const encoding) noexcept { }
};


/// <summary>
/// Specialisation for <see langword="char32_t" />.
/// </summary>
template<> class with_encoding<char32_t> {

public:

    /// <summary>
    /// Indicates that the class does not really store a code page because the
    /// encoding is determined by the type itself.
    /// </summary>
    static constexpr const bool has_code_page = false;

    /// <summary>
    /// Answers UTF-32.
    /// </summary>
    /// <returns>The identifier for UTF-32.</returns>
    inline constexpr _Ret_z_ encoding code_page(void) const noexcept {
        return encodings::utf32;
    }

    /// <summary>
    /// Does nothing.
    /// </summary>
    inline void code_page(_In_opt_z_ const encoding) noexcept { }

protected:

    /// <summary>
    /// Initialises a new instance.
    /// </summary>
    /// <param name="code_page">The initial value of the code page.</param>
    inline with_encoding(_In_opt_z_ const encoding) noexcept { }
};

FPD_DETAIL_NAMESPACE_END

#endif /* !defined(_FPD_WITH_ENCODING_H) */
