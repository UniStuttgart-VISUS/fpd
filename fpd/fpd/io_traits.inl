// <copyright file="io_traits.inl" company="Visualisierungsinstitut der Universität Stuttgart">
// Copyright © 2024 - 2025 Visualisierungsinstitut der Universität Stuttgart.
// Licensed under the MIT licence. See LICENCE file for details.
// </copyright>
// <author>Christoph Müller</author>


#if defined(FPD_WITH_DSTORAGE)
/*
 * FPD_DETAIL_NAMESPACE::io_traits<IDStorageFile>::get_factory
 */
_Ret_valid_ IDStorageFactory *
FPD_DETAIL_NAMESPACE::io_traits<IDStorageFile>::get_factory(void) {
    IDStorageFactory *retval = nullptr;

    auto hr = ::DStorageGetFactory(IID_PPV_ARGS(&retval));
    if (FAILED(hr)) {
        throw std::system_error(hr, com_category());
    }

    return retval;
}


/*
 * FPD_DETAIL_NAMESPACE::io_traits<IDStorageFile>::io_traits
 */
inline FPD_DETAIL_NAMESPACE::io_traits<IDStorageFile>::io_traits(
        _In_ handle_type handle,
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


/*
 * FPD_DETAIL_NAMESPACE::io_traits<IDStorageFile>::read
 */
std::size_t FPD_DETAIL_NAMESPACE::io_traits<IDStorageFile>::read(
        _Out_writes_bytes_to_(cnt, return) void *dst,
        _In_ const std::size_t cnt,
        _In_ io_traits& handle,
        _In_ const std::size_t offset) {
    if (cnt > (std::numeric_limits<UINT>::max)()) {
        throw std::system_error(ERROR_INVALID_PARAMETER,
            std::system_category());
    }

    DSTORAGE_REQUEST request = { };
    request.Options.SourceType = DSTORAGE_REQUEST_SOURCE_FILE;
    request.Options.DestinationType = DSTORAGE_REQUEST_DESTINATION_MEMORY;
    request.Source.File.Source = handle._handle;
    request.Source.File.Offset = offset;
    request.Source.File.Size = static_cast<UINT>(cnt);
    request.UncompressedSize = static_cast<UINT>(cnt);
    request.Destination.Memory.Buffer = dst,
        request.Destination.Buffer.Offset = 0;
    request.Destination.Buffer.Size = static_cast<UINT>(cnt);

    assert(handle._queue != nullptr);
    handle._queue->EnqueueRequest(&request);

    const auto value = ++handle._fence_value;
    assert(handle._fence != nullptr);
    handle._fence->SetEventOnCompletion(value, handle._event);
    handle._queue->EnqueueSignal(handle._fence, value);
    handle._queue->Submit();

    ::WaitForSingleObject(handle._event, INFINITE);

    DSTORAGE_ERROR_RECORD errors;
    handle._queue->RetrieveErrorRecord(&errors);
    if (FAILED(errors.FirstFailure.HResult)) {
        throw std::system_error(errors.FirstFailure.HResult,
            com_category());
    }

    return cnt;
}


/*
 * FPD_DETAIL_NAMESPACE::io_traits<IDStorageFile>::operator =
 */
FPD_DETAIL_NAMESPACE::io_traits<IDStorageFile>&
FPD_DETAIL_NAMESPACE::io_traits<IDStorageFile>::operator =(
        _Inout_ io_traits&& rhs) noexcept {
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

#endif /* defined(FPD_WITH_DSTORAGE) */


/*
 * FPD_DETAIL_NAMESPACE::io_traits<int>::read
 */
std::size_t FPD_DETAIL_NAMESPACE::io_traits<int>::read(
        _Out_writes_bytes_to_(cnt, return) void *dst,
        _In_ const std::size_t cnt,
        _In_ const handle_type handle) {
#if defined(_WIN32)
    auto retval = ::_read(handle, dst, static_cast<unsigned int>(cnt));
    if (retval < 0) {
        throw std::system_error(errno, std::generic_category());
    }
#else /* defined(_WIN32) */
    auto retval = ::read(handle, dst, cnt);
    if (retval < 0) {
        throw std::system_error(errno, std::system_category());
    }
#endif /* defined(_WIN32) */
    return retval;
}


/*
 * FPD_DETAIL_NAMESPACE::io_traits<int>::seek
 */
void FPD_DETAIL_NAMESPACE::io_traits<int>::seek(
        _In_ const handle_type handle,
        _In_ const std::streamoff offset,
        _In_ const int origin) {
#if defined(_WIN32)
    if (::_lseeki64(handle, offset, origin) < 0) {
        throw std::system_error(errno, std::generic_category());
    }
#else /* defined(_WIN32) */
    if (::lseek(handle, offset, origin) < 0) {
        throw std::system_error(errno, std::system_category());
    }
#endif /* defined(_WIN32) */
}


/*
 * FPD_DETAIL_NAMESPACE::io_traits<int>::write
 */
std::size_t FPD_DETAIL_NAMESPACE::io_traits<int>::write(
        _In_ const handle_type handle,
        _In_reads_bytes_(cnt) const void *src,
        _In_ const std::size_t cnt) {
#if defined(_WIN32)
    auto retval = ::_write(handle, src, static_cast<unsigned int>(cnt));
    if (retval < 0) {
        throw std::system_error(errno, std::generic_category());
    }
#else /* defined(_WIN32) */
    auto retval = ::write(handle, src, cnt);
    if (retval < 0) {
        throw std::system_error(errno, std::system_category());
    }
#endif /* defined(_WIN32) */
    return retval;
}


#if defined(_WIN32)
/*
 * FPD_DETAIL_NAMESPACE::io_traits<HANDLE>::read
 */
std::size_t FPD_DETAIL_NAMESPACE::io_traits<HANDLE>::read(
        _Out_writes_bytes_to_opt_(cnt, return) void *dst,
        _In_ const std::size_t cnt,
        _In_ const handle_type handle) {
    DWORD retval = 0;
    if (!::ReadFile(handle, dst, static_cast<DWORD>(cnt), &retval, nullptr)) {
        throw std::system_error(::GetLastError(), std::system_category());
    }
    return retval;
}


/*
 * FPD_DETAIL_NAMESPACE::io_traits<HANDLE>::seek
 */
void FPD_DETAIL_NAMESPACE::io_traits<HANDLE>::seek(
        _In_ const handle_type handle,
        _In_ const std::streamoff offset,
        _In_ const DWORD origin) {
    LARGE_INTEGER o;
    o.QuadPart = static_cast<decltype(o.QuadPart)>(offset);
    if (!::SetFilePointerEx(handle, o, nullptr, origin)) {
        throw std::system_error(::GetLastError(),
            std::system_category());
    }
}


/*
 * FPD_DETAIL_NAMESPACE::io_traits<HANDLE>::write
 */
std::size_t FPD_DETAIL_NAMESPACE::io_traits<HANDLE>::write(
        _In_ const handle_type handle,
        _In_reads_bytes_opt_(cnt) const void *src,
        _In_ const std::size_t cnt) {
    DWORD retval = 0;
    if (!::WriteFile(handle, src, static_cast<DWORD>(cnt), &retval, nullptr)) {
        throw std::system_error(::GetLastError(), std::system_category());
    }
    return retval;
}
#endif /* defined(_WIN32) */
