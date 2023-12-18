// <copyright file="endian.h" company="Visualisierungsinstitut der Universität Stuttgart">
// Copyright © 2023 Visualisierungsinstitut der Universität Stuttgart.
// Licensed under the MIT licence. See LICENCE file for details.
// </copyright>
// <author>Christoph Müller</author>

#pragma once

#include <cinttypes>

#include "fpd/abi.h"


FPD_NAMESPACE_BEGIN

/// <summary>
/// Specifies the endians the library supports.
/// </summary>
enum class endian : std::uint16_t {

    /// <summary>
    /// The endian is unknown, which usually means that the library will be
    /// unable to process data.
    /// </summary>
    undefined = 0,

    /// <summary>
    /// Little endian representation with least significant byte coming
    /// first in memory.
    /// </summary>
    little = 0x0102,

    /// <summary>
    /// Big endian representation with most significant byte coming first in
    /// memory.
    /// </summary>
    big = 0x0201,

    /// <summary>
    /// Endian of the system the code is running on.
    /// </summary>
    system = '\1\2'

};

FPD_NAMESPACE_END
