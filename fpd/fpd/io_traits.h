// <copyright file="io_traits.h" company="Visualisierungsinstitut der Universität Stuttgart">
// Copyright © 2024 - 2025 Visualisierungsinstitut der Universität Stuttgart.
// Licensed under the MIT licence. See LICENCE file for details.
// </copyright>
// <author>Christoph Müller</author>

#if !defined(_FPD_IO_TRAITS_H)
#define _FPD_IO_TRAITS_H
#pragma once

#include <algorithm>
#include <atomic>
#include <memory>
#include <numeric>
#include <system_error>
#include <thread>

#include <fcntl.h>

#if defined(FPD_WITH_DSTORAGE)
#include <dstorage.h>
#endif /* defined(FPD_WITH_DSTORAGE) */

#if defined(_WIN32)
#include <io.h>
#include <share.h>
#include <windows.h>
#else /* defined(_WIN32) */
#include <unistd.h>
#endif /* defined(_WIN32) */

#include <sys/stat.h>

#include "fpd/com_category.h"
#include "fpd/convert_string.h"


FPD_DETAIL_NAMESPACE_BEGIN

/// <summary>
/// Provides a common interface to different types of file handles. The traits
/// class also serves as an RAII wrapper around the respective handle.
/// </summary>
/// <typeparam name="THandle">The type of the file handle.</typeparam>
template<class THandle> class io_traits;


#if defined(FPD_WITH_DSTORAGE)
/// <summary>
/// Specialisation for DirectStorage.
/// </summary>
/// <remarks>
/// It is usually not reasonable to use this implementation for loading data to
/// the GPU, because it reads to main memory instead of GPU and it is blocking.
/// For actual application rendering FPD, applications should create a manual
/// I/O implementation that is integrated with the renderer. This class can be
/// used to initially read the meta data, but the I/O of the particles should
/// be asynchronous.
/// </remarks>
template<> class io_traits<IDStorageFile> {

public:

    /// <summary>
    /// The native type of a fence used to wait for I/O to complete.
    /// </summary>
    typedef ID3D12Fence *fence_type;

    /// <summary>
    /// The native type of a file handle.
    /// </summary>
    typedef IDStorageFile *handle_type;

    /// <summary>
    /// The type of the storage queue used to submit I/O requests.
    /// </summary>
    typedef IDStorageQueue *queue_type;

    /// <summary>
    /// Compress the data from <paramref name="begin" /> to
    /// <paramref name="end" /> as <paramref name="blocks" />.
    /// </summary>
    /// <typeparam name="TByte"></typeparam>
    /// <typeparam name="TIterator"></typeparam>
    /// <param name="format"></param>
    /// <param name="begin"></param>
    /// <param name="end"></param>
    /// <param name="blocks"></param>
    /// <returns></returns>
    template<class TByte, class TIterator>
    static std::vector<std::vector<std::enable_if_t<sizeof(TByte) == 1, TByte>>>
    compress(_In_ const DSTORAGE_COMPRESSION_FORMAT format,
        _In_ const TIterator begin,
        _In_ const TIterator end,
        _In_ std::size_t blocks);

    /// <summary>
    /// Gets a DirectStorage compression codec.
    /// </summary>
    /// <param name="format">The compression format, which must be GDeflate.
    /// </param>
    /// <returns>The compression codec.</returns>
    /// <exception cref="std::system_error">If the codec could not be retrieved.
    /// </exception>
    inline static _Ret_valid_ IDStorageCompressionCodec *get_codec(
        _In_ const DSTORAGE_COMPRESSION_FORMAT format);

    /// <summary>
    /// Gets a DirectStorage factory.
    /// </summary>
    /// <returns>A DirectStorage factory. The caller must release the object
    /// once it is no longer needed.</returns>
    /// <exception cref="std::system_error">If the factory could not be
    /// retrieved.</exception>
    inline static _Ret_valid_ IDStorageFactory *get_factory(void);

    /// <summary>
    /// Reads at most <paramref name="cnt" /> bytes from the specified offset in
    /// the specified file.
    /// </summary>
    /// <param name="dst">Receives the data that have been read.</param>
    /// <param name="cnt">The size of <paramref name="dst" /> in bytes.</param>
    /// <param name="handle">A DirectStorage file wrapped in I/O traits.</param>
    /// <param name="offset">The offset into the file from where to read in
    /// bytes from the begin of the file.</param>
    /// <returns>The number of bytes actually read.</returns>
    /// <exception cref="std::system_error">If either seeking to the desired
    /// location or reading from it failed.</exception>
    static inline std::size_t read(_Out_writes_bytes_to_(cnt, return) void *dst,
        _In_ const std::size_t cnt,
        _In_ io_traits& handle,
        _In_ const std::size_t offset);

    /// <summary>
    /// Initialises a new instance from an existing file handle.
    /// </summary>
    /// <param name="handle">A DirectStorage file handle. The object will
    /// increment the reference counter and does not take ownership of the
    /// caller's reference.</param>
    inline io_traits(_In_ handle_type handle, _In_ queue_type queue,
        _In_ fence_type fence);

    io_traits(const io_traits&) = delete;

    /// <summary>
    /// Initialise from move.
    /// </summary>
    /// <param name="rhs">The object to be moved.</param>
    inline io_traits(_Inout_ io_traits&& rhs) noexcept
            : _event(rhs._event),
            _fence(rhs._fence),
            _fence_value(rhs._fence_value),
            _handle(rhs._handle),
            _queue(rhs._queue) {
        rhs._event = NULL;
        rhs._fence = nullptr;
        rhs._fence_value = 0;
        rhs._handle = nullptr;
        rhs._queue = nullptr;
    }

    /// <summary>
    /// Finalises the instance.
    /// </summary>
    inline ~io_traits(void) noexcept {
        ::CloseHandle(this->_event);
        safe_release(this->_fence);
        safe_release(this->_handle);
        safe_release(this->_queue);
    }

    io_traits& operator =(const io_traits&) = delete;

    /// <summary>
    /// Move assignment.
    /// </summary>
    /// <param name="rhs">The object to be moved.</param>
    /// <returns><c>*this</c>.</returns>
    inline io_traits& operator =(_Inout_ io_traits&& rhs) noexcept;

    /// <summary>
    /// Indicates whether the file handle is valid.
    /// </summary>
    /// <returns><c>true</c> if the file handle in the object is valid,
    /// <c>false</c> otherwise.</returns>
    inline operator bool(void) const noexcept {
        return ((this->_event != NULL)
            && (this->_fence != nullptr)
            && (this->_handle != nullptr)
            && (this->_queue != nullptr));
    }

private:

    template<class TPtr>
    static inline void safe_add_ref(_Inout_opt_ TPtr *ptr) {
        if (ptr != nullptr) {
            ptr->AddRef();
        }
    }

    template<class TPtr>
    static inline void safe_release(_Inout_opt_ TPtr *ptr) {
        if (ptr != nullptr) {
            ptr->Release();
        }
    }

    HANDLE _event;
    fence_type _fence;
    std::uint64_t _fence_value;
    handle_type _handle;
    queue_type _queue;
};
#endif /* defined(FPD_WITH_DSTORAGE) */


