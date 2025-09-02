// <copyright file="io.h" company="Visualisierungsinstitut der Universität Stuttgart">
// Copyright © 2025 Visualisierungsinstitut der Universität Stuttgart.
// Licensed under the MIT licence. See LICENCE file for details.
// </copyright>
// <author>Christoph Müller</author>

#if !defined(_FPD_IO_H)
#define _FPD_IO_H
#pragma once

#include <cassert>
#include <cinttypes>
#include <cstdlib>
#include <type_traits>

#include "fpd/abi.h"


FPD_DETAIL_NAMESPACE_BEGIN


/// <summary>
/// A wrapper around the I/O implementation <typeparamref name="TImpl" /> that
/// writes until all bytes have been written or a failure has occurred.
/// </summary>
/// <typeparam name="TImpl">The I/O implementation to be used. This can be one
/// of <see cref="posix_io" /> or <see cref="win32_io" />.</typeparam>
/// <param name="io">The I/O wrapper used for writing.</param>
/// <param name="data">A pointer to at least <paramref name="cnt" /> bytes that
/// are being writting.</param>
/// <param name="cnt">The number of ybtes to write.</param>
template<class TImpl> void write_all(_In_ TImpl& io,
        _In_reads_bytes_(size) const void *data,
        _In_ std::size_t cnt) {
    if ((data != nullptr) && (cnt > 0)) {
        auto cur = static_cast<const std::uint8_t *>(data);

        while (cnt > 0) {
            const auto written = io.write(cur, cnt);
            assert(written <= cnt);
            cur += written;
            cnt -= written;
        }
    }
}


/// <summary>
/// A wrapper around the I/O implementation <typeparamref name="TImpl" /> that
/// writes the whole given POD type to disk.
/// </summary>
/// <typeparam name="TImpl">The I/O implementation to be used. This can be one
/// of <see cref="posix_io" /> or <see cref="win32_io" />.</typeparam>
/// <typeparam name="TType">A POD type to be written.</typeparam>
/// <param name="io">The I/O wrapper used for writing.</param>
/// <param name="data">The data to be written.</param>
template<class TImpl, class TType>
std::enable_if_t<std::is_standard_layout_v<TType>> write_all(
        _In_ TImpl&& io,
        const TType& data) {
    return write_all(std::forward<TImpl>(io),
        std::addressof(data),
        sizeof(data));
}

FPD_DETAIL_NAMESPACE_END

#endif /* !defined(_FPD_IO_H) */
