// <copyright file="convert_string.h" company="Visualisierungsinstitut der Universität Stuttgart">
// Copyright © 2023 - 2025 Visualisierungsinstitut der Universität Stuttgart.
// Licensed under the MIT licence. See LICENCE file for details.
// </copyright>
// <author>Christoph Müller</author>

#pragma once

#include <algorithm>
#include <array>
#include <cassert>
#include <memory>
#include <string>
#include <type_traits>

#include "fpd/code_page.h"
#include "fpd/endian.h"
#include "fpd/icu_category.h"


FPD_NAMESPACE_BEGIN

#if defined(_WIN32) && (NTDDI_VERSION < NTDDI_WIN10_RS2)
/// <summary>
/// The representation of a UTF-16 code unit.
/// </summary>
/// <remarks>
/// From https://unicode-org.github.io/icu/userguide/strings/: &quot;The Unicode
/// standard defines a default encoding based on 16-bit code units. This is
/// supported in ICU by the definition of the UChar to be an unsigned 16-bit
/// integer type. This is the base type for character arrays for strings in
/// ICU.&quot; This means that libicu is using UTF-16 like Win32 wherefore we
/// need an intermediate step via <c>char16_t</c> on Linux. This typedef ensures
/// that we can have the same interface declarations on all platforms.
/// </remarks>
typedef wchar_t utf16_type;

#else /* defined(_WIN32) && (NTDDI_VERSION < NTDDI_WIN10_RS2) */
/// <summary>
/// The representation of a UTF-16 code unit.
/// </summary>
/// <remarks>
/// From https://unicode-org.github.io/icu/userguide/strings/: &quot;The Unicode
/// standard defines a default encoding based on 16-bit code units. This is
/// supported in ICU by the definition of the UChar to be an unsigned 16-bit
/// integer type. This is the base type for character arrays for strings in
/// ICU.&quot; This means that libicu is using UTF-16 like Win32 wherefore we
/// need an intermediate step via <c>char16_t</c> on Linux. This typedef ensures
/// that we can have the same interface declarations on all platforms.
/// </remarks>
typedef char16_t utf16_type;
#endif /* defined(_WIN32) && (NTDDI_VERSION < NTDDI_WIN10_RS2) */


/// <summary>
/// Converts a multi-byte string of the given encoding into an UTF-16
/// string.
/// </summary>
/// <remarks>
/// This implementation will be used if the Unicode type is
/// <see cref="utf16_type" />.
/// </remarks>
/// <typeparam name="TUnicode">The type used to represent a UTF-16 character,
/// which must be <see cref="utf16_type" /> or a binary compatible type. On
/// Windows, this can be <c>wchar_t</c>, but on Linux, it cannot as it uses
/// UTF-32 for <c>wchar_t</c>.</typeparam>
/// <param name="dst">A buffer to receive the output that can hold at least
/// <paramref name="cnt_dst" /> characters. This can be
/// <see langword="nullptr" /> if <paramref name="cnt_dst" /> is zero.</param>
/// <param name="cnt_dst">The number of elements that can be stored to
/// <paramref name="dst" />.</param>
/// <param name="src">The source string. It is safe to pass
/// <see langword="nullptr" />, in which case nothing will be written.</param>
/// <param name="cnt_src">The length of the source string, which can be -1
/// if <paramref name="src" /> is null-terminated.</param>
/// <param name="encoding">The code page used for the narrow string.</param>
/// <returns>The number of characters required for the output string,
/// including the terminating zero if the conversion size is not limited
/// by <paramref name="cnt_src" />.</returns>
/// <exception cref="std::system_error">If the conversion failed.
/// </exception>
template<class TUnicode>
std::enable_if_t<sizeof(TUnicode) == sizeof(utf16_type), std::size_t>
convert_string(_Out_writes_to_opt_(cnt_dst, return) TUnicode *dst,
    _In_ std::size_t cnt_dst,
    _In_reads_or_z_opt_(cnt_src) const char *src,
    _In_ int cnt_src,
    _In_opt_z_ const code_page code_page);


