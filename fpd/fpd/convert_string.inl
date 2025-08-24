// <copyright file="convert_string.inl" company="Visualisierungsinstitut der Universität Stuttgart">
// Copyright © 2023 - 2025 Visualisierungsinstitut der Universität Stuttgart.
// Licensed under the MIT licence. See LICENCE file for details.
// </copyright>
// <author>Christoph Müller</author>


/*
 * FPD_NAMESPACE::convert_string
 */
template<class TUnicode>
std::enable_if_t<sizeof(TUnicode) == sizeof(FPD_NAMESPACE::utf16_type),
    std::size_t>
FPD_NAMESPACE::convert_string(
        _Out_writes_to_opt_(cnt_dst, return) TUnicode *dst,
        _In_ std::size_t cnt_dst,
        _In_reads_or_z_(cnt_src) const char *src,
        _In_ int cnt_src,
        _In_ const code_page code_page) {
    if (dst == nullptr) {
        // Make sure that the output buffer size is zero if the output
        // buffer is invalid, such that we get the required buffer size.
        cnt_dst = 0;
    }
    if (src == nullptr) {
        // Likewise, if the source is invalid, bail out directly.
        return 0;
    }

    if ((cnt_src < 0) && (src != nullptr)) {
        // User requested us to measure the input, do so now.
        cnt_src = static_cast<int>(std::char_traits<char>::length(src)) + 1;
    }

    assert(src != nullptr);
    assert(cnt_src > 0);

    auto status = U_ZERO_ERROR;

    // Create a converter for the code page specified by the caller.
    std::unique_ptr<UConverter, decltype(&::ucnv_close)> conv(
        ::ucnv_open(code_page, &status), ::ucnv_close);
    detail::throw_if_icu_failed(status);

    // Perform a stream conversion, possibly into an intermediate buffer just
    // for measuring.
    auto dst_cur = reinterpret_cast<utf16_type *>(dst);
    auto dst_end = dst_cur + cnt_dst;
    auto src_cur = src;
    auto src_end = src + cnt_src;

    ::ucnv_toUnicode(conv.get(),
        &dst_cur, dst_end,
        &src_cur, src_end,
        nullptr, true, &status);

    auto retval = (dst_cur - reinterpret_cast<utf16_type *>(dst));

    if (status == U_BUFFER_OVERFLOW_ERROR) {
        // If the output buffer was too small, create an intermediate one and
        // measure how much we need. ICU typically uses 1024 for this case, so
        // we do so, too.
        std::array<utf16_type, 1024> buffer;
        dst_end = buffer.data() + buffer.size();

        do {
            dst_cur = buffer.data();
            status = U_ZERO_ERROR;
            ::ucnv_toUnicode(conv.get(),
                &dst_cur, dst_end,
                &src_cur, src_end,
                nullptr, true, &status);
            retval += dst_cur - buffer.data();
        } while (status == U_BUFFER_OVERFLOW_ERROR);
    }

    // If we are still in an error state here, something other than too little
    // output storage must have gone wrong.
    detail::throw_if_icu_failed(status);

    // 'retval' should now be the number of output characters.
    return retval;
}


/*
 * FPD_NAMESPACE::convert_string
 */
template<class TUnicode>
std::enable_if_t<sizeof(TUnicode) == sizeof(FPD_NAMESPACE::utf16_type),
    std::size_t>
