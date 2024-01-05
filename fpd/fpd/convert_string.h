// <copyright file="convert_string.h" company="Visualisierungsinstitut der Universität Stuttgart">
// Copyright © 2023 - 2024 Visualisierungsinstitut der Universität Stuttgart.
// Licensed under the MIT licence. See LICENCE file for details.
// </copyright>
// <author>Christoph Müller</author>

#pragma once

#include <cassert>
#include <string>
#include <system_error>

#if defined(ICU_FOUND)
#include <unicode/ucnv.h>
#include <unicode/unistr.h>
#include <unicode/ustring.h>
#include <unicode/utypes.h>
#endif /* defined(ICU_FOUND) */

#include "fpd/code_page.h"
#include "fpd/icu_category.h"


FPD_NAMESPACE_BEGIN

/// <summary>
/// Converts a wide string to UTF-16.
/// </summary>
/// <param name="dst">A buffer to receive the output that can hold at least
/// <paramref name="cnt_dst" /> characters. This can be <c>nullptr</c> if
/// <paramref name="cnt_dst" /> is zero.</param>
/// <param name="cnt_dst">The number of elements that can be stored to
/// <paramref name="dst" />.</param>
/// <param name="src">The source string.</param>
/// <param name="cnt_src">The length of the source string, which can be negative
/// if <paramref name="src" /> is null-terminated.</param>
/// <param name="code_page">This parameter is unused and is only for API
/// compatibility.</param>
/// <returns>The number of characters required for the output string,
/// including the terminating zero.</returns>
/// <exception cref="std::invalid_argument">If <paramref name="src" /> is
/// <c>nullptr</c>.</exception>
/// <exception cref="std::system_error">If the conversion failed.
/// </exception>
std::size_t convert_string(_Out_writes_to_opt_(cnt_dst, return) char16_t *dst,
    _In_ std::size_t cnt_dst,
    _In_reads_or_z_(cnt_src) const wchar_t *src,
    _In_ const std::ptrdiff_t cnt_src,
    _In_ const code_page code_page);

/// <summary>
/// Converts a UTF-16 string to a wide string.
/// </summary>
/// <param name="dst">A buffer to receive the output that can hold at least
/// <paramref name="cnt_dst" /> characters. This can be <c>nullptr</c> if
/// <paramref name="cnt_dst" /> is zero.</param>
/// <param name="cnt_dst">The number of elements that can be stored to
/// <paramref name="dst" />.</param>
/// <param name="src">The source string.</param>
/// <param name="cnt_src">The length of the source string, which can be negative
/// if <paramref name="src" /> is null-terminated.</param>
/// <param name="code_page">The code page used for the narrow string.</param>
/// <returns>The number of characters required for the output string,
/// including the terminating zero.</returns>
/// <exception cref="std::invalid_argument">If <paramref name="src" /> is
/// <c>nullptr</c>.</exception>
/// <exception cref="std::system_error">If the conversion failed.
/// </exception>
std::size_t convert_string(_Out_writes_to_opt_(cnt_dst, return) wchar_t *dst,
    _In_ std::size_t cnt_dst,
    _In_reads_or_z_(cnt_src) const char16_t *src,
    _In_ const std::ptrdiff_t cnt_src,
    _In_ const code_page code_page);

/// <summary>
/// Converts a multi-byte string of the given encoding into an UTF-16
/// string.
/// </summary>
/// <param name="dst">A buffer to receive the output that can hold at least
/// <paramref name="cnt_dst" /> characters. This can be <c>nullptr</c> if
/// <paramref name="cnt_dst" /> is zero.</param>
/// <param name="cnt_dst">The number of elements that can be stored to
/// <paramref name="dst" />.</param>
/// <param name="src">The source string.</param>
/// <param name="cnt_src">The length of the source string, which can be negative
/// if <paramref name="src" /> is null-terminated.</param>
/// <param name="code_page">The code page used for the narrow string.</param>
/// <returns>The number of characters required for the output string,
/// including the terminating zero.</returns>
/// <exception cref="std::invalid_argument">If <paramref name="src" /> is
/// <c>nullptr</c>.</exception>
/// <exception cref="std::system_error">If the conversion failed.
/// </exception>
std::size_t convert_string(_Out_writes_to_opt_(cnt_dst, return) char16_t *dst,
    _In_ std::size_t cnt_dst,
    _In_reads_or_z_(cnt_src) const char *src,
    _In_ const std::ptrdiff_t cnt_src,
    _In_ const code_page code_page);

