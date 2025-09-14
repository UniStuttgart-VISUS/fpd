// <copyright file="compression.h" company="Visualisierungsinstitut der Universität Stuttgart">
// Copyright © 2024 - 2025 Visualisierungsinstitut der Universität Stuttgart.
// Licensed under the MIT licence. See LICENCE file for details.
// </copyright>
// <author>Christoph Müller</author>

#if !defined(_FPD_COMPRESSION_H_)
#define _FPD_COMPRESSION_H_
#pragma once

#include <cinttypes>

#include "fpd/enum_dispatch_list.h"


FPD_NAMESPACE_BEGIN

/// <summary>
/// The compression algorithms supported by the library.
/// </summary>
/// <remarks>
/// <para>Adding a new compression method involves several steps: add it to the
/// enumeration with a unique value; add it to the dispatch list
/// <see cref="compressions" /> below the enum; add a specialisation of the
/// <see cref="compression_meta_data" /> template; add support for measuring,
/// reading and writing the specialisation in <see cref="meta_data" />.</para>
/// </remarks>
enum class compression : std::uint32_t {

    /// <summary>
    /// The data are not compressed.
    /// </summary>
    none = 0x00000000,

    /// <summary>
    /// The data are compressed using GDeflate, which can be decompressed using
    /// DirectStorage on the GPU. Each frame comprises one or more blocks that
    /// are compressed individually, and all frames in the file have the same
    /// number of blocks, which can be obtained from the remaining data in the
    /// compression header.
    /// </summary>
    gdeflate = 0x00000001,

};

FPD_NAMESPACE_END


FPD_DETAIL_NAMESPACE_BEGIN

/// <summary>
/// A specialised <see cref="enum_dispatch_list" /> for
/// <see cref="compression" /> methods.
/// </summary>
/// <typeparam name="Compressions">The possible compression methods that can be
/// dispatched.</typeparam>
template<compression... Compressions>
using compression_dispatch_list = enum_dispatch_list<compression,
    Compressions...>;


/// <summary>
/// A dispatching list for <see compression /> values.
/// </summary>
typedef compression_dispatch_list<
    compression::none,
    compression::gdeflate> compressions;

FPD_DETAIL_NAMESPACE_END

#endif /* !defined(_FPD_COMPRESSION_H_) */
