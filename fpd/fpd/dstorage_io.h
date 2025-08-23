// <copyright file="dstorage_io.h" company="Visualisierungsinstitut der Universität Stuttgart">
// Copyright © 2024 - 2025 Visualisierungsinstitut der Universität Stuttgart.
// Licensed under the MIT licence. See LICENCE file for details.
// </copyright>
// <author>Christoph Müller</author>

#if !defined(_FPD_DSTORAGE_IO_H)
#define _FPD_DSTORAGE_IO_H
#pragma once

#if defined(FPD_WITH_DSTORAGE)
#include <limits>
#include <system_error>

#include <dstorage.h>
#include <Windows.h>

#include "fpd/com_category.h"


FPD_NAMESPACE_BEGIN

/// <summary>
/// Implements DirectStorage I/O behaviour.
/// </summary>
/// <remarks>
/// It is usually not reasonable to use this implementation, because it reads
/// to main memory instead of GPU and it is blocking. For actual application
/// rendering FPD, applications should create a manual I/O implementation that
/// is integrated with the renderer. This class can be used to initially read
/// the meta data, but the I/O of the particles should be asynchronous.
/// </remarks>
class dstorage_io final {

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
    /// Gets a DirectStorage factory.
    /// </summary>
    /// <returns>A DirectStorage factory. The caller must release the object
    /// once it is no longer needed.</returns>
    /// <exception cref="std::system_error">If the factory could not be
    /// retrieved</exception>
    inline static _Ret_valid_ IDStorageFactory *get_factory(void) {
        IDStorageFactory *retval = nullptr;

        auto hr = ::DStorageGetFactory(IID_PPV_ARGS(&retval));
        if (FAILED(hr)) {
            throw std::system_error(hr, com_category());
        }

        return retval;
    }

    /// <summary>
    /// Initialises a new instance from an existing file handle.
    /// </summary>
    /// <param name="handle">A DirectStorage file handle. The object will
    /// increment the reference counter and does not take ownership of the
    /// caller's reference.</param>
    inline dstorage_io(_In_ handle_type handle,
            _In_ queue_type queue,
            _In_ fence_type fence)
        : _event(::CreateEvent(nullptr, FALSE, FALSE, nullptr)),
            _fence(fence),
            _fence_value(0),
            _handle(handle),
            _queue(queue) {
        if (this->_event == NULL) {
            throw std::system_error(::GetLastError(), std::system_category());
        }
        safe_add_ref(this->_fence);
        safe_add_ref(this->_handle);
        safe_add_ref(this->_queue);
    }

    dstorage_io(const dstorage_io&) = delete;


    /// <summary>
    /// Initialise from move.
    /// </summary>
    /// <param name="rhs">The object to be moved.</param>
    inline dstorage_io(_Inout_ dstorage_io&& rhs) noexcept
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
    inline ~dstorage_io(void) noexcept {
        ::CloseHandle(this->_event);
        safe_release(this->_fence);
        safe_release(this->_handle);
        safe_release(this->_queue);
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
            _In_ const std::size_t offset) {
        if (!*this) {
            throw std::system_error(ERROR_INVALID_HANDLE,
                std::system_category());
        }
        if (cnt > (std::numeric_limits<UINT>::max)()) {
            throw std::system_error(ERROR_INVALID_PARAMETER,
                std::system_category());
        }

        DSTORAGE_REQUEST request = { };
        request.Options.SourceType = DSTORAGE_REQUEST_SOURCE_FILE;
        request.Options.DestinationType = DSTORAGE_REQUEST_DESTINATION_MEMORY;
        request.Source.File.Source = this->_handle;
        request.Source.File.Offset = offset;
        request.Source.File.Size = static_cast<UINT>(cnt);
        request.UncompressedSize = static_cast<UINT>(cnt);
        request.Destination.Memory.Buffer = dst,
        request.Destination.Buffer.Offset = 0;
        request.Destination.Buffer.Size = static_cast<UINT>(cnt);

        assert(this->_queue != nullptr);
        this->_queue->EnqueueRequest(&request);

        const auto value = ++this->_fence_value;
        assert(this->_fence != nullptr);
        this->_fence->SetEventOnCompletion(value, this->_event);
        this->_queue->EnqueueSignal(this->_fence, value);
        this->_queue->Submit();

        ::WaitForSingleObject(this->_event, INFINITE);

        DSTORAGE_ERROR_RECORD errors;
        this->_queue->RetrieveErrorRecord(&errors);
        if (FAILED(errors.FirstFailure.HResult)) {
            throw std::system_error(errors.FirstFailure.HResult,
                com_category());
        }

        return cnt;
    }

    /// <summary>
    /// Does nothing, because DirectStorage is read-only.
    /// </summary>
    /// <param name="offset">The offset where to write the bytes to in bytes
    /// from the begin of the file.</param>
    /// <param name="src">The location from where to get the data.</param>
    /// <param name="cnt">The number of bytes to write.</param>
    /// <returns>The number of bytes actually written.</returns>
    /// <exception cref="std::system_error">Unconditionally.</exception>
    inline std::size_t write(_In_ const std::size_t offset,
            _In_reads_bytes_(cnt) const void *src,
            _In_ const std::size_t cnt) const {
        throw std::system_error(ERROR_FILE_READ_ONLY,
            std::system_category());
    }

    dstorage_io& operator =(const dstorage_io&) = delete;

    /// <summary>
    /// Move assignment.
    /// </summary>
    /// <param name="rhs">The object to be moved.</param>
    /// <returns><c>*this</c>.</returns>
    inline dstorage_io& operator =(_Inout_ dstorage_io&& rhs) noexcept {
        if (this != std::addressof(rhs)) {
            ::CloseHandle(this->_event);
            this->_event = rhs._event;
            rhs._event = NULL;

            safe_release(this->_fence);
            this->_fence = rhs._fence;
            rhs._fence = nullptr;

            this->_fence_value = rhs._fence_value;
            rhs._fence_value = 0;

            safe_release(this->_handle);
            this->_handle = rhs._handle;
            rhs._handle = nullptr;

            safe_release(this->_queue);
            this->_queue = rhs._queue;
            rhs._queue = nullptr;
        }
        return *this;
    }

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

FPD_NAMESPACE_END

#endif /* defined(FPD_WITH_DSTORAGE) */
#endif /* !defined(_FPD_DSTORAGE_IO_H) */

