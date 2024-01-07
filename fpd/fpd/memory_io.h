// <copyright file="memory_io.h" company="Visualisierungsinstitut der Universität Stuttgart">
// Copyright © 2024 Visualisierungsinstitut der Universität Stuttgart.
// Licensed under the MIT licence. See LICENCE file for details.
// </copyright>
// <author>Christoph Müller</author>

#pragma once

#include <algorithm>
#include <cstdlib>
#include <memory>
#include <stdexcept>
#include <system_error>
#include <type_traits>
#include <utility>

#if defined(_WIN32)
#include <Windows.h>
#else /* defined(_WIN32) */
#include <errno.h>
#endif /* defined(_WIN32) */

#include "fpd/abi.h"


FPD_NAMESPACE_BEGIN

/// <summary>
/// Implements I/O operations on a fixed-size memory buffer.
/// </summary>
/// <typeparam name="THandle">The type of the pointer, which is basically
/// used to determine whether the data are constant or not. Usually, this
/// should be <c>void</c> or <c>const void</c></typeparam>
template<class THandle> class basic_memory_io final {

public:

    /// <summary>
    /// The pointer type used to reference the data.
    /// </summary>
    typedef THandle *handle_type;

    /// <summary>
    /// The type of the pointer used to internally reference the data.
    /// </summary>
    typedef typename std::conditional<std::is_const<THandle>::value,
        const std::uint8_t *, std::uint8_t *>::type pointer_type;

    /// <summary>
    /// Initialises a new instance from a buffer.
    /// </summary>
    /// <param name="data">The pointer to the buffer. The caller must make
    /// sure that these data remain valid as long as the object is in use.
    /// </param name="cnt">The size of the <pararmef name="data" /> buffer
    /// in bytes.</param>
    inline basic_memory_io(_In_reads_(cnt) handle_type data,
            _In_ const std::size_t cnt) noexcept
        : _data(reinterpret_cast<pointer_type>(data)), _size(cnt) { }

    /// <summary>
    /// Initialise from move.
    /// </summary>
    /// <param name="rhs">The object to be moved.</param>
    inline basic_memory_io(_Inout_ basic_memory_io&& rhs) noexcept
            : _data(rhs._data), _size(rhs._size) {
        rhs._data = nullptr;
        rhs._size = 0;
    }

    /// <summary>
    /// Reads at most <paramref name="cnt" /> bytes from the specified offset in
    /// the underlying buffer.
    /// </summary>
    /// <param name="dst">Receives the data that have been read.</param>
    /// <param name="cnt">The size of <paramref name="dst" /> in bytes.</param>
    /// <param name="offset">The offset into the buffer from where to read in
    /// bytes.</param>
    /// <returns>The number of bytes actually read.</returns>
    /// <exception cref="std::system_error">If either seeking to the desired
    /// location failed.</exception>
    inline std::size_t read(_Out_writes_bytes_to_(cnt, return) void *dst,
            _In_ const std::size_t cnt,
            _In_ const std::size_t offset) const {
        this->seek(offset);
        assert(offset < this->_size);
        auto src = this->_data + offset;
        const auto retval = (std::min)(cnt, this->_size - offset);
        ::memcpy(dst, src, retval);
        return retval;
    }

    /// <summary>
    /// Writes at most <paramref name="cnt" /> bytes from
    /// <paramref name="src" /> to the specified location in the buffer.
    /// </summary>
    /// <param name="offset">The offset where to write the data to, in bytes.
    /// </param>
    /// <param name="src">The location from where to get the data.</param>
    /// <param name="cnt">The number of bytes to write.</param>
    /// <returns>The number of bytes actually written.</returns>
    inline std::size_t write(_In_ const std::size_t offset,
            _In_reads_bytes_(cnt) const void *src,
            _In_ const std::size_t cnt) {
        this->seek(offset);
        assert(offset < this->_size);
        auto dst = this->_data + offset;
        const auto retval = (std::min)(cnt, this->_size - offset);
        this->write(dst, src, retval);
        return retval;
    }

    /// <summary>
    /// Move assignment.
    /// </summary>
    /// <param name="rhs">The object to be moved.</param>
    /// <returns><c>*this</c>.</returns>
    inline basic_memory_io& operator =(_Inout_ basic_memory_io&& rhs) noexcept {
        if (this != std::addressof(rhs)) {
            this->_data = rhs._data;
            rhs._data = nullptr;
            this->_size = rhs._size;
            rhs._size = 0;
        }
        return *this;
    }

    /// <summary>
    /// Indicates whether the data are valid.
    /// </summary>
    /// <returns><c>true</c> if the object contains valid data, <c>false</c>
    /// otherwise.</returns>
    inline operator bool(void) const noexcept {
        return (this->_data != nullptr);
    }

private:

    inline void seek(_In_ const std::size_t offset) const {
        if (offset >= this->_size) {
#if defined(_WIN32)
            throw std::system_error(ERROR_INVALID_PARAMETER,
                std::system_category());
#else /* defined(_WIN32) */
            throw std::system_error(EINVAL, std::system_category());
#endif /* defined(_WIN32) */
        }
    }

    template<class TPointer>
    inline typename std::enable_if<std::is_const<TPointer>::value>::type
    write(_In_opt_ TPointer *dst,
            _In_opt_ const void *src,
            _In_ const std::size_t cnt) {
#if defined(_WIN32)
        throw std::system_error(ERROR_FILE_READ_ONLY,
            std::system_category());
#else /* defined(_WIN32) */
        throw std::system_error(EROFS, std::system_category());
#endif /* defined(_WIN32) */
    }

    template<class TPointer>
    inline typename std::enable_if<!std::is_const<TPointer>::value>::type
    write(_Out_writes_bytes_all_(cnt) TPointer *dst,
            _In_reads_bytes_(cnt) const void *src,
            _In_ const std::size_t cnt) {
        assert(dst != nullptr);
        assert(src != nullptr);
        ::memcpy(dst, src, cnt);
    }

    pointer_type _data;
    std::size_t _size;
};


/// <summary>
/// A memory-backed implementation of the I/O traits that allows for reading
/// from and writing to a user-provided buffer.
/// </summary>
typedef basic_memory_io<void> memory_io;


/// <summary>
/// A memory-backed implementation of the I/O traits that allows for reading
/// from a user-provided buffer.
/// </summary>
typedef basic_memory_io<const void> read_only_memory_io;

FPD_NAMESPACE_END