/// <summary>
/// Converts a UTF-16 string to a multi-bytes string of the given encoding.
/// </summary>
/// <remarks>
/// This implementation will be used if the Unicode type is
/// <see cref="utf16_type" />.
/// </remarks>
/// <typeparam name="TUnicode">The type used to represent a UTF-16 character,
/// which must be <see cref="utf16_type" /> or a binary compatible type. On
/// Windows, this can be <c>wchar_t</c>, but on Linux, it cannot as it uses
/// UTF-32 for <c>wchar_t</c>.</typeparam>
/// <param name="dst">A buffer to receive the output that can hold at least
/// <paramref name="cnt_dst" /> characters. This can be
/// <see langword="nullptr" /> if <paramref name="cnt_dst" /> is zero.</param>
/// <param name="cnt_dst">The number of elements that can be stored to
/// <paramref name="dst" />.</param>
/// <param name="src">The source string.</param>
/// <param name="cnt_src">The length of the source string, which can be -1
/// if <paramref name="src" /> is null-terminated.</param>
/// <param name="encoding">The code page used for the narrow string.</param>
/// <returns>The number of characters required for the output string,
/// including the terminating zero if the conversion size is not limited
/// by <paramref name="cnt_src" />.</returns>
/// <exception cref="std::system_error">If the conversion failed.
/// </exception>
template<class TUnicode>
std::enable_if_t<sizeof(TUnicode) == sizeof(utf16_type), std::size_t>
convert_string(_Out_writes_to_opt_(cnt_dst, return) char *dst,
    _In_ std::size_t cnt_dst,
    _In_reads_or_z_opt_(cnt_src) const TUnicode *src,
    _In_ int cnt_src,
    _In_opt_z_ const code_page code_page);


/// <summary>
/// Convert a string from one encoding to another using an intermediate
/// step via UTF-16.
/// </summary>
/// <remarks>
/// <para>While this function can convert between arbitrary encodings, it uses
/// <see cref="ucnv_convert" /> under the hood, which pivots to the canonical
/// UTF-16 to perform the task. The conversion therefore might be rather
/// expensive. If you can use UTF-16 directly, the other overloads of this
/// function might be more appropriate for the task.</para>
/// <para>Please be aware that LibICU will output a BOM when converting to
/// UTF-16 or UTF-32 without specifying the byte order in the encodings.</para>
/// <para>Please be aware that the return value of the function is not the
/// number of characters required, but the number of bytes!</para>
/// </remarks>
/// <typeparam name="TTo">The type used to represent a byte in the output
/// encoding, which must be one byte in length.</typeparam>
/// <typeparam name="TFrom">The type used to represent a byte in the input
/// encoding, which must be one byte in length.</typeparam>
/// <param name="dst">The destination buffer used to store the converted
/// data. If this is <see langword="nullptr" />, the required buffer size will
/// be measured, but nothing will be written.</param>
/// <param name="cnt_dst">The size of <paramref name="dst" /> in bytes.</param>
/// <param name="dst_code_page">The encoding to convert to.</param>
/// <param name="src">The input string, which may be null-terminated.</param>
/// <param name="cnt_src">The length of the input string in bytes or -1 if the
/// input is null-terminated.</param>
/// <param name="src_code_page">The encoding of <paramref name="src" />.</param>
/// <returns>The number of bytes required to store the output string, <i>not</i>
/// including the terminator.</returns>
template<class TTo, class TFrom>
typename std::enable_if<(sizeof(TTo) == 1) && (sizeof(TFrom) == 1),
    std::size_t>::type
convert_string(_Out_writes_to_opt_(cnt_dst, return) TTo *dst,
    _In_ std::size_t cnt_dst,
    _In_opt_z_ const code_page dst_code_page,
    _In_reads_or_z_opt_(cnt_src) const TFrom *src,
    _In_ int cnt_src,
    _In_opt_z_ const code_page src_code_page);


