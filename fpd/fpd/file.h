// <copyright file="file.h" company="Visualisierungsinstitut der Universität Stuttgart">
// Copyright © 2024 Visualisierungsinstitut der Universität Stuttgart.
// Licensed under the MIT licence. See LICENCE file for details.
// </copyright>
// <author>Christoph Müller</author>

#if !defined(_FPD_FILE_H)
#define _FPD_FILE_H
#pragma once

#include <cinttypes>
#include <utility>
#include <vector>

#include "fpd/meta_data.h"


FPD_NAMESPACE_BEGIN

/// <summary>
/// This class represents an FDP file.
/// </summary>
/// <typeparam name="TChar">The type of a character used for strings in the
/// meta data.</typeparam>
/// <typeparam name="TIoTraits">The I/O traits used to retrieve data from disk.
/// </typeparam>
template<class TChar, class TIoTraits> class file final {

public:

    /// <summary>
    /// The type of a character used for strings in the meta data.
    /// </summary>
    typedef TChar char_type;

    /// <summary>
    /// The type of the meta data read from the file.
    /// </summary>
    typedef meta_data<char_type> meta_data_type;

    /// <summary>
    /// The type of the I/O API.
    /// </summary>
    typedef TIoTraits io_type;

    /// <summary>
    /// Initialises a new instance.
    /// </summary>
    /// <typeparam name="TArgs">The type of the arguments passed to the I/O
    /// constructor.</typeparam>
    /// <param name="args">The arguments passed to the I/O constructors. This
    /// depends on <see cref="io_type" /> and its constructor.</param>
    template<class... TArgs>
    explicit inline file(_In_ TArgs&&... args)
            : _io(std::forward<TArgs>(args)...) {
        this->read_meta_data();
    }

    /// <summary>
    /// Initialises a new instance.
    /// </summary>
    /// <typeparam name="TArgs">The type of the arguments passed to the I/O
    /// constructor.</typeparam>
    /// <param name="cp">The code page used for narrow strings. This parameter
    /// has no effect if the <see cref="char_type" /> is UTF-16 or UTF-32.
    /// </param>
    /// <param name="args">The arguments passed to the I/O constructors. This
    /// depends on <see cref="io_type" /> and its constructor.</param>
    template<class... TArgs>
    explicit inline file(_In_ const code_page cp, _In_ TArgs&&... args)
            : _io(std::forward<TArgs>(args)...), _meta_data(cp) {
        this->read_meta_data();
    }

    /// <summary>
    /// Initialises a new instance.
    /// </summary>
    /// <typeparam name="TArgs">The type of the arguments passed to the I/O
    /// constructor.</typeparam>
    /// <param name="expected_header_size">The expected size of the file header
    /// in bytes. If this is a good guess, the constructor can process the
    /// header with a single read request. Otherwise, two reads will be
    /// required.</param>
    /// <param name="args">The arguments passed to the I/O constructors. This
    /// depends on <see cref="io_type" /> and its constructor.</param>
    template<class... TArgs>
    explicit inline file(_In_ const std::size_t expected_header_size,
            _In_ TArgs&&... args) : _io(std::forward<TArgs>(args)...) {
        this->read_meta_data(expected_header_size);
    }

    /// <summary>
    /// Initialises a new instance.
    /// </summary>
    /// <typeparam name="TArgs">The type of the arguments passed to the I/O
    /// constructor.</typeparam>
    /// <param name="cp">The code page used for narrow strings. This parameter
    /// has no effect if the <see cref="char_type" /> is UTF-16 or UTF-32.
    /// </param>
    /// <param name="expected_header_size">The expected size of the file header
    /// in bytes. If this is a good guess, the constructor can process the
    /// header with a single read request. Otherwise, two reads will be
    /// required.</param>
    /// <param name="args">The arguments passed to the I/O constructors. This
    /// depends on <see cref="io_type" /> and its constructor.</param>
    template<class... TArgs>
    explicit inline file(_In_ const code_page cp,
            _In_ const std::size_t expected_header_size,
            _In_ TArgs&&... args)
            : _io(std::forward<TArgs>(args)...), _meta_data(cp) {
        this->read_meta_data(expected_header_size);
    }

    /// <summary>
    /// Gets the meta data of the FPD file.
    /// </summary>
    /// <returns>The meta data read from the begin of the file.</returns>
    inline const meta_data_type& meta_data(void) const noexcept {
        return this->_meta_data;
    }

private:

    void read(_Out_writes_all_(cnt) std::uint8_t *dst,
        _In_ const std::size_t cnt,
        _In_ std::size_t from) const;

    void read_meta_data(_In_ const std::size_t expected_header_size = 0);

    io_type _io;
    meta_data_type _meta_data;
};

FPD_NAMESPACE_END

#include "fpd/file.inl"

#endif /* !defined(_FPD_FILE_H) */