/// <summary>
/// Converts a multi-byte string of the given encoding into a wide
/// string.
/// </summary>
/// <param name="dst">A buffer to receive the output that can hold at least
/// <paramref name="cnt_dst" /> characters. This can be <c>nullptr</c> if
/// <paramref name="cnt_dst" /> is zero.</param>
/// <param name="cnt_dst">The number of elements that can be stored to
/// <paramref name="dst" />.</param>
/// <param name="src">The source string.</param>
/// <param name="cnt_src">The length of the source string, which can be negative
/// if <paramref name="src" /> is null-terminated.</param>
/// <param name="code_page">The code page used for the narrow string.</param>
/// <returns>The number of characters required for the output string,
/// including the terminating zero.</returns>
/// <exception cref="std::invalid_argument">If <paramref name="src" /> is
/// <c>nullptr</c>.</exception>
/// <exception cref="std::system_error">If the conversion failed.
/// </exception>
std::size_t convert_string(_Out_writes_to_opt_(cnt_dst, return) wchar_t *dst,
    _In_ std::size_t cnt_dst,
    _In_reads_or_z_(cnt_src) const char *src,
    _In_ const std::ptrdiff_t cnt_src,
    _In_ const code_page code_page);

/// <summary>
/// Converts a UTF-16 string to a string of the given encoding.
/// </summary>
/// <param name="dst">A buffer to receive the output that can hold at least
/// <paramref name="cnt_dst" /> characters. This can be <c>nullptr</c> if
/// <paramref name="cnt_dst" /> is zero.</param>
/// <param name="cnt_dst">The number of elements that can be stored to
/// <paramref name="dst" />.</param>
/// <param name="src">The source string.</param>
/// <param name="cnt_src">The length of the source string, which can be negative
/// if <paramref name="src" /> is null-terminated.</param>
/// <param name="code_page">The code page used for the narrow string.</param>
/// <returns>The number of characters required for the output string,
/// including the terminating zero.</returns>
/// <exception cref="std::invalid_argument">If <paramref name="src" /> is
/// <c>nullptr</c>.</exception>
/// <exception cref="std::system_error">If the conversion failed.
/// </exception>
std::size_t convert_string(_Out_writes_to_opt_(cnt_dst, return) char *dst,
    _In_ std::size_t cnt_dst,
    _In_reads_or_z_(cnt_src) const char16_t *src,
    _In_ const std::ptrdiff_t cnt_src,
    _In_ const code_page code_page);

/// <summary>
/// Converts a wide string to a string of the given encoding.
/// </summary>
/// <param name="dst">A buffer to receive the output that can hold at least
/// <paramref name="cnt_dst" /> characters. This can be <c>nullptr</c> if
/// <paramref name="cnt_dst" /> is zero.</param>
/// <param name="cnt_dst">The number of elements that can be stored to
/// <paramref name="dst" />.</param>
/// <param name="src">The source string.</param>
/// <param name="cnt_src">The length of the source string, which can be negative
/// if <paramref name="src" /> is null-terminated.</param>
/// <param name="code_page">The code page used for the narrow string.</param>
/// <returns>The number of characters required for the output string,
/// including the terminating zero.</returns>
/// <exception cref="std::invalid_argument">If <paramref name="src" /> is
/// <c>nullptr</c>.</exception>
/// <exception cref="std::system_error">If the conversion failed.
/// </exception>
std::size_t convert_string(_Out_writes_to_opt_(cnt_dst, return) char *dst,
    _In_ std::size_t cnt_dst,
    _In_reads_or_z_(cnt_src) const wchar_t *src,
    _In_ const std::ptrdiff_t cnt_src,
    _In_ const code_page code_page);

/// <summary>
/// Performs a trivial conversion (<see cref="memcpy" />).
/// </summary>
/// <typeparam name="TChar"></typeparam>
/// <param name="dst">A buffer to receive the output that can hold at least
/// <paramref name="cnt_dst" /> characters. This can be <c>nullptr</c> if
/// <paramref name="cnt_dst" /> is zero.</param>
/// <param name="cnt_dst">The number of elements that can be stored to
/// <paramref name="dst" />.</param>
/// <param name="src">The source string.</param>
/// <param name="cnt_src">The length of the source string, which can be zero
/// if <paramref name="src" /> is null-terminated.</param>
/// <param name="code_page">This parameter is ignored.</param>
/// <returns>The number of characters required for the output string,
/// including the terminating zero.</returns>
/// <exception cref="std::invalid_argument">If <paramref name="src" /> is
/// <c>nullptr</c>.</exception>
/// <exception cref="std::system_error">If the conversion failed.
/// </exception>
template<class TChar> std::size_t convert_string(
    _Out_writes_to_opt_(cnt_dst, return) TChar *dst,
    _In_ const std::size_t cnt_dst,
    _In_reads_or_z_(cnt_src) const TChar *src,
    _In_ std::ptrdiff_t cnt_src,
    _In_ const code_page code_page);