//template<class TUnicode>
//inline std::enable_if_t<!std::is_same_v<TUnicode, utf16_type>
//    && sizeof(TUnicode) == sizeof(utf16_type), std::size_t>
//convert_string(_Out_writes_to_opt_(cnt_dst, return) TUnicode *dst,
//        _In_ std::size_t cnt_dst,
//        _In_reads_or_z_opt_(cnt_src) const char *src,
//        _In_ int cnt_src,
//        _In_opt_z_ const code_page code_page) {
//    return convert_string(reinterpret_cast<utf16_type *>(dst), cnt_dst,
//        src, cnt_src,
//        code_page);
//}
//
//
//template<class TUnicode>
//inline std::enable_if_t<!std::is_same_v<TUnicode, utf16_type>
//    && sizeof(TUnicode) == sizeof(utf16_type), std::size_t>
//convert_string(_Out_writes_to_opt_(cnt_dst, return) char *dst,
//        _In_ std::size_t cnt_dst,
//        _In_reads_or_z_opt_(cnt_src) const TUnicode *src,
//        _In_ int cnt_src,
//        _In_opt_z_ const code_page code_page) {
//    return convert_string(dst, cnt_dst,
//        reinterpret_cast<const utf16_type *>(src), cnt_src,
//        code_page);
//}


/// <summary>
/// Converts a multi-byte string of the given encoding into an UTF-32
/// string.
/// </summary>
/// <typeparam name="TUnicode">The type used to represent a UTF-16 character,
/// which must be <see cref="utf16_type" /> or a binary compatible type. On
/// Windows, this can be <c>wchar_t</c>, but on Linux, it cannot as it uses
/// UTF-32 for <c>wchar_t</c>.</typeparam>
/// <param name="dst">A buffer to receive the output that can hold at least
/// <paramref name="cnt_dst" /> characters. This can be
/// <see langword="nullptr" /> if <paramref name="cnt_dst" /> is zero.</param>
/// <param name="cnt_dst">The number of elements that can be stored to
/// <paramref name="dst" />.</param>
/// <param name="src">The source string. It is safe to pass
/// <see langword="nullptr" />, in which case nothing will be written.</param>
/// <param name="cnt_src">The length of the source string, which can be -1
/// if <paramref name="src" /> is null-terminated.</param>
/// <param name="encoding">The code page used for the narrow string.</param>
/// <returns>The number of characters required for the output string,
/// including the terminating zero.</returns>
/// <exception cref="std::system_error">If the conversion failed.
/// </exception>
template<class TUnicode>
std::enable_if_t<sizeof(TUnicode) == sizeof(char32_t), std::size_t>
convert_string(_Out_writes_to_opt_(cnt_dst, return) TUnicode *dst,
    _In_ std::size_t cnt_dst,
    _In_reads_or_z_opt_(cnt_src) const char *src,
    _In_ int cnt_src,
    _In_opt_z_ const code_page code_page);


/// <summary>
/// Converts a UTF-32 string to a string of the given encoding.
/// </summary>
/// <typeparam name="TUnicode">The type used to represent a UTF-16 character,
/// which must be <see cref="utf16_type" /> or a binary compatible type. On
/// Windows, this can be <c>wchar_t</c>, but on Linux, it cannot as it uses
/// UTF-32 for <c>wchar_t</c>.</typeparam>
/// <param name="dst">A buffer to receive the output that can hold at least
/// <paramref name="cnt_dst" /> characters. This can be
/// <see langword="nullptr" /> if <paramref name="cnt_dst" /> is zero.</param>
/// <param name="cnt_dst">The number of elements that can be stored to
/// <paramref name="dst" />.</param>
/// <param name="src">The source string.</param>
/// <param name="cnt_src">The length of the source string, which can be -1
/// if <paramref name="src" /> is null-terminated.</param>
/// <param name="encoding">The code page used for the narrow string.</param>
/// <returns>The number of characters required for the output string,
/// including the terminating zero.</returns>
/// <exception cref="std::system_error">If the conversion failed.
/// </exception>
template<class TUnicode>
std::enable_if_t<sizeof(TUnicode) == sizeof(char32_t), std::size_t>
convert_string(_Out_writes_to_opt_(cnt_dst, return) char *dst,
    _In_ std::size_t cnt_dst,
    _In_reads_or_z_opt_(cnt_src) const TUnicode *src,
    _In_ int cnt_src,
    _In_opt_z_ const code_page code_page);


