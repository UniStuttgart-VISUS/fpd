// <copyright file="convert_string.inl" company="Visualisierungsinstitut der Universität Stuttgart">
// Copyright © 2023 - 2025 Visualisierungsinstitut der Universität Stuttgart.
// Licensed under the MIT licence. See LICENCE file for details.
// </copyright>
// <author>Christoph Müller</author>


FPD_DETAIL_NAMESPACE_BEGIN

/// <summary>
/// A unique pointer for libicu converters.
/// </summary>
typedef std::unique_ptr<UConverter, decltype(&::ucnv_close)> unique_uconverter;


/// <summary>
/// Perform a conversion from Unicode using the given
/// <paramref name="converter" />.
/// </summary>
template<class TTo>
std::size_t from_unicode(
        _Out_writes_to_ptr_opt_(dst_limit) TTo *dst,
        _In_opt_ const TTo *dst_limit,
        _In_reads_to_ptr_(src_limit) const utf16_type *src,
        _In_ const utf16_type *src_limit,
        _In_ UConverter *converter) {
    static_assert(sizeof(TTo) == 1, "TTo must be a single byte type.");
    assert(dst_limit >= dst);
    assert(src != nullptr);
    assert(src_limit >= src);
    assert(converter != nullptr);

    // Perform a stream conversion, possibly into an intermediate buffer just
    // for measuring.
    auto cur_dst = reinterpret_cast<char *>(dst);
    auto dst_end = reinterpret_cast<const char *>(dst_limit);
    auto cur_src = src;
    auto src_end = src_limit;

    auto status = U_ZERO_ERROR;
    ::ucnv_fromUnicode(converter,
        &cur_dst, dst_end,
        &cur_src, src_end,
        nullptr, true, &status);

    auto retval = (cur_dst - reinterpret_cast<char *>(dst));

    if (status == U_BUFFER_OVERFLOW_ERROR) {
        // If the output buffer was too small, create an intermediate one and
        // measure how much we need. ICU typically uses 1024 for this case, so
        // we do so, too.
        std::array<char, 1024> buffer;
        dst_end = buffer.data() + buffer.size();

        do {
            cur_dst = buffer.data();
            status = U_ZERO_ERROR;
            ::ucnv_fromUnicode(converter,
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


/// <summary>
/// Perform a conversion from Unicode by creating a converter for the given.
/// <paramref name="code_page" />.
/// </summary>
template<class TTo>
std::size_t from_unicode(
        _Out_writes_to_ptr_opt_(dst_limit) TTo *dst,
        _In_opt_ const TTo *dst_limit,
        _In_reads_to_ptr_(src_limit) const utf16_type *src,
        _In_ const utf16_type *src_limit,
        _In_opt_z_ const encoding code_page) {
    auto status = U_ZERO_ERROR;
    detail::unique_uconverter conv(::ucnv_open(code_page, &status),
        ::ucnv_close);
    detail::throw_if_icu_failed(status);
    return from_unicode(dst, dst_limit, src, src_limit, conv.get());
}


/// <summary>
/// Perform a conversion to Unicode using the given
/// <paramref name="converter" />.
/// </summary>
template<class TFrom>
std::size_t to_unicode(
    _Out_writes_to_ptr_opt_(dst_limit) utf16_type *dst,
        _In_reads_opt_(0) const utf16_type *dst_limit,
        _In_reads_to_ptr_(src_limit) const TFrom *src,
        _In_reads_(0) const TFrom *src_limit,
        _In_ UConverter *converter) {
    static_assert(sizeof(TFrom) == 1, "TFrom must be a single byte type.");
    assert(dst_limit >= dst);
    assert(src != nullptr);
    assert(src_limit >= src);
    assert(converter != nullptr);

    // Perform a stream conversion, possibly into an intermediate buffer just
    // for measuring.
    auto dst_cur = dst;
    auto dst_end = dst_limit;
    auto src_cur = reinterpret_cast<const char *>(src);
    auto src_end = reinterpret_cast<const char *>(src_limit);

    auto status = U_ZERO_ERROR;
    ::ucnv_toUnicode(converter,
        &dst_cur, dst_end,
        &src_cur, src_end,
        nullptr, true, &status);

    auto retval = (dst_cur - dst);

    if (status == U_BUFFER_OVERFLOW_ERROR) {
        // If the output buffer was too small, create an intermediate one and
        // measure how much we need. ICU typically uses 1024 for this case, so
        // we do so, too.
        std::array<utf16_type, 1024> buffer;
        dst_end = buffer.data() + buffer.size();

        do {
            dst_cur = buffer.data();
            status = U_ZERO_ERROR;
            ::ucnv_toUnicode(converter,
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


/// <summary>
/// Perform a conversion to Unicode from the given
/// <paramref name="code_page" />.
/// </summary>
template<class TFrom>
inline std::size_t to_unicode(
        _Out_writes_to_ptr_opt_(dst_limit) utf16_type *dst,
        _In_reads_opt_(0) const utf16_type *dst_limit,
        _In_reads_to_ptr_(src_limit) const TFrom *src,
        _In_reads_(0) const TFrom *src_limit,
        _In_opt_z_ const encoding code_page) {
    auto status = U_ZERO_ERROR;
    detail::unique_uconverter conv(::ucnv_open(code_page, &status),
        ::ucnv_close);
    detail::throw_if_icu_failed(status);
    return to_unicode(dst, dst_limit, src, src_limit, conv.get());
}

FPD_DETAIL_NAMESPACE_END


/*
 * FPD_NAMESPACE::convert_string
 */
template<class TUnicode>
std::enable_if_t<sizeof(TUnicode) == sizeof(FPD_NAMESPACE::utf16_type),
    std::size_t>
FPD_NAMESPACE::convert_string(
        _Out_writes_to_opt_(cnt_dst, return) TUnicode *dst,
        _In_ std::size_t cnt_dst,
        _In_reads_or_z_opt_(cnt_src) const char *src,
        _In_ int cnt_src,
        _In_ const encoding code_page) {
    if (dst == nullptr) {
        // Make sure that the output buffer size is zero if the output
        // buffer is invalid, such that we get the required buffer size.
        cnt_dst = 0;
    }
    if (src == nullptr) {
        // Likewise, if the source is invalid, bail out directly.
        return 0;
    }

    std::size_t retval = 0;
    if ((cnt_src < 0) && (src != nullptr)) {
        // User requested us to measure the input, do so now.
        cnt_src = static_cast<int>(std::char_traits<char>::length(src));
        // If we are measuring, tell the caller about the terminating zero.
        retval = 1;
    }

    assert(src != nullptr);
    assert(cnt_src <= std::char_traits<char>::length(src));
    retval += detail::to_unicode(
        reinterpret_cast<utf16_type *>(dst),
        reinterpret_cast<utf16_type *>(dst) + cnt_dst,
        src,
        src + cnt_src,
        code_page);
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
        _In_opt_z_ const encoding code_page) {
    if (dst == nullptr) {
        // Make sure that the output buffer size is zero if the output
        // buffer is invalid, such that we get the required buffer size.
        cnt_dst = 0;
    }
    if (src == nullptr) {
        // Likewise, if the source is invalid, bail out directly.
        return 0;
    }

    std::size_t retval = 0;
    if ((cnt_src < 0) && (src != nullptr)) {
        // User requested us to measure the input, do so now.
        cnt_src = static_cast<int>(std::char_traits<TUnicode>::length(src));
        // If we are measuring, tell the caller about the terminating zero.
        retval = 1;
    }

    assert(src != nullptr);
    assert(cnt_src <= std::char_traits<TUnicode>::length(src));
    retval += detail::from_unicode(
        dst,
        dst + cnt_dst,
        reinterpret_cast<const utf16_type *>(src),
        reinterpret_cast<const utf16_type *>(src) + cnt_src,
        code_page);
    return retval;
}


/*
 * FPD_NAMESPACE::convert_string
 */
template<class TTo, class TFrom>
std::size_t FPD_NAMESPACE::convert_string(
        _Out_writes_to_opt_(cnt_dst, return) TTo *dst,
        _In_ std::size_t cnt_dst,
        _In_opt_z_ const encoding dst_code_page,
        _In_reads_or_z_opt_(cnt_src) const TFrom *src,
        _In_ int cnt_src,
        _In_opt_z_ const encoding src_code_page) {
    // Determines whether two names refer to the same encoding.
    static const auto same_encoding = [](
            const encoding lhs,
            const encoding rhs) {
        if (lhs == rhs) {
            return true;
        } else if ((lhs == nullptr) || (rhs == nullptr)) {
            return false;
        } else {
            return (::ucnv_compareNames(lhs, rhs) == 0);
        }
    };

    if (dst == nullptr) {
        // Make sure that the output buffer size is zero if the output
        // buffer is invalid, such that we get the required buffer size.
        cnt_dst = 0;
    }
    if (src == nullptr) {
        // Likewise, if the source is invalid, bail out directly.
        return 0;
    }

    if (same_encoding(dst_code_page, src_code_page)) {
        // No conversion was requested, to we can pass it to the trivial copy
        // conversion.
        assert(sizeof(TTo) == sizeof(TFrom));
        return convert_string(
            dst, cnt_dst,
            reinterpret_cast<const TTo *>(src), cnt_src,
            dst_code_page);
    }

    if (same_encoding(dst_code_page, encodings::utf16)) {
        // Convert from the source encoding to UTF-16, which is a task we can
        // also pass on to another, more effcient implementation.
        assert(sizeof(TTo) == sizeof(utf16_type));
        assert(sizeof(TFrom) == 1);
        auto s = reinterpret_cast<const char *>(src);
        return convert_string(dst, cnt_dst, s, cnt_src, src_code_page);
    }

    if (same_encoding(src_code_page, encodings::utf16)) {
        // Convert to UTF-16, which is a task we can also pass on to another,
        // more efficient implementation.
        assert(sizeof(TTo) == 1);
        assert(sizeof(TFrom) == sizeof(utf16_type));
        auto d = reinterpret_cast<char *>(dst);
        return convert_string(d, cnt_dst, src, cnt_src, dst_code_page);
    }

    // Note: there is ucnv_convert which does *almost* the same pivot via UTF-16
    // as this code, but has the issue that is *never* counts the terminating
    // null even if -1 is specified for 'cnt_src'. This violates our contract,
    // so we reimplement the functionality using our own wrappers here.
    auto status = U_ZERO_ERROR;
    detail::unique_uconverter conv(::ucnv_open(src_code_page, &status),
        ::ucnv_close);
    detail::throw_if_icu_failed(status);

    std::size_t retval = 0;
    if (cnt_src < 0) {
        // User requested us to measure the input, do so now.
        assert(::ucnv_getMinCharSize(conv.get()) == sizeof(TFrom));
        cnt_src = static_cast<int>(std::char_traits<TFrom>::length(src));
        // If we are measuring, tell the caller about the terminating zero.
        retval = 1;
    }

    // Otherwise, we need to pivot via UTF-16. First, we measure how big the
    // UTF-16 representation will be.
    assert(sizeof(TFrom) == 1);
    auto s = reinterpret_cast<const char *>(src);
    std::vector<utf16_type> tmp(detail::to_unicode(
        nullptr, nullptr,
        s, s + cnt_src,
        conv.get()));

    // Create the intermediate UTF-16 representation.
    detail::to_unicode(
        tmp.data(), tmp.data() + tmp.size(),
        s, s + cnt_src,
        src_code_page);

    // Convert from UTF-16 to target encoding.
    assert(sizeof(TTo) == 1);
    auto d = reinterpret_cast<char *>(dst);
    retval += detail::from_unicode(
        d, d + cnt_dst,
        tmp.data(), tmp.data() + tmp.size(),
        dst_code_page);

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
        _In_ const encoding code_page) {
    auto d = reinterpret_cast<const std::uint8_t *>(dst);
    if (cnt_dst > 0) {
        cnt_dst *= sizeof(char32_t);
    }

    auto retval = convert_string(
        d, cnt_dst, encodings::utf32,
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
        _In_opt_z_ const encoding code_page) {
    auto s = reinterpret_cast<const std::uint8_t *>(src);
    if (cnt_src > 0) {
        cnt_src *= sizeof(char32_t);
    }

    return convert_string(
        dst, cnt_dst, code_page,
        s, cnt_src, encodings::utf32);
}


/*
 * FPD_NAMESPACE::convert_string
 */
template<class TTo, class TFrom>
std::enable_if_t<sizeof(TTo) == sizeof(TFrom), std::size_t>
FPD_NAMESPACE::convert_string(
        _Out_writes_to_opt_(cnt_dst, return) TTo *dst,
        _In_ std::size_t cnt_dst,
        _In_reads_or_z_opt_(cnt_src) const TFrom *src,
        _In_ int cnt_src,
        _In_ const encoding code_page) {
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
        cnt_src = static_cast<int>(std::char_traits<TFrom>::length(src));
        // If we are measuring, tell the caller about the terminating zero.
        ++cnt_src;
    }

    const auto cnt = (std::min)(cnt_dst, static_cast<std::size_t>(cnt_src));
    ::memcpy(dst, src, cnt * sizeof(TFrom));

    return cnt_src;
}


/*
 * FPD_NAMESPACE::convert_string
 */
template<class TTo, class TFrom>
std::enable_if_t<sizeof(TTo) != sizeof(TFrom), std::basic_string<TTo>>
FPD_NAMESPACE::convert_string(
        _In_reads_or_z_opt_(cnt_src) const TFrom *src,
        _In_ int cnt_src,
        _In_ const encoding code_page) {
    static_assert((sizeof(TTo) > 1) || (sizeof(TFrom) > 1), "At least one of "
        "the character types must be a multi-byte Unicode type.");
    if ((src == nullptr) || (*src == 0) || (cnt_src == 0)) {
        // Nothing to convert, the result is an empty string.
        return std::basic_string<TTo>();
    }

    if (cnt_src < 0) {
        // Caller requested us to measure the input, so we do that now. Note
        // that we do not include the terminating zero as the STL string class
        // does this for us.
        assert(src != nullptr);
        cnt_src = static_cast<int>(std::char_traits<TFrom>::length(src));
    }

    const auto cnt_dst = convert_string(
        static_cast<TTo *>(nullptr), 0,
        src, cnt_src,
        code_page);
    assert(cnt_dst > 0);
    std::basic_string<TTo> retval(cnt_dst, static_cast<TTo>(0));

    convert_string(
        std::addressof(retval[0]), cnt_dst,
        src, cnt_src,
        code_page);
    assert(std::none_of(retval.begin(), retval.end(),
        [](const TTo c) { return c == 0; }));
    return retval;
}


/*
 * FPD_NAMESPACE::convert_string
 */
template<class TTo, class TFrom>
std::enable_if_t<sizeof(TTo) == sizeof(TFrom), std::basic_string<TTo>>
FPD_NAMESPACE::convert_string(
        _In_reads_or_z_opt_(cnt_src) const TFrom *src,
        _In_ int cnt_src,
        _In_opt_z_ const encoding code_page) {
    if ((src == nullptr) || (cnt_src == 0)) {
        return std::basic_string<TTo>();
    }

    auto s = reinterpret_cast<const TTo *>(src);

    if ((cnt_src < 0) || (cnt_src >= std::char_traits<TFrom>::length(src))) {
        return s;
    }

    return std::basic_string<TTo>(s, cnt_src);
}


/*
 * FPD_NAMESPACE::convert_string
 */
template<class TChar, class TTraits, class TAlloc>
std::basic_string<TChar, TTraits, TAlloc> FPD_NAMESPACE::convert_string(
        _In_ const std::basic_string<TChar, TTraits, TAlloc>& src,
        _In_ const int cnt,
        _In_opt_z_ const encoding reserved) {
    if ((cnt < 0) || (cnt >= src.size())) {
        return src;
    } else {
        return src.substr(0, cnt);
    }
}