/// <summary>
/// Converts a C-style string into an STL string.
/// </summary>
/// <typeparam name="TChar">The type of the characters.</typeparam>
/// <param name="src">The source string.</param>
/// <param name="cnt_src">The number of characters in the source string to
/// convert or zero for converting a full null-terminated string.</param>
/// <param name="code_page">This parameter is unused and is only for API
/// compatibility.</param>
/// <returns>The converted string.</returns>
template<class TChar> inline std::basic_string<TChar> convert(
        _In_reads_or_z_(cnt_src) const TChar *src,
        _In_ const std::ptrdiff_t cnt_src,
        _In_ const code_page code_page) {
    assert(src != nullptr);
    return (cnt_src > 0)
        ? std::basic_string<TChar>(src, src + cnt_src)
        : std::basic_string<TChar>(src);
}

/// <summary>
/// Converts a C-style string into an STL string.
/// </summary>
/// <typeparam name="TDstChar">The type of the target characters.</typeparam>
/// <typeparam name="TSrcChar">The type of the source caracters.</typeparam>
/// <param name="src"></param>
/// <param name="cnt_src"></param>
/// <param name="code_page">The code page to be used if any of the strings is
/// a narrow character.</param>
/// <returns>The converted string.</returns>
template<class TDstChar, class TSrcChar>
inline std::basic_string<TDstChar> convert_string(
        _In_reads_or_z_(cnt_src) const TSrcChar *src,
        _In_ const std::ptrdiff_t cnt_src,
        _In_ const code_page code_page) {
    std::vector<TDstChar> buffer(convert_string(
        static_cast<TDstChar *>(nullptr), 0,
        src, cnt_src,
        code_page));
    auto cnt = convert_string(buffer.data(), buffer.size(),
        src, cnt_src,
        code_page);
    return std::basic_string<TDstChar>(buffer.data(),
        buffer.data() + cnt - 1);
}

/// <summary>
/// Conversion between STL strings.
/// </summary>
/// <typeparam name="TChar">The type of characters.</typeparam>
/// <typeparam name="TTraits">The character traits.</typeparam>
/// <typeparam name="TAlloc">The string allocator.</typeparam>
/// <param name="src">The source string.</param>
/// <param name="cnt_src">The number of characters to convert. This can be
/// -1 to return all of <pararmef name="src" />.</param>
/// <returns>The first <paramref name="cnt_src" /> characters of the input
/// string or the whole input string, if <paramref name="cnt_src" /> is -1.
/// </returns>
template<class TChar, class TTraits, class TAlloc>
inline std::basic_string<TChar> convert_string(
        _In_ const std::basic_string<TChar, TTraits, TAlloc>& src,
        _In_ const std::ptrdiff_t cnt_src,
        _In_ const code_page) {
    assert(cnt_src <= src.size());
    return (cnt_src < 0) ? src : src.substr(0, cnt_src);
}

/// <summary>
/// Conversion between STL strings.
/// </summary>
/// <typeparam name="TChar">The type of characters.</typeparam>
/// <typeparam name="TTraits">The character traits.</typeparam>
/// <typeparam name="TAlloc">The string allocator.</typeparam>
/// <param name="src">The source string.</param>
/// <param name="code_page">This parameter is unused and is only for API
/// compatibility.</param>
/// <returns>The first <paramref name="</returns>
template<class TChar, class TTraits, class TAlloc>
inline std::basic_string<TChar>& convert_string(
        _In_ const std::basic_string<TChar, TTraits, TAlloc>& src,
        _In_ const code_page code_page) {
    return src;
}

/// <summary>
/// Convert between STL strings.
/// </summary>
/// <typeparam name="TDstChar">The type of the target characters.</typeparam>
/// <typeparam name="TSrcChar">The type of the source caracters.</typeparam>
/// <typeparam name="TSrcTraits">The character traits of the source string.
/// </typeparam>
/// <typeparam name="TSrcAlloc">The allocator of the source string.</typeparam>
/// <param name="src">The source string.</param>
/// <param name="cnt_src">The length of the source string, which can be -1 for
/// using the size of the string object.</param>
/// <param name="code_page">The code page of the MBCS string.</param>
/// <returns>The converted string.</returns>
template<class TDstChar, class TSrcChar, class TSrcTraits, class TSrcAlloc>
inline std::basic_string<TDstChar> convert_string(
        _In_ const std::basic_string<TSrcChar, TSrcTraits, TSrcAlloc>& src,
        _In_ const std::ptrdiff_t cnt_src,
        _In_ const code_page code_page) {
    assert(cnt_src <= src.size());
    std::vector<TDstChar> buffer(convert_string(
        static_cast<TDstChar *>(nullptr), 0,
        src.data(), (cnt_src < 0) ? src.size() : cnt_src,
        code_page));
    auto cnt = convert_string(buffer.data(), buffer.size(),
        src.data(), (cnt_src < 0) ? src.size() : cnt_src,
        code_page);
    return std::basic_string<TDstChar>(buffer.data(),
        buffer.data() + cnt - 1);
}

