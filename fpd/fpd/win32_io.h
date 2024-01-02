// <copyright file="win32_io.h" company="Visualisierungsinstitut der Universität Stuttgart">
// Copyright © 2024 Visualisierungsinstitut der Universität Stuttgart.
// Licensed under the MIT licence. See LICENCE file for details.
// </copyright>
// <author>Christoph Müller</author>

#pragma once

#if defined(_WIN32)
#include <memory>
#include <system_error>

#include <windows.h>

#include "fpd/abi.h"


FPD_NAMESPACE_BEGIN

/// <summary>
/// Implements Win32 API I/O behaviour.
/// </summary>
class win32_io final {

public:

    /// <summary>
    /// The native type of a file handle.
    /// </summary>
    typedef HANDLE handle_type;

    /// <summary>
    /// Initialises a new instance from an existing file handle.
    /// </summary>
    /// <param name="handle">The handle to move into the new instance.</param>
    explicit inline win32_io(_Inout_ handle_type&& handle) noexcept
            : _handle(handle) {
        handle = INVALID_HANDLE_VALUE;
    }

    /// <summary>
    /// Initialises a new instance by opening a file.
    /// </summary>
    /// <param name="path">The path to the file to be opened.</param>
    /// <param name="desired_access">The desired access to the file, which
    /// defaults to <c>GENERIC_READ<c/>.</param>
    /// <param name="share_mode">The desired share mode for the open file, which
    /// defaults to <c>FILE_SHARE_READ</c>.</param>
    /// <param name="create_disposition">Indicates how missing and existing
    /// files are handled. This defaults to <c>OPEN_EXISTING</c>.</param>
    /// <param name="flags">The flags for the file handle, which defaults to
    /// zero.</param>
    /// <exception cref="std::system_error">If the file could not be opened.
    /// </exception>
    inline win32_io(_In_z_ const wchar_t *path,
            _In_ const DWORD desired_access = GENERIC_READ,
            _In_ const DWORD share_mode = FILE_SHARE_READ,
            _In_ const DWORD create_disposition = OPEN_EXISTING,
            _In_ const DWORD flags = 0)
        : _handle(::CreateFileW(path, desired_access, share_mode, nullptr,
            create_disposition, flags, NULL)) {
        if (this->_handle == INVALID_HANDLE_VALUE) {
            throw std::system_error(::GetLastError(), std::system_category());
        }
    }

    /// <summary>
    /// Initialises a new instance by opening a file.
    /// </summary>
    /// <param name="path">The path to the file to be opened.</param>
    /// <param name="desired_access">The desired access to the file, which
    /// defaults to <c>GENERIC_READ<c/>.</param>
    /// <param name="share_mode">The desired share mode for the open file, which
    /// defaults to <c>FILE_SHARE_READ</c>.</param>
    /// <param name="create_disposition">Indicates how missing and existing
    /// files are handled. This defaults to <c>OPEN_EXISTING</c>.</param>
    /// <param name="flags">The flags for the file handle, which defaults to
    /// zero.</param>
    /// <exception cref="std::system_error">If the file could not be opened.
    /// </exception>
    inline win32_io(_In_z_ const char *path,
            _In_ const DWORD desired_access = GENERIC_READ,
            _In_ const DWORD share_mode = FILE_SHARE_READ,
            _In_ const DWORD create_disposition = OPEN_EXISTING,
            _In_ const DWORD flags = 0) 
        : _handle(::CreateFileA(path, desired_access, share_mode, nullptr,
            create_disposition, flags, NULL)) {
        if (this->_handle == INVALID_HANDLE_VALUE) {
            throw std::system_error(::GetLastError(), std::system_category());
        }
    }

    /// <summary>
    /// Initialise from move.
    /// </summary>
    /// <param name="rhs">The object to be moved.</param>
    inline win32_io(_Inout_ win32_io&& rhs) noexcept : _handle(rhs._handle) {
        rhs._handle = INVALID_HANDLE_VALUE;
    }

    /// <summary>
    /// Finalises the instance.
    /// </summary>
    inline ~win32_io(void) noexcept {
        ::CloseHandle(this->_handle);
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
    inline std::size_t read(_Out_writes_bytes_to_opt_(cnt, return) void *dst,
            _In_ const std::size_t cnt,
            _In_ const std::size_t offset) const {
        DWORD retval = 0;
        this->seek(offset);
        if (!::ReadFile(this->_handle, dst, static_cast<DWORD>(cnt), &retval,
                nullptr)) {
            throw std::system_error(::GetLastError(), std::system_category());
        }
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
            _In_reads_bytes_opt_(cnt) const void *src,
            _In_ const std::size_t cnt) const {
        DWORD retval = 0;
        this->seek(offset);
        if (!::WriteFile(this->_handle, src, static_cast<DWORD>(cnt), &retval,
                nullptr)) {
            throw std::system_error(::GetLastError(), std::system_category());
        }
        return retval;
    }

    /// <summary>
    /// Move assignment.
    /// </summary>
    /// <param name="rhs">The object to be moved.</param>
    /// <returns><c>*this</c>.</returns>
    inline win32_io& operator =(_Inout_ win32_io&& rhs) noexcept {
        if (this != std::addressof(rhs)) {
            this->_handle = rhs._handle;
            rhs._handle = INVALID_HANDLE_VALUE;
        }
        return *this;
    }

    /// <summary>
    /// Indicates whether the file handle is valid.
    /// </summary>
    /// <returns><c>true</c> if the file handle in the object is valid,
    /// <c>false</c> otherwise.</returns>
    inline operator bool(void) const noexcept {
        return (this->_handle != INVALID_HANDLE_VALUE);
    }

private:

    inline void seek(_In_ const std::size_t offset) const {
        LARGE_INTEGER o;
        o.QuadPart = static_cast<decltype(o.QuadPart)>(offset);
        if (!::SetFilePointerEx(this->_handle, o, nullptr, FILE_BEGIN)) {
            throw std::system_error(::GetLastError(),
                std::system_category());
        }
    }

    handle_type _handle;

};

FPD_NAMESPACE_END

#endif /* defined(_WIN32) */
