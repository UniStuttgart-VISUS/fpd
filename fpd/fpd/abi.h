// <copyright file="abi.h" company="Visualisierungsinstitut der Universität Stuttgart">
// Copyright © 2023 Visualisierungsinstitut der Universität Stuttgart.
// Licensed under the MIT licence. See LICENCE file for details.
// </copyright>
// <author>Christoph Müller</author>

#pragma once

#include <cinttypes>


#define FPD_FILE_MAJOR static_cast<std::uint8_t>(0)
#define FPD_FILE_MINOR static_cast<std::uint8_t>(1)


#define FPD_ABI_MAJOR 0
#define FPD_ABI_MINOR 1


#define _FPD_CONCAT0(l, r) l ##r
#define _FPD_CONCAT(l, r) _FPD_CONCAT0(l, r)


/// <summary>
/// Creates the full ABI versison string.
/// </summary>
/// <param name="mj">The major version of the ABI.</param>
/// <param name="mn">The minor version of the ABI.</param>
#define FPD_ABI_VERSION(mj, mn) _FPD_CONCAT(mj, _FPD_CONCAT(_, mn))


/// <summary>
/// Creates the name of the namespace for the specified ABI version.
/// </summary>
/// <param name="mj">The major version of the ABI.</param>
/// <param name="mn">The minor version of the ABI.</param>
#define FPD_ABI_NAMESPACE(mj, mn) _FPD_CONCAT(v, FPD_ABI_VERSION(mj, mn))


/// <summary>
/// Creates the name of the current ABI namespace.
/// </summary>
#define FPD_NAMESPACE fpd::FPD_ABI_NAMESPACE(FPD_ABI_MAJOR, FPD_ABI_MINOR)


/// <summary>
/// Marks the begin of the current ABI namespace.
/// </summary>
#define FPD_NAMESPACE_BEGIN namespace fpd {\
    inline namespace FPD_ABI_NAMESPACE(FPD_ABI_MAJOR, FPD_ABI_MINOR) {


/// <summary>
/// Marks the end of the current ABI namespace.
/// </summary>
#define FPD_NAMESPACE_END } }
