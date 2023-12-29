// <copyright file="code_page.h" company="Visualisierungsinstitut der Universität Stuttgart">
// Copyright © 2023 Visualisierungsinstitut der Universität Stuttgart.
// Licensed under the MIT licence. See LICENCE file for details.
// </copyright>
// <author>Christoph Müller</author>

#pragma once

#if defined(_WIN32)
#include <Windows.h>
#endif /* defined(_WIN32) */

#include "fpd/abi.h"


FPD_NAMESPACE_BEGIN

#if defined(_WIN32)

/// <summary>
/// Possible code pages for narrow strings.
/// </summary>
enum class code_page : UINT {

    /// <summary>
    /// The ANSI code page.
    /// </summary>
    ansi = CP_ACP,

    /// <summary>
    /// The OEM code page used by the console.
    /// </summary>
    oem = CP_OEMCP,

    ///// <summary>
    ///// UTF-7 encoding.
    ///// </summary>
    //utf7 = CP_UTF7,

    /// <summary>
    /// UTF-8 encoding.
    /// </summary>
    utf8 = CP_UTF8
};

#else /* defined(_WIN32) */

namespace code_page {

    /// <summary>
    /// The ANSI code page.
    /// </summary>
    constexpr const char *const ansi = "ANSI";

    /// <summary>
    /// The default code page of the system.
    /// </summary>
    constexpr const char *const oem = nullptr;

    /// <summary>
    /// UTF-8 encoding.
    /// </summary>
    constexpr const char *const ansi = "UTF-8";

} /* namespace encoding */

#endif /* defined(_WIN32) */

FPD_NAMESPACE_END
