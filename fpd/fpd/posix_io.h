// <copyright file="posix_io.h" company="Visualisierungsinstitut der Universität Stuttgart">
// Copyright © 2024 Visualisierungsinstitut der Universität Stuttgart.
// Licensed under the MIT licence. See LICENCE file for details.
// </copyright>
// <author>Christoph Müller</author>

#pragma once

#include <memory>
#include <system_error>

#include <fcntl.h>

#if defined(_WIN32)
#include <io.h>
#include <share.h>
#else /* defined(_WIN32) */
#include <unistd.h>
#endif /* defined(_WIN32) */

#include <sys/stat.h>

#include "fpd/convert_string.h"


FPD_NAMESPACE_BEGIN

/// <summary>
/// Implements POSIX I/O behaviour.
/// </summary>
class posix_io final {

public:

    /// <summary>
    /// The native type of a file handle.
    /// </summary>
    typedef int handle_type;

    /// <summary>
    /// Initialises a new instance from an existing file handle.
    /// </summary>
    /// <param name="handle">The handle to move into the new instance.</param>
    explicit inline posix_io(_Inout_ handle_type&& handle) noexcept
            : _handle(handle) {
        handle = -1;
    }

    /// <summary>
    /// Initialises a new instance by opening a file.
    /// </summary>
    /// <param name="path">The path to the file to be opened.</param>
    /// <param name="flags">The desired access flags, which
    /// defaults to <c>_O_BINARY | _O_RDONLY | _O_SEQUENTIAL<c/>.</param>
    /// <param name="share">Defines the share mode, which defaults to
    /// <c>_SH_DENYWR</c>. This parameter has no effect on non-Windows
    /// platforms.</param>
    /// <param name="mode">The mode of newly created files. This parameter
    /// defaults to zero.</param>
    /// <exception cref="std::system_error">If the file could not be opened.
    /// </exception>
#if defined(_WIN32)
    inline posix_io(_In_z_ const wchar_t *path,
            _In_ const int flags = _O_BINARY | _O_RDONLY | _O_SEQUENTIAL,
            _In_ const int share = _SH_DENYWR,
            _In_ const int mode = 0)
            : _handle(-1) {
        auto status = ::_wsopen_s(&this->_handle, path, flags, share, mode);
        if (status != 0) {
            throw std::system_error(status, std::generic_category());
        }
    }
#else /* defined(_WIN32) */
    inline posix_io(_In_z_ const wchar_t *path,
            _In_ const int flags = O_RDONLY,
            _In_ const int share = 0,
            _In_ const mode_t mode = 0)
            : _handle(-1) {
        auto p = convert_string<char>(path);
        if ((this->_handle = ::open(p.c_str(), flags, mode)) == -1) {
            throw std::system_error(errno, std::system_category());
        }
    }
#endif /* defined(_WIN32) */

    /// <summary>
    /// Initialises a new instance by opening a file.
    /// </summary>
    /// <param name="path">The path to the file to be opened.</param>
    /// <param name="flags">The desired access flags, which
    /// defaults to <c>_O_BINARY | _O_RDONLY | _O_SEQUENTIAL<c/>.</param>
    /// <param name="mode">The mode of newly created files. This parameter
    /// defaults to zero.</param>
    /// <exception cref="std::system_error">If the file could not be opened.
    /// </exception>
#if defined(_WIN32)
    inline posix_io(_In_z_ const char *path,
            _In_ const int flags = _O_BINARY | _O_RDONLY | _O_SEQUENTIAL,
            _In_ const int share = _SH_DENYWR,
            _In_ const int mode = 0)
            : _handle(-1) {
        auto status = ::_sopen_s(&this->_handle, path, flags, share, mode);
        if (status != 0) {
            throw std::system_error(status, std::generic_category());
        }
    }
#else /* defined(_WIN32) */
    inline posix_io(_In_z_ const char *path,
            _In_ const int flags = O_RDONLY,
            _In_ const int share = 0,
            _In_ const mode_t mode = 0)
            : _handle(::open(path, flags, mode)) {
        if (this->_handle == -1) {
            throw std::system_error(errno, std::system_category());
        }
    }
#endif /* defined(_WIN32) */