/// <summary>
/// Specialisation for POSIX file handles.
/// </summary>
template<> class io_traits<int> {

public:

    /// <summary>
    /// The native type of a file handle.
    /// </summary>
    typedef int handle_type;

    /// <summary>
    /// Reads at most <paramref name="cnt" /> bytes from the current location in
    /// the specified file.
    /// </summary>
    /// <param name="dst">Receives the data that have been read.</param>
    /// <param name="cnt">The size of <paramref name="dst" /> in bytes.</param>
    /// <param name="handle">A file handle opened for reading.</param>
    /// <returns>The number of bytes actually read.</returns>
    /// <exception cref="std::system_error">In case the operation failed.
    /// </exception>
    static inline std::size_t read(_Out_writes_bytes_to_(cnt, return) void *dst,
        _In_ const std::size_t cnt,
        _In_ const handle_type handle);

    /// <summary>
    /// Reads at most <paramref name="cnt" /> bytes from the specified offset in
    /// the specified file.
    /// </summary>
    /// <param name="dst">Receives the data that have been read.</param>
    /// <param name="cnt">The size of <paramref name="dst" /> in bytes.</param>
    /// <param name="handle">A file handle opened for reading.</param>
    /// <param name="offset">The offset into the file from where to read in
    /// bytes from the begin of the file.</param>
    /// <returns>The number of bytes actually read.</returns>
    /// <exception cref="std::system_error">If either seeking to the desired
    /// location or reading from it failed.</exception>
    static inline std::size_t read(_Out_writes_bytes_to_(cnt, return) void *dst,
            _In_ const std::size_t cnt,
            _In_ const handle_type handle,
            _In_ const std::streamoff offset){
        seek(handle, offset);
        return read(dst, cnt, handle);
    }

    /// <summary>
    /// Seek to the specified location in a file.
    /// </summary>
    /// <param name="handle">A file handle.</param>
    /// <param name="offset">The offset to seek in bytes.</param>
    /// <param name="origin">The origin from where to seek, which defaults to
    /// the begin of the file.</param>
    static inline void seek(_In_ const handle_type handle,
        _In_ const std::streamoff offset,
        _In_ const int origin = SEEK_SET);

    /// <summary>
    /// Writes at most <paramref name="cnt" /> bytes from
    /// <paramref name="src" /> to the current location in a file.
    /// </summary>
    /// <param name="handle">A file handle opened for writing.</param>
    /// <param name="src">The location from where to get the data.</param>
    /// <param name="cnt">The number of bytes to write.</param>
    /// <returns>The number of bytes actually written.</returns>
    static inline std::size_t write(_In_ const handle_type handle,
        _In_reads_bytes_(cnt) const void *src,
        _In_ const std::size_t cnt);

    /// <summary>
    /// Writes at most <paramref name="cnt" /> bytes from
    /// <paramref name="src" /> to the specified location in a file.
    /// </summary>
    /// <param name="handle">A file handle opened for writing.</param>
    /// <param name="offset">The offset where to write the bytes to in bytes
    /// from the begin of the file.</param>
    /// <param name="src">The location from where to get the data.</param>
    /// <param name="cnt">The number of bytes to write.</param>
    /// <returns>The number of bytes actually written.</returns>
    static inline std::size_t write(_In_ const handle_type handle,
            _In_ const std::streamoff offset,
            _In_reads_bytes_(cnt) const void *src,
            _In_ const std::size_t cnt) {
        seek(handle, offset);
        return write(handle, src, cnt);
    }

    /// <summary>
    /// Initialises a new instance from an existing file handle.
    /// </summary>
    /// <param name="handle">The handle to move into the new instance.</param>
    explicit inline io_traits(_Inout_ handle_type&& handle) noexcept
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
    explicit inline io_traits(_In_z_ const wchar_t *path,
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
    explicit inline io_traits(_In_z_ const wchar_t *path,
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
    explicit inline io_traits(_In_z_ const char *path,
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
    explicit inline io_traits(_In_z_ const char *path,
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
    inline io_traits(_Inout_ io_traits&& rhs) noexcept : _handle(rhs._handle) {
        rhs._handle = -1;
    }

    /// <summary>
    /// Finalises the instance.
    /// </summary>
    inline ~io_traits(void) noexcept {
#if defined(_WIN32)
        ::_close(this->_handle);
#else /* defined(_WIN32) */
        ::close(this->_handle);
#endif /* defined(_WIN32) */
    }

    /// <summary>
    /// Move assignment.
    /// </summary>
    /// <param name="rhs">The object to be moved.</param>
    /// <returns><c>*this</c>.</returns>
    inline io_traits& operator =(_Inout_ io_traits&& rhs) noexcept {
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
        return (this->_handle >= 0);
    }

    /// <summary>
    /// Exposes the native file handle.
    /// </summary>
    /// <returns>The native file handle.</returns>
    inline operator handle_type(void) const noexcept {
        return this->_handle;
    }

private:

    handle_type _handle;
};


#if defined(_WIN32)
/// <summary>
/// Specialisation for native Win32 handles.
/// </summary>
template<> class io_traits<HANDLE> {

public:

    /// <summary>
    /// The native type of a file handle.
    /// </summary>
    typedef HANDLE handle_type;

    /// <summary>
    /// Reads at most <paramref name="cnt" /> bytes from the current location in
    /// the specified file.
    /// </summary>
    /// <param name="dst">Receives the data that have been read.</param>
    /// <param name="cnt">The size of <paramref name="dst" /> in bytes.</param>
    /// <param name="handle">A file handle opened for reading.</param>
    /// <returns>The number of bytes actually read.</returns>
    /// <exception cref="std::system_error">In case the operation failed.
    /// </exception>
    static inline std::size_t read(
        _Out_writes_bytes_to_opt_(cnt, return) void *dst,
        _In_ const std::size_t cnt,
        _In_ const handle_type handle);

    /// <summary>
    /// Reads at most <paramref name="cnt" /> bytes from the specified offset in
    /// the specified file.
    /// </summary>
    /// <param name="dst">Receives the data that have been read.</param>
    /// <param name="cnt">The size of <paramref name="dst" /> in bytes.</param>
    /// <param name="handle">A file handle opened for reading.</param>
    /// <param name="offset">The offset into the file from where to read in
    /// bytes from the begin of the file.</param>
    /// <returns>The number of bytes actually read.</returns>
    /// <exception cref="std::system_error">If either seeking to the desired
    /// location or reading from it failed.</exception>
    static inline std::size_t read(_Out_writes_bytes_to_(cnt, return) void *dst,
            _In_ const std::size_t cnt,
            _In_ const handle_type handle,
            _In_ const std::streamoff offset){
        seek(handle, offset);
        return read(dst, cnt, handle);
    }

    /// <summary>
    /// Seek to the specified location in a file.
    /// </summary>
    /// <param name="handle">A file handle.</param>
    /// <param name="offset">The offset to seek in bytes.</param>
    /// <param name="origin">The origin from where to seek, which defaults to
    /// the begin of the file.</param>
    static inline void seek(_In_ const handle_type handle,
        _In_ const std::streamoff offset,
        _In_ const DWORD origin = FILE_BEGIN);

    /// <summary>
    /// Writes at most <paramref name="cnt" /> bytes from
    /// <paramref name="src" /> to the current location in a file.
    /// </summary>
    /// <param name="handle">A file handle opened for writing.</param>
    /// <param name="src">The location from where to get the data.</param>
    /// <param name="cnt">The number of bytes to write.</param>
    /// <returns>The number of bytes actually written.</returns>
    static inline std::size_t write(_In_ const handle_type handle,
        _In_reads_bytes_opt_(cnt) const void *src,
        _In_ const std::size_t cnt);

    /// <summary>
    /// Writes at most <paramref name="cnt" /> bytes from
    /// <paramref name="src" /> to the specified location in a file.
    /// </summary>
    /// <param name="handle">A file handle opened for writing.</param>
    /// <param name="offset">The offset where to write the bytes to in bytes
    /// from the begin of the file.</param>
    /// <param name="src">The location from where to get the data.</param>
    /// <param name="cnt">The number of bytes to write.</param>
    /// <returns>The number of bytes actually written.</returns>
    static inline std::size_t write(_In_ const handle_type handle,
            _In_ const std::streamoff offset,
            _In_reads_bytes_(cnt) const void *src,
            _In_ const std::size_t cnt) {
        seek(handle, offset);
        return write(handle, src, cnt);
    }

    /// <summary>
    /// Initialises a new instance from an existing file handle.
    /// </summary>
    /// <param name="handle">The handle to move into the new instance.</param>
    explicit inline io_traits(_Inout_ handle_type&& handle) noexcept
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
    explicit inline io_traits(_In_z_ const wchar_t *path,
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
    explicit inline io_traits(_In_z_ const char *path,
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
    inline io_traits(_Inout_ io_traits&& rhs) noexcept : _handle(rhs._handle) {
        rhs._handle = INVALID_HANDLE_VALUE;
    }

    /// <summary>
    /// Finalises the instance.
    /// </summary>
    inline ~io_traits(void) noexcept {
        ::CloseHandle(this->_handle);
    }

    /// <summary>
    /// Move assignment.
    /// </summary>
    /// <param name="rhs">The object to be moved.</param>
    /// <returns><c>*this</c>.</returns>
    inline io_traits& operator =(_Inout_ io_traits&& rhs) noexcept {
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

    /// <summary>
    /// Exposes the native file handle.
    /// </summary>
    /// <returns>The native file handle.</returns>
    inline operator handle_type(void) const noexcept {
        return this->_handle;
    }

private:

    handle_type _handle;
};
#endif /* defined(_WIN32) */


/// <summary>
/// Collapses I/O traits created from I/O traits to the underlying traits.
/// </summary>
/// <typeparam name="THandle">The type of the file handle.</typeparam>
template<class THandle>
class io_traits<io_traits<THandle>> : public io_traits<THandle> { };


FPD_DETAIL_NAMESPACE_END

#include "fpd/io_traits.inl"

#endif /* !defined(_FPD_IO_TRAITS_H) */