/// <summary>
/// Performs a trivial conversion (<see cref="memcpy" />) of bitwise equivalent
/// types.
/// </summary>
/// <typeparam name="TTo">The character type to convert to.</typeparam>
/// <typeparam name="TFrom">The character type to convert from.</typeparam>
/// <param name="dst">A buffer to receive the output that can hold at least
/// <paramref name="cnt_dst" /> characters. This can be
/// <see langword="nullptr" /> if <paramref name="cnt_dst" /> is zero.</param>
/// <param name="cnt_dst">The number of elements that can be stored to
/// <paramref name="dst" />.</param>
/// <param name="src">The source string.</param>
/// <param name="cnt_src">The length of the source string, which can be -1
/// if <paramref name="src" /> is null-terminated.</param>
/// <param name="reserved">This parameter is ignored.</param>
/// <returns>The number of characters required for the output string,
/// including the terminating zero.</returns>
/// <exception cref="std::system_error">If the conversion failed.
/// </exception>
template<class TTo, class TFrom>
std::enable_if_t<sizeof(TTo) == sizeof(TFrom), std::size_t> convert_string(
    _Out_writes_to_opt_(cnt_dst, return) TTo *dst,
    _In_ std::size_t cnt_dst,
    _In_reads_or_z_opt_(cnt_src) const TFrom *src,
    _In_ int cnt_src,
    _In_opt_z_ const code_page reserved);


/// <summary>
/// Performs a conversion to an STL string.
/// </summary>
/// <typeparam name="TTo">The character type to convert to. Note that only
/// one of <typeparamref name="TTo" /> and <typeparamref name="TFrom" />
/// can be a byte-sized type, the other must be UTF-16 or UTF-32.</typeparam>
/// <typeparam name="TFrom">The character type to convert from.</typeparam>
/// <param name="src">The string to be converted. It is safe to pass
/// <see langword="nullptr" />, in which case the result will be an empty
/// string.</param>
/// <param name="cnt_src">The length of the source string, which can be -1
/// if <paramref name="src" /> is null-terminated.</param>
/// <param name="code_page">The code page used for the multi-byte character
/// string.</param>
/// <returns>The converted string.</returns>
/// <exception cref="std::system_error">If the conversion failed.
/// </exception>
template<class TTo, class TFrom>
std::enable_if_t<sizeof(TTo) != sizeof(TFrom), std::basic_string<TTo>>
convert_string(_In_reads_or_z_opt_(cnt_src) const TFrom *src,
    _In_ int cnt_src,
    _In_opt_z_ const code_page code_page);


/// <summary>
/// Performs a trivial conversion to an STL string.
/// </summary>
/// <remarks>
/// This overload is used to convert between bitwise identical types such as
/// <see langword="wchar_t" /> and <see langword="char16_t" /> on Windows or
/// <see langword="wchar_t" /> and <see langword="char32_t" /> on Linux.
/// </remarks>
/// <typeparam name="TTo">The character type to convert to.</typeparam>
/// <typeparam name="TFrom">The character type to convert from.</typeparam>
/// <param name="src">The string to be converted. It is safe to pass
/// <see langword="nullptr" />, in which case the result will be an empty
/// string.</param>
/// <param name="cnt_src">The length of the source string, which can be -1
/// if <paramref name="src" /> is null-terminated.</param>
/// <param name="code_page">The code page used for the multi-byte character
/// string.</param>
/// <returns>The converted string.</returns>
/// <exception cref="std::system_error">If the conversion failed.
/// </exception>
template<class TTo, class TFrom>
std::enable_if_t<sizeof(TTo) == sizeof(TFrom), std::basic_string<TTo>>
convert_string(_In_reads_or_z_opt_(cnt_src) const TFrom *src,
    _In_ int cnt_src,
    _In_opt_z_ const code_page code_page);


/// <summary>
/// Performs a trivial conversion to an STL string.
/// </summary>
/// <typeparam name="TChar">The character type used by the string.</typeparam>
/// <param name="src">The string to be copied, which must be either at least
/// <paramref name="cnt_src" /> characters long or zero-terminated if
/// <paramref name="cnt_src" /> is -1. It is safe to pass
/// <see langword="nullptr" />, in which case the output string will be empty.
/// </param>
/// <param name="cnt_src">The number of characters in <paramref name="src" /> or
/// -1 to use the full null-terminated input.</param>
/// <param name="reserved">This parameter is ignored.</param>
/// <returns>A new string containing <paramref name="src" /> or a substring
/// thereof.</returns>
template<class TChar>
inline std::basic_string<TChar> convert_string(
        _In_reads_or_z_opt_(cnt_src) const TChar *src,
        _In_ int cnt_src,
        _In_opt_z_ const code_page reserved) {
    return convert_string<TChar>(src, cnt_src, reserved);
}