/// <summary>
/// Convert between STL strings.
/// </summary>
/// <typeparam name="TDstChar">The type of the target characters.</typeparam>
/// <typeparam name="TSrcChar">The type of the source caracters.</typeparam>
/// <typeparam name="TSrcTraits">The character traits of the source string.
/// </typeparam>
/// <typeparam name="TSrcAlloc">The allocator of the source string.</typeparam>
/// <param name="src">The source string.</param>
/// <param name="code_page">The code page of the MBCS string.</param>
/// <returns>The converted string.</returns>
template<class TDstChar, class TSrcChar, class TSrcTraits, class TSrcAlloc>
inline std::basic_string<TDstChar> convert_string(
        _In_ const std::basic_string<TSrcChar, TSrcTraits, TSrcAlloc>& src,
        _In_ const code_page code_page) {
    return convert_string(src, -1, code_page);
}


/// <summary>
/// Measures how many code units of <typeparamref name="TDstChar" /> are
/// required for <paramref name="str" />.
/// </summary>
/// <remarks>
/// This is a convenience function for <see cref="convert_string" /> without
/// a valid output buffer.
/// </remarks>
/// <typeparam name="TDstChar"></typeparam>
/// <typeparam name="TSrcChar"></typeparam>
/// <param name="str"></param>
/// <param name="cnt"></param>
/// <param name="code_page"></param>
/// <returns>The number of characters required for the converted string,
/// including the terminating zero.</returns>
/// <exception cref="std::invalid_argument">If <paramref name="src" /> is
/// <c>nullptr</c>.</exception>
/// <exception cref="std::system_error">If the conversion is not possible.
/// </exception>
template<class TDstChar, class TSrcChar>
inline std::size_t measure_string(_In_reads_or_z_(cnt_src) const TSrcChar *str,
        _In_ const std::ptrdiff_t cnt,
        _In_ const code_page code_page) {
    return convert_string(static_cast<TDstChar *>(nullptr), 0, str, cnt,
        code_page);
}

/// <summary>
/// Measures how many code units of <typeparamref name="TDstChar" /> are
/// required for <paramref name="str" />.
/// </summary>
/// <remarks>
/// This is a convenience function for <see cref="convert_string" /> without
/// a valid output buffer.
/// </remarks>
/// <typeparam name="TDstChar"></typeparam>
/// <typeparam name="TSrcChar"></typeparam>
/// <param name="str"></param>
/// <param name="code_page"></param>
/// <returns>The number of characters required for the converted string,
/// including the terminating zero.</returns>
/// <exception cref="std::invalid_argument">If <paramref name="src" /> is
/// <c>nullptr</c>.</exception>
/// <exception cref="std::system_error">If the conversion is not possible.
/// </exception>
template<class TDstChar, class TSrcChar>
inline std::size_t measure_string(_In_reads_or_z_(cnt_src) const TSrcChar *str,
        _In_ const code_page code_page) {
    return convert_string(static_cast<TDstChar *>(nullptr), 0, str, -1,
        code_page);
}

/// <summary>
/// Measures how many code units of <typeparamref name="TDstChar" /> are
/// required for <paramref name="str" />.
/// </summary>
/// <remarks>
/// This is a convenience function for <see cref="convert_string" /> without
/// a valid output buffer.
/// </remarks>
/// <typeparam name="TDstChar"></typeparam>
/// <typeparam name="TSrcChar"></typeparam>
/// <typeparam name="TSrcTraits"></typeparam>
/// <typeparam name="TSrcAlloc"></typeparam>
/// <param name="str"></param>
/// <param name="code_page"></param>
/// <returns>The number of characters required for the converted string,
/// including the terminating zero.</returns>
/// <exception cref="std::system_error">If the conversion is not possible.
/// </exception>
template<class TDstChar, class TSrcChar, class TSrcTraits, class TSrcAlloc>
inline std::size_t measure_string(
        _In_ const std::basic_string<TSrcChar, TSrcTraits, TSrcAlloc>& str,
        _In_ const code_page code_page) {
    return convert_string(static_cast<TDstChar *>(nullptr), 0, str.c_str(), -1,
        code_page);
}

FPD_NAMESPACE_END

#include "fpd/convert_string.inl"
