// <copyright file="code_page.h" company="Visualisierungsinstitut der Universität Stuttgart">
// Copyright © 2023 - 2025 Visualisierungsinstitut der Universität Stuttgart.
// Licensed under the MIT licence. See LICENCE file for details.
// </copyright>
// <author>Christoph Müller</author>

#if !defined(_FPD_CODE_PAGE_H_)
#define _FPD_CODE_PAGE_H_
#pragma once

#include "fpd/endian.h"


FPD_NAMESPACE_BEGIN

/// <summary>
/// The type used to specify a code page.
/// </summary>
typedef const char *code_page;


namespace code_pages {

    /// <summary>
    /// The constant identifying ASCII encoding.
    /// </summary>
    constexpr const code_page ascii = "US-ASCII";

    /// <summary>
    /// The constant identifiyng the ANSI code page.
    /// </summary>
    constexpr const code_page ansi = "CP1252";

    /// <summary>
    /// The default code page of the system.
    /// </summary>
    constexpr const code_page oem = nullptr;

    /// <summary>
    /// The constant identifiyng the UTF-16 encoding using big endian.
    /// </summary>
    /// <remarks>
    /// Note that LibICU will include a BOMs when converting to UTF-16
    /// repesentations that do not specify the endian. Therefore, we do not
    /// provide constants without it here as this poses a great risk of
    /// unexpected effects.
    /// </remarks>
    constexpr const code_page utf16be = "UTF-16BE";

    /// <summary>
    /// The constant identifiyng the UTF-16 encoding using little endian.
    /// </summary>
    /// <remarks>
    /// Note that LibICU will include a BOMs when converting to UTF-16
    /// repesentations that do not specify the endian. Therefore, we do not
    /// provide constants without it here as this poses a great risk of
    /// unexpected effects.
    /// </remarks>
    constexpr const code_page utf16le = "UTF-16LE";

    /// <summary>
    /// Identifies the UTF-16 for the native endianness of the system.
    /// </summary>
    constexpr const code_page utf16 = (endian::system == endian::big)
        ? utf16be : utf16le;

    /// <summary>
    /// The constant identifiyng the UTF-32 encoding using big endian.
    /// </summary>
    constexpr const code_page utf32be = "UTF-32BE";

    /// <summary>
    /// The constant identifiyng the UTF-32 encoding using little endian.
    /// </summary>
    constexpr const code_page utf32le = "UTF-32LE";

    /// <summary>
    /// Identifies the UTF-32 for the native endianness of the system.
    /// </summary>
    constexpr const code_page utf32 = (endian::system == endian::big)
        ? utf32be : utf32le;

    /// <summary>
    /// The constant identifiyng the UTF-8 encoding.
    /// </summary>
    constexpr const code_page utf8 = "UTF-8";

} /* namespace encoding */

FPD_NAMESPACE_END

#endif /* !defined(_FPD_CODE_PAGE_H_) */