/// <summary>
/// Passes through the input string or a substring thereof.
/// </summary>
/// <typeparam name="TChar">The character type used for the input string.
/// </typeparam>
/// <typeparam name="TTraits">The traits used by the input string.</typeparam>
/// <typeparam name="TAlloc">The allocator used by the input string.</typeparam>
/// <param name="src">The input strin, which is also the output string.</param>
/// <param name="cnt">If not negative, the output string will be truncated to
/// this number of characters.</param>
/// <param name="reserved">This parameter is ignored.</param>
/// <returns><paramref name="src" /> or a substring thereof.</returns>
template<class TChar, class TTraits, class TAlloc>
std::basic_string<TChar, TTraits, TAlloc> convert_string(
    _In_ const std::basic_string<TChar, TTraits, TAlloc>& src,
    _In_ const int cnt,
    _In_opt_z_ const code_page reserved);


/// <summary>
/// Measures how many code units of <typeparamref name="TDstChar" /> are
/// required for <paramref name="str" />.
/// </summary>
/// <remarks>
/// This is a convenience function for <see cref="convert_string" /> without
/// a valid output buffer.
/// </remarks>
/// <typeparam name="TDstChar">The character type to convert to.</typeparam>
/// <typeparam name="TSrcChar">The character type to convert from.</typeparam>
/// <param name="src">The source string.</param>
/// <param name="cnt_src">The length of the source string, which can be -1
/// if <paramref name="src" /> is null-terminated.</param>
/// <param name="code_page"></param>
/// <returns>The number of characters required for the converted string,
/// including the terminating zero.</returns>
/// <exception cref="std::invalid_argument">If <paramref name="src" /> is
/// <c>nullptr</c>.</exception>
/// <exception cref="std::system_error">If the conversion is not possible.
/// </exception>
template<class TDstChar, class TSrcChar>
inline std::size_t measure_string(
        _In_reads_or_z_opt_(cnt_src) const TSrcChar *str,
        _In_ const int cnt,
        _In_opt_z_ const code_page code_page) {
    return convert_string(
        static_cast<TDstChar *>(nullptr), 0,
        str, cnt,
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
/// <typeparam name="TDstChar">The character type to convert to.</typeparam>
/// <typeparam name="TSrcChar">The character type to convert from.</typeparam>
/// <param name="src">The source string, which must be null-terminated.</param>
/// <param name="code_page"></param>
/// <returns>The number of characters required for the converted string,
/// including the terminating zero.</returns>
/// <exception cref="std::invalid_argument">If <paramref name="src" /> is
/// <c>nullptr</c>.</exception>
/// <exception cref="std::system_error">If the conversion is not possible.
/// </exception>
template<class TDstChar, class TSrcChar>
inline std::size_t measure_string(
        _In_opt_z_ const TSrcChar *str,
        _In_opt_z_ const code_page code_page) {
    return convert_string(
        static_cast<TDstChar *>(nullptr), static_cast<std::size_t>(0),
        str, -1,
        code_page);
}


/// <summary>
/// Measures how many <i>bytes</i> a string in in the given encoding
/// <paramref name="src_code_page" /> would require in
/// <paramref name="dst_code_page" />.
/// </summary>
/// <typeparam name="TChar"></typeparam>
/// <param name="dst_code_page"></param>
/// <param name="str"></param>
/// <param name="cnt"></param>
/// <param name="src_code_page"></param>
/// <returns>The number of <i>bytes</i> the input string would require in
/// the output encoding.</returns>
template<class TChar>
std::size_t measure_string(
    _In_opt_z_ const code_page dst_code_page,
    _In_ const TChar *str,
    _In_ int cnt,
    _In_opt_z_ const code_page src_code_page);


/// <summary>
/// Measures how many <i>byte</i> a string in in the given encoding
/// <paramref name="src_code_page" /> would require in
/// <paramref name="dst_code_page" />.
/// </summary>
/// <typeparam name="TChar"></typeparam>
/// <param name="dst_code_page"></param>
/// <param name="str"></param>
/// <param name="cnt"></param>
/// <param name="src_code_page"></param>
/// <returns></returns>
template<class TChar> inline std::size_t measure_string(
        _In_opt_z_ const code_page dst_code_page,
        _In_ const TChar *str,
        _In_opt_z_ const code_page src_code_page) {
    return measure_string(dst_code_page, str, -1, src_code_page);
}


/// <summary>
/// Measure the length of the given <paramref name="str" />, but return at most
/// <paramref name="cnt" /> if it is non-negative.
/// </summary>
/// <typeparam name="TChar"></typeparam>
/// <param name="str">The string to be measured.</param>
/// <param name="cnt">If not negative, the maximum return value.</param>
/// <param name="reserved">This parameter is ignored.</param>
/// <returns>The length of <paramref name="str" /> including the terminating
/// null character, but at most <paramref name="cnt" /> if it is not -1.
/// </returns>
template<class TChar> std::size_t measure_string(
    _In_reads_or_z_opt_(cnt) const TChar *str,
    _In_ const int cnt,
    _In_opt_z_ const code_page reserved);


/// <summary>
/// Measure the length of the given <paramref name="str" />.
/// </summary>
/// <typeparam name="TChar"></typeparam>
/// <param name="str">The string to be measured. It is safe to pass
/// <see langword="nullptr" />, in which case the answer is zero.</param>
/// <param name="reserved">This parameter is ignored.</param>
/// <returns>The length of <paramref name="str" /> including the terminating
/// null character.</returns>
template<class TChar> inline std::size_t measure_string(
        _In_opt_z_ const TChar *str,
        _In_opt_z_ const code_page reserved) {
    return (str != nullptr) ? std::char_traits<TChar>::length(str) + 1 : 0;
}


/// <summary>
/// Measures how many code units of <typeparamref name="TDstChar" /> are
/// required for <paramref name="str" />.
/// </summary>
/// <remarks>
/// This is a convenience function for <see cref="convert_string" /> without
/// a valid output buffer.
/// </remarks>
/// <typeparam name="TDstChar">The character type to convert to.</typeparam>
/// <typeparam name="TSrcChar">The character type to convert from.</typeparam>
/// <typeparam name="TSrcTraits">The character traits used by the source string.
/// </typeparam>
/// <typeparam name="TSrcAlloc">The allocator used by the source string.
/// </typeparam>
/// <param name="src">The source string.</param>
/// <param name="code_page"></param>
/// <returns>The number of characters required for the converted string,
/// including the terminating zero.</returns>
/// <exception cref="std::system_error">If the conversion is not possible.
/// </exception>
template<class TDstChar, class TSrcChar, class TSrcTraits, class TSrcAlloc>
inline std::size_t measure_string(
        _In_ const std::basic_string<TSrcChar, TSrcTraits, TSrcAlloc>& str,
        _In_opt_z_ const code_page code_page) {
    return convert_string(
        static_cast<TDstChar *>(nullptr), 0,
        str.c_str(), str.size() + 1,
        code_page);
}


/// <summary>
/// Answer the number of characters in the given string.
/// </summary>
/// <remarks>
/// This function effectively retrieves the size of <paramref name="str" />,
/// but uses the conversion interface, therefore optimising the actual call
/// to <see cref="convert_string" /> away if the string is not being converted.
/// </remarks>
/// <typeparam name="TDstChar">The character type to convert to.</typeparam>
/// <typeparam name="TSrcChar">The character type to convert from.</typeparam>
/// <typeparam name="TSrcTraits">The character traits used by the source string.
/// </typeparam>
/// <typeparam name="TSrcAlloc">The allocator used by the source string.
/// </typeparam>
/// <param name="src">The source string.</param>
/// <param name="reserved">This parameter is ignored.</param>
/// <returns>The number of characters required for the string, including the
/// terminating zero.</returns>
template<class TChar, class TTraits, class TAlloc>
inline std::size_t measure_string(
        _In_ const std::basic_string<TChar, TTraits, TAlloc>& str,
        _In_opt_z_ const code_page reserved) {
    return (str.size() + 1);
}

FPD_NAMESPACE_END

#include "fpd/convert_string.inl"
