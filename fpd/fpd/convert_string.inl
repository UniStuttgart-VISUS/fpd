// <copyright file="convert_string.inl" company="Visualisierungsinstitut der Universität Stuttgart">
// Copyright © 2023 - 2024 Visualisierungsinstitut der Universität Stuttgart.
// Licensed under the MIT licence. See LICENCE file for details.
// </copyright>
// <author>Christoph Müller</author>


/*
 * FPD_NAMESPACE::convert_string
 */
inline std::size_t FPD_NAMESPACE::convert_string(
        _Out_writes_to_opt_(cnt_dst, return) char16_t *dst,
        _In_ std::size_t cnt_dst,
        _In_reads_or_z_(cnt_src) const char *src,
        _In_ const std::ptrdiff_t cnt_src,
        _In_ const code_page code_page) {
#if defined(_WIN32)
    static_assert(sizeof(wchar_t) == sizeof(char16_t), "wchar_t is UTF-16.");
    return convert_string(reinterpret_cast<wchar_t *>(dst), cnt_dst,
        src, cnt_src, code_page);
#else /* defined(_WIN32) */
    UErrorCode status = U_ZERO_ERROR;

    // Create a converter for the code page specified by the caller.
    std::unique_ptr<UConverter, decltype(&::ucnv_close)> conv(
        ::ucnv_open(code_page, &status), ::ucnv_close);
    if (U_FAILURE(status)) {
        throw std::system_error(status, detail::icu_category());
    }

    // Perform the conversion.
    icu::UnicodeString unicode(src, s, conv.get(), status);
    if (U_FAILURE(status)) {
        throw std::system_error(status, detail::icu_category());
    }

    // Extract the characters to the caller's buffer.
    auto retval = unicode.extract(reinterpret_cast<Char16Ptr>(dst),
        static_cast<std::int32_t>(cnt_dst), status);
    switch (status) {
        case U_ZERO_ERROR:
        case U_BUFFER_OVERFLOW_ERROR:
            break;

        default:
            throw std::system_error(status, detail::icu_category());
    }

    return retval;
#endif /* defined(_WIN32) */
}


/*
 * FPD_NAMESPACE::convert_string
 */
inline std::size_t FPD_NAMESPACE::convert_string(
        _Out_writes_to_opt_(cnt_dst, return) wchar_t *dst,
        _In_ std::size_t cnt_dst,
        _In_reads_or_z_(cnt_src) const char *src,
        _In_ const std::ptrdiff_t cnt_src,
        _In_ const code_page code_page) {
    if (src == nullptr) {
        throw std::invalid_argument("The string to be converter cannot be "
            "null.");
    }

    const auto s = static_cast<int>(cnt_src);

#if defined(_WIN32)
    _try{
        if (dst == nullptr) {
            // Make sure that the output buffer size is zero if the output
            // buffer is invalid, such that we get the required buffer size.
            cnt_dst = 0;
        }

        auto retval = ::MultiByteToWideChar(static_cast<UINT>(code_page), 0,
            src, s,
            dst, static_cast<int>(cnt_dst));

        if (retval <= 0) {
            auto error = ::GetLastError();
            throw std::system_error(error, std::system_category());
        }

        return retval;
    } _except(EXCEPTION_EXECUTE_HANDLER) {
        throw std::system_error(ERROR_NOACCESS, std::system_category());
    }

#else /* defined(_WIN32) */
    UErrorCode status = U_ZERO_ERROR;

    // Create a converter for the code page specified by the caller.
    std::unique_ptr<UConverter, decltype(&::ucnv_close)> conv(
        ::ucnv_open(code_page, &status), ::ucnv_close);
    if (U_FAILURE(status)) {
        throw std::system_error(status, detail::icu_category());
    }

    // From https://unicode-org.github.io/icu/userguide/strings/: "The Unicode
    // standard defines a default encoding based on 16-bit code units. This is
    // supported in ICU by the definition of the UChar to be an unsigned 16-bit
    // integer type. This is the base type for character arrays for strings in
    // ICU." This means that libicu is using UTF-16 like Win32. Unfortunately,
    // wchar_t is UTF-32 on Linux, which means that we first need to convert to
    // UTF-16 in 'unicode' and from there to UTF-32. Even more unfortunate, we
    // need to do both conversions even for measuring the output buffer. While
    // it is highly unlikely that users of the library will use characters that
    // require two UTF-16 code units, relying on that would be similar to the
    // "MegaMol way" of "defining" that all strings are UTF-8 and hoping that
    // callers are using only 7-bit ASCII for it to actually work ...
    icu::UnicodeString unicode(src, s, conv.get(), status);
    if (U_FAILURE(status)) {
        throw std::system_error(status, detail::icu_category());
    }

    // Convert from UTF-16 to UTF-32.
    auto retval = unicode.toUTF32(reinterpret_cast<UChar32 *>(dst),
        static_cast<std::int32_t>(cnt_dst), status);
    switch (status) {
        case U_ZERO_ERROR:
        case U_BUFFER_OVERFLOW_ERROR:
            break;

        default:
            throw std::system_error(status, detail::icu_category());
    }

    return retval;
#endif /* defined(_WIN32) */
}