FPD_NAMESPACE::convert_string(
        _Out_writes_to_opt_(cnt_dst, return) char *dst,
        _In_ std::size_t cnt_dst,
        _In_reads_or_z_opt_(cnt_src) const TUnicode *src,
        _In_ int cnt_src,
        _In_opt_z_ const code_page code_page) {
    if (dst == nullptr) {
        // Make sure that the output buffer size is zero if the output
        // buffer is invalid, such that we get the required buffer size.
        cnt_dst = 0;
    }
    if (src == nullptr) {
        // Likewise, if the source is invalid, bail out directly.
        return 0;
    }

    if ((cnt_src < 0) && (src != nullptr)) {
        // User requested us to measure the input, do so now.
        cnt_src = static_cast<int>(std::char_traits<TUnicode>::length(src)) + 1;
    }

    assert(src != nullptr);
    assert(cnt_src > 0);

    auto status = U_ZERO_ERROR;

    // Create a converter for the code page specified by the caller.
    std::unique_ptr<UConverter, decltype(&::ucnv_close)> conv(
        ::ucnv_open(code_page, &status), ::ucnv_close);
    detail::throw_if_icu_failed(status);

    // Perform a stream conversion, possibly into an intermediate buffer just
    // for measuring.
    auto cur_dst = dst;
    auto dst_end = dst + cnt_dst;
    auto cur_src = reinterpret_cast<const utf16_type *>(src);
    auto src_end = cur_src + cnt_src;

    ::ucnv_fromUnicode(conv.get(),
        &cur_dst, dst_end,
        &cur_src, src_end,
        nullptr, true, &status);

    auto retval = (cur_dst - dst);

    if (status == U_BUFFER_OVERFLOW_ERROR) {
        // If the output buffer was too small, create an intermediate one and
        // measure how much we need. ICU typically uses 1024 for this case, so
        // we do so, too.
        std::array<char, 1024> buffer;
        dst_end = buffer.data() + buffer.size();

        do {
            cur_dst = buffer.data();
            status = U_ZERO_ERROR;
            ::ucnv_fromUnicode(conv.get(),
                &cur_dst, dst_end,
                &cur_src, src_end,
                nullptr, true, &status);
            retval += cur_dst - buffer.data();
        } while (status == U_BUFFER_OVERFLOW_ERROR);
    }

    // If we are still in an error state here, something other than too little
    // output storage must have gone wrong.
    detail::throw_if_icu_failed(status);

    return retval;
}


/*
 * FPD_NAMESPACE::convert_string
 */
template<class TTo, class TFrom>
typename std::enable_if<(sizeof(TTo) == 1) && (sizeof(TFrom) == 1),
    std::size_t>::type
FPD_NAMESPACE::convert_string(
        _Out_writes_to_opt_(cnt_dst, return) TTo *dst,
        _In_ std::size_t cnt_dst,
        _In_opt_z_ const code_page dst_code_page,
        _In_reads_or_z_opt_(cnt_src) const TFrom *src,
        _In_ int cnt_src,
        _In_opt_z_ const code_page src_code_page) {
    if (dst == nullptr) {
        // Make sure that the output buffer size is zero if the output
        // buffer is invalid, such that we get the required buffer size.
        cnt_dst = 0;
    }
    if (src == nullptr) {
        // Likewise, if the source is invalid, bail out directly.
        return 0;
    }

    assert(src != nullptr);
    assert(cnt_dst <= (std::numeric_limits<std::int32_t>::max)());
    assert(cnt_src <= (std::numeric_limits<std::int32_t>::max)());
    auto status = U_ZERO_ERROR;

    auto retval = ::ucnv_convert(dst_code_page, src_code_page,
        reinterpret_cast<char *>(dst), static_cast<std::int32_t>(cnt_dst),
        reinterpret_cast<const char *>(src), static_cast<int32_t>(cnt_src),
        &status);

    if ((status != U_BUFFER_OVERFLOW_ERROR) || (cnt_dst > 0)) {
        // Throw if an error other than insufficient was encountered.
        detail::throw_if_icu_failed(status);
    }

    return retval;
}


/*
 * FPD_NAMESPACE::convert_string
 */
template<class TUnicode>
std::enable_if_t<sizeof(TUnicode) == sizeof(char32_t), std::size_t>
FPD_NAMESPACE::convert_string(
        _Out_writes_to_opt_(cnt_dst, return) TUnicode *dst,
        _In_ std::size_t cnt_dst,
        _In_reads_or_z_opt_(cnt_src) const char *src,
        _In_ int cnt_src,
        _In_ const code_page code_page) {
    static constexpr auto wchar_code_page = (endian::system == endian::big)
        ? code_pages::utf32be
        : code_pages::utf32le;

    auto d = reinterpret_cast<const std::uint8_t *>(dst);
    if (cnt_dst > 0) {
        cnt_dst *= sizeof(char32_t);
    }

    auto retval = convert_string(d, cnt_dst, wchar_code_page,
        src, cnt_src, code_page);
    assert((retval % sizeof(char32_t)) == 0);
    retval %= sizeof(char32_t);

    return retval;
}



