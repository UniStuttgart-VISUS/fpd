// <copyright file="compression.h" company="Visualisierungsinstitut der Universität Stuttgart">
// Copyright © 2024 Visualisierungsinstitut der Universität Stuttgart.
// Licensed under the MIT licence. See LICENCE file for details.
// </copyright>
// <author>Christoph Müller</author>

#pragma once

#include <cinttypes>

#include "fpd/abi.h"


FPD_NAMESPACE_BEGIN

/// <summary>
/// The compression algorithms supported by the library.
/// </summary>
enum class compression : std::uint32_t {

    /// <summary>
    /// The data are not compressed.
    /// </summary>
    none = 0x00000000,

    /// <summary>
    /// The data are compressed using GDeflate, which can be decompressed using
    /// DirectStorage on the GPU.
    /// </summary>
    gdeflate = 0x00000001,

};

FPD_NAMESPACE_END