/*
 * FPD_NAMESPACE::convert_string
 */
inline std::size_t FPD_NAMESPACE::convert_string(
        _Out_writes_to_opt_(cnt_dst, return) char *dst,
        _In_ std::size_t cnt_dst,
        _In_reads_or_z_(cnt_src) const wchar_t *src,
        _In_ const std::ptrdiff_t cnt_src,
        _In_ const code_page code_page) {
    if (src == nullptr) {
        throw std::invalid_argument("The string to be converted cannot be "
            "null.");
    }

    const auto s = static_cast<int>(cnt_src);

#if defined(_WIN32)
    _try{
        if (dst == nullptr) {
            // Make sure that the output buffer size is zero if the output
            // buffer is invalid, such that we get the required buffer size.
            cnt_dst = 0;
        }

        BOOL replacement = FALSE;
        const auto cp = static_cast<UINT>(code_page);
        const auto r = ((cp != CP_UTF7) && (cp != CP_UTF8))
            ? &replacement
            : nullptr;

        auto retval = ::WideCharToMultiByte(cp,
            0,
            src, s,
            dst, static_cast<int>(cnt_dst),
            nullptr, r);

        if (retval <= 0) {
            auto error = ::GetLastError();
            throw std::system_error(error, std::system_category());
        }

        if (replacement) {
            // The input cannot be represented in the requested code page.
            throw std::system_error(ERROR_INVALID_PARAMETER,
                std::system_category());
        }

        return retval;
    } _except(EXCEPTION_EXECUTE_HANDLER) {
        throw std::system_error(ERROR_NOACCESS, std::system_category());
    }

#else /* defined(_WIN32) */
    UErrorCode status = U_ZERO_ERROR;

    // Create a UnicodeString, which we can subsequently convert to whatever
    // code page the user requested.
    auto unicode = icu::UnicodeString::fromUTF32(
        reinterpret_cast<const UChar32 *>(src), s);
    if (U_FAILURE(status)) {
        throw std::system_error(status, detail::icu_category());
    }

    // Create a converter for the code page specified by the caller.
    std::unique_ptr<Uconverter, decltype(&::ucnv_close)> conv(
        ::ucnv_open(code_page, &status), ::ucnv_close);
    if (U_FAILURE(status)) {
        throw std::system_error(status, detail::icu_category());
    }

    // As for the opposite direction, we need to pay for an intermediate
    // conversion to get from UTF-32 to the specified code page.
    auto retval = unicode.extract(dst, static_cast<std::int32_t>(cnt_dst),
        conv.get(), status);
    switch (status) {
        case U_ZERO_ERROR:
        case U_BUFFER_OVERFLOW_ERROR:
            break;

        default:
            throw std::system_error(status, detail::icu_category());
    }

    return retval;
#endif /* defined(_WIN32) */
}