/*
 * FPD_NAMESPACE::convert_string
 */
template<class TUnicode>
std::enable_if_t<sizeof(TUnicode) == sizeof(char32_t), std::size_t>
FPD_NAMESPACE::convert_string(
        _Out_writes_to_opt_(cnt_dst, return) char *dst,
        _In_ std::size_t cnt_dst,
        _In_reads_or_z_opt_(cnt_src) const TUnicode *src,
        _In_ int cnt_src,
        _In_opt_z_ const code_page code_page) {
    static constexpr auto wchar_code_page = (endian::system == endian::big)
        ? code_pages::utf32be
        : code_pages::utf32le;

    auto s = reinterpret_cast<const std::uint8_t *>(src);
    if (cnt_src > 0) {
        cnt_src *= sizeof(char32_t);
    }

    return convert_string(dst, cnt_dst, code_page, s, cnt_src, wchar_code_page);
}


/*
 * FPD_NAMESPACE::convert_string
 */
template<class TChar>
std::size_t FPD_NAMESPACE::convert_string(
        _Out_writes_to_opt_(cnt_dst, return) TChar *dst,
        _In_ std::size_t cnt_dst,
        _In_reads_or_z_opt_(cnt_src) const TChar *src,
        _In_ int cnt_src,
        _In_ const code_page code_page) {
    if (dst == nullptr) {
        // Make sure that the output buffer size is zero if the output
        // buffer is invalid, such that we get the required buffer size.
        cnt_dst = 0;
    }
    if (src == nullptr) {
        // Likewise, if the source is invalid, bail out directly.
        return 0;
    }

    if ((cnt_src < 0) && (src != nullptr)) {
        // User requested us to measure the input, do so now.
        cnt_src = static_cast<int>(std::char_traits<TChar>::length(src)) + 1;
    }

    const auto cnt = (std::min)(cnt_dst, static_cast<std::size_t>(cnt_src));
    ::memcpy(dst, src, cnt * sizeof(TChar));

    return cnt_src;
}


/*
 * FPD_NAMESPACE::convert_string
 */
template<class TTo, class TFrom>
std::basic_string<TTo> FPD_NAMESPACE::convert_string(
        _In_reads_or_z_opt_(cnt_src) const TFrom *src,
        _In_ int cnt_src,
        _In_ const code_page code_page) {
    static_assert((sizeof(TTo) > 1) || (sizeof(TFrom) > 1), "At least one of "
        "the character types must be a multi-byte Unicode type.");
    constexpr auto zero = static_cast<TTo>(0);
    const auto cnt_dst = measure_string<TTo>(src, cnt_src, code_page);
    std::basic_string<TTo> retval(cnt_dst, zero);

    if (cnt_dst > 0) {
        convert_string<TTo>(
            std::addressof(retval[0]), cnt_dst,
            src, cnt_src,
            code_page);
        auto end = std::remove(retval.begin(), retval.end(), zero);
        retval.erase(end, retval.end());
    }

    return retval;
}


/*
 * FPD_NAMESPACE::convert_string
 */
template<class TChar>
std::basic_string<TChar> FPD_NAMESPACE::convert_string(
        _In_reads_or_z_opt_(cnt_src) const TChar *src,
        _In_ int cnt_src,
        _In_ const code_page code_page) {
    if ((src == nullptr) || (cnt_src == 0)) {
        return std::basic_string<TChar>();
    }

    if (cnt_src < 0) {
        return src;
    }

    assert(cnt_src <= std::char_traits<TChar>::length(src));
    return std::basic_string<TChar>(src, cnt_src);
}


/*
 * FPD_NAMESPACE::convert_string
 */
template<class TChar, class TTraits, class TAlloc>
std::basic_string<TChar, TTraits, TAlloc> FPD_NAMESPACE::convert_string(
        _In_ const std::basic_string<TChar, TTraits, TAlloc>& src,
        _In_ const int cnt,
        _In_opt_z_ const code_page reserved) {
    if ((cnt < 0) || (cnt >= src.size())) {
        return src;
    } else {
        return src.substr(0, cnt);
    }
}
