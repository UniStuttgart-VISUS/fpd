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
/// A wrapper around the I/O implementation <typeparamref name="TFile" /> that
/// reads up to <paramref name="cnt" /> bytes or until the end of the input has
/// been reached.
/// </summary>
/// <typeparam name="TFile"></typeparam>
/// <param name="data"></param>
/// <param name="cnt"></param>
/// <param name="file"></param>
/// <returns>The number of bytes read, which is <paramref name="cnt" /> unless
/// the end of the file was reached.</returns>
template<class TFile> std::size_t read_all(
        _Out_writes_bytes_opt_(cnt) void *data,
        _In_ const std::size_t cnt,
        _In_ TFile& file) {
    std::size_t retval = 0;

    if ((data != nullptr) && (cnt > 0)) {
        auto cur = static_cast<std::uint8_t *>(data);

        while (retval < cnt) {
            const auto read = file.read(cur, cnt - retval);
            assert(read <= cnt);

            if (read == 0) {
                // EOF was reached.
                return retval;
            }

            cur += read;
            retval += read;
        }
    }

    return retval;
}


/// <summary>
/// A wrapper around the I/O implementation <typeparamref name="TFile" /> that
/// writes until all bytes have been written or a failure has occurred.
/// </summary>
/// <typeparam name="TFile">The I/O implementation to be used. This can be one
/// of <see cref="posix_io" /> or <see cref="win32_io" />.</typeparam>
/// <param name="file">The I/O wrapper used for writing.</param>
/// <param name="data">A pointer to at least <paramref name="cnt" /> bytes that
/// are being writting.</param>
/// <param name="cnt">The number of ybtes to write.</param>
/// <returns>The number of bytes written, which is always
/// <paramref name="cnt" />.</returns>
template<class TFile> std::size_t write_all(_In_ TFile& file,
        _In_reads_bytes_opt_(cnt) const void *data,
        _In_ const std::size_t cnt) {
    std::size_t retval = 0;

    if ((data != nullptr) && (cnt > 0)) {
        auto cur = static_cast<const std::uint8_t *>(data);

        while (retval < cnt) {
            const auto written = file.write(cur, cnt - retval);
            assert(written <= cnt);
            cur += written;
            retval += written;
        }
    }

    return retval;
}


/// <summary>
/// A wrapper around the I/O implementation <typeparamref name="TFile" /> that
/// writes the whole given POD type to disk.
/// </summary>
/// <typeparam name="TFile">The I/O implementation to be used. This can be one
/// of <see cref="posix_io" /> or <see cref="win32_io" />.</typeparam>
/// <typeparam name="TType">A POD type to be written.</typeparam>
/// <param name="file">The I/O wrapper used for writing.</param>
/// <param name="data">The data to be written.</param>
/// <returns>The number of bytes written, which is the size of
/// <typeparamref name="TType" />.</returns>
template<class TFile, class TType>
std::enable_if_t<std::is_standard_layout_v<TType>, std::size_t> write_all(
        _In_ TFile&& file,
        _In_ const TType& data) {
    return write_all(std::forward<TFile>(file),
        std::addressof(data),
        sizeof(data));
}

FPD_DETAIL_NAMESPACE_END

#endif /* !defined(_FPD_IO_H) */
