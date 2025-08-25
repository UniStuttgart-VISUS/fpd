// <copyright file="with_code_page.h" company="Visualisierungsinstitut der Universität Stuttgart">
// Copyright © 2024 - 2025 Visualisierungsinstitut der Universität Stuttgart.
// Licensed under the MIT licence. See LICENCE file for details.
// </copyright>
// <author>Christoph Müller</author>

#if !defined(_FPD_WITH_CODE_PAGE_H)
#define _FPD_WITH_CODE_PAGE_H
#pragma once

#include <cassert>

#include "fpd/abi.h"
#include "fpd/annotations.h"
#include "fpd/encoding.h"


FPD_NAMESPACE_BEGIN

/// <summary>
/// Attaches a code page to a class if the character type is an MBCS type.
/// </summary>
/// <typeparam name="TChar">The type of a character in a string.</typeparam>
template<class TChar> class with_code_page {

public:

    /// <summary>
    /// The type used to specify a code page.
    /// </summary>
    typedef FPD_NAMESPACE::encoding encoding_type;

    /// <summary>
    /// Indicates that the class does not really store a code page.
    /// </summary>
    static constexpr const bool has_code_page = false;

    /// <summary>
    /// This should not be called, because it is meaningless for UTF-16 and UTF-32.
    /// </summary>
    /// <returns><see langword="nullptr" />.</returns>
    inline constexpr _Ret_null_ encoding_type code_page(void) const noexcept {
        assert(false);
        return nullptr;
    }

    /// <summary>
    /// Does nothing.
    /// </summary>
    /// <param name="code_page">The new code page for narrow strings.</param>
    inline void code_page(_In_ const encoding_type code_page) noexcept { }

protected:

    /// <summary>
    /// Initialises a new instance.
    /// </summary>
    /// <param name="code_page">The initial value of the code page.</param>
    inline with_code_page(_In_ const encoding_type) noexcept { }

};


/// <summary>
/// Specialisation for <c>char</c>, which requires are code page.
/// </summary>
template<> class with_code_page<char> {

public:

    /// <summary>
    /// The type used to specify a code page.
    /// </summary>
    typedef FPD_NAMESPACE::encoding encoding_type;

    /// <summary>
    /// Indicates that the class actually stores a code page.
    /// </summary>
    static constexpr const bool has_code_page = true;

    /// <summary>
    /// Answer the configured code page for narrow strings.
    /// </summary>
    /// <returns>The configured code page.</returns>
    inline encoding_type code_page(void) const noexcept {
        return this->_code_page;
    }

    /// <summary>
    /// Sets the code page for narrow strings.
    /// </summary>
    /// <param name="code_page">The new code page for narrow strings.</param>
    inline void code_page(_In_ const encoding_type code_page) noexcept {
        this->_code_page = code_page;
    }

protected:

    /// <summary>
    /// Initialises a new instance.
    /// </summary>
    /// <param name="code_page">The initial value of the code page.</param>
    inline with_code_page(
            _In_ const encoding_type code_page = encodings::oem) noexcept
        : _code_page(code_page) { }

private:

    encoding_type _code_page;
};

FPD_NAMESPACE_END

#endif /* !defined(_FPD_WITH_CODE_PAGE_H) */