    /// <summary>
    /// Initialise from move.
    /// </summary>
    /// <param name="rhs">The object to be moved.</param>
    inline posix_io(_Inout_ posix_io&& rhs) noexcept : _handle(rhs._handle) {
        rhs._handle = -1;
    }

    /// <summary>
    /// Finalises the instance.
    /// </summary>
    inline ~posix_io(void) noexcept {
#if defined(_WIN32)
        ::_close(this->_handle);
#else /* defined(_WIN32) */
        ::close(this->_handle);
#endif /* defined(_WIN32) */
    }

    /// <summary>
    /// Reads at most <paramref name="cnt" /> bytes from the specified offset in
    /// the file.
    /// </summary>
    /// <param name="dst">Receives the data that have been read.</param>
    /// <param name="cnt">The size of <paramref name="dst" /> in bytes.</param>
    /// <param name="offset">The offset into the file from where to read in
    /// bytes from the begin of the file.</param>
    /// <returns>The number of bytes actually read.</returns>
    /// <exception cref="std::system_error">If either seeking to the desired
    /// location or reading from it failed.</exception>
    inline std::size_t read(_Out_writes_bytes_to_(cnt, return) void *dst,
            _In_ const std::size_t cnt,
            _In_ const std::size_t offset) const {
        this->seek(offset);
#if defined(_WIN32)
        auto retval = ::_read(this->_handle, dst,
            static_cast<unsigned int>(cnt));
        if (retval < 0) {
            throw std::system_error(errno, std::generic_category());
        }
#else /* defined(_WIN32) */
        auto retval = ::read(this->_handle, dst, cnt);
        if (retval < 0) {
            throw std::system_error(errno, std::system_category());
        }
#endif /* defined(_WIN32) */
        return retval;
    }

    /// <summary>
    /// Writes at most <paramref name="cnt" /> bytes from
    /// <paramref name="src" /> to the specified location in the file.
    /// </summary>
    /// <param name="offset">The offset where to write the bytes to in bytes
    /// from the begin of the file.</param>
    /// <param name="src">The location from where to get the data.</param>
    /// <param name="cnt">The number of bytes to write.</param>
    /// <returns>The number of bytes actually written.</returns>
    inline std::size_t write(_In_ const std::size_t offset,
            _In_reads_bytes_(cnt) const void *src,
            _In_ const std::size_t cnt) const {
        this->seek(offset);
#if defined(_WIN32)
        auto retval = ::_write(this->_handle, src,
            static_cast<unsigned int>(cnt));
        if (retval < 0) {
            throw std::system_error(errno, std::generic_category());
        }
#else /* defined(_WIN32) */
        auto retval = ::write(this->_handle, src, cnt);
        if (retval < 0) {
            throw std::system_error(errno, std::system_category());
        }
#endif /* defined(_WIN32) */
        return retval;
    }

    /// <summary>
    /// Move assignment.
    /// </summary>
    /// <param name="rhs">The object to be moved.</param>
    /// <returns><c>*this</c>.</returns>
    inline posix_io& operator =(_Inout_ posix_io&& rhs) noexcept {
        if (this != std::addressof(rhs)) {
            this->_handle = rhs._handle;
            rhs._handle = -1;
        }
        return *this;
    }

    /// <summary>
    /// Indicates whether the file handle is valid.
    /// </summary>
    /// <returns><c>true</c> if the file handle in the object is valid,
    /// <c>false</c> otherwise.</returns>
    inline operator bool(void) const noexcept {
        return (this->_handle < 0);
    }

private:

    inline void seek(_In_ const std::size_t offset) const {
#if defined(_WIN32)
        if (::_lseeki64(this->_handle, offset, SEEK_SET) < 0) {
            throw std::system_error(errno, std::generic_category());
        }
#else /* defined(_WIN32) */
        if (!::lseek(this->_handle, offset, SEEK_SET) < 0) {
            throw std::system_error(errno, std::system_category());
        }
#endif /* defined(_WIN32) */
    }

    handle_type _handle;

};

FPD_NAMESPACE_END
