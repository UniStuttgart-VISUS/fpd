// <copyright file="code_page.h" company="Visualisierungsinstitut der Universität Stuttgart">
// Copyright © 2023 - 2025 Visualisierungsinstitut der Universität Stuttgart.
// Licensed under the MIT licence. See LICENCE file for details.
// </copyright>
// <author>Christoph Müller</author>

#if !defined(_FPD_ENCODING_H)
#define _FPD_ENCODING_H
#pragma once

#include "fpd/endian.h"


FPD_NAMESPACE_BEGIN

/// <summary>
/// The type used to specify a code page.
/// </summary>
typedef const char *encoding;


namespace encodings {

    /// <summary>
    /// The constant identifying ASCII encoding.
    /// </summary>
    constexpr const encoding ascii = "US-ASCII";

    /// <summary>
    /// The constant identifiyng the ANSI code page.
    /// </summary>
    constexpr const encoding ansi = "CP1252";

    /// <summary>
    /// The constant identifying ISO Latin-1 (ISO 8859-1).
    /// </summary>
    constexpr const encoding latin1 = "ISO_8859-1";

    /// <summary>
    /// The default code page of the system.
    /// </summary>
    constexpr const encoding oem = nullptr;

    /// <summary>
    /// The constant identifiyng the UTF-16 encoding using big endian.
    /// </summary>
    /// <remarks>
    /// Note that LibICU will include a BOMs when converting to UTF-16
    /// repesentations that do not specify the endian. Therefore, we do not
    /// provide constants without it here as this poses a great risk of
    /// unexpected effects.
    /// </remarks>
    constexpr const encoding utf16be = "UTF-16BE";

    /// <summary>
    /// The constant identifiyng the UTF-16 encoding using little endian.
    /// </summary>
    /// <remarks>
    /// Note that LibICU will include a BOMs when converting to UTF-16
    /// repesentations that do not specify the endian. Therefore, we do not
    /// provide constants without it here as this poses a great risk of
    /// unexpected effects.
    /// </remarks>
    constexpr const encoding utf16le = "UTF-16LE";

    /// <summary>
    /// Identifies the UTF-16 for the native endianness of the system.
    /// </summary>
    constexpr const encoding utf16 = (endian::system == endian::big)
        ? utf16be : utf16le;

    /// <summary>
    /// The constant identifiyng the UTF-32 encoding using big endian.
    /// </summary>
    constexpr const encoding utf32be = "UTF-32BE";

    /// <summary>
    /// The constant identifiyng the UTF-32 encoding using little endian.
    /// </summary>
    constexpr const encoding utf32le = "UTF-32LE";

    /// <summary>
    /// Identifies the UTF-32 for the native endianness of the system.
    /// </summary>
    constexpr const encoding utf32 = (endian::system == endian::big)
        ? utf32be : utf32le;

    /// <summary>
    /// The constant identifiyng the UTF-8 encoding.
    /// </summary>
    constexpr const encoding utf8 = "UTF-8";

} /* namespace encodings */

FPD_NAMESPACE_END

#endif /* !defined(_FPD_ENCODING_H) */
