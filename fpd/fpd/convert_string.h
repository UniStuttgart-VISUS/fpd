// <copyright file="convert_string.h" company="Visualisierungsinstitut der Universität Stuttgart">
// Copyright © 2023 - 2025 Visualisierungsinstitut der Universität Stuttgart.
// Licensed under the MIT licence. See LICENCE file for details.
// </copyright>
// <author>Christoph Müller</author>

#pragma once

#include <algorithm>
#include <array>
#include <cassert>
#include <cstring>
#include <memory>
#include <string>
#include <type_traits>

#include "fpd/encoding.h"
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
    _In_opt_z_ const encoding code_page);


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
    _In_opt_z_ const encoding code_page);


/// <summary>
/// Convert a string from one encoding to another using an intermediate
/// step via UTF-16.
/// </summary>
/// <remarks>
/// <para>While this function can convert between arbitrary encodings, it pivots
/// to the canonical UTF-16 to perform the task. The conversion therefore might
/// be rather expensive. If you can use UTF-16 directly, the other overloads of
/// this function might be more appropriate for the task.</para>
/// <para>Please be aware that LibICU will output a BOM when converting to
/// UTF-16 or UTF-32 without specifying the byte order in the encodings.</para>
/// </remarks>
/// <typeparam name="TTo">The type used to represent the output string. It is
/// important that the size of an individual character matches the size of a
/// code unit of <paramref name="dst_code_page" />.</typeparam>
/// <typeparam name="TFrom">The type used to represent the input string. It is
/// important that the size of an individual character matches the size of a
/// code unit of <paramref name="src_code_page" />.</typeparam>
/// <param name="dst">The destination buffer used to store the converted
/// data. If this is <see langword="nullptr" />, the required buffer size will
/// be measured, but nothing will be written.</param>
/// <param name="cnt_dst">The size of <paramref name="dst" /> in number of
/// characters of type <typeparamref name="TTo" />.</param>
/// <param name="dst_code_page">The encoding to convert to.</param>
/// <param name="src">The input string, which may be null-terminated.</param>
/// <param name="cnt_src">The length of the input string in number of characters of
/// type <typeparamref name="TFrom" />, or -1 if the input is null-terminated.
/// </param>
/// <param name="src_code_page">The encoding of <paramref name="src" />.</param>
/// <returns>The number of code units required to store the output string,
/// including the null terminator if the input was null terminated.</returns>
template<class TTo, class TFrom>
std::size_t convert_string(_Out_writes_to_opt_(cnt_dst, return) TTo *dst,
    _In_ std::size_t cnt_dst,
    _In_opt_z_ const encoding dst_code_page,
    _In_reads_or_z_opt_(cnt_src) const TFrom *src,
    _In_ int cnt_src,
    _In_opt_z_ const encoding src_code_page);


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
    _In_opt_z_ const encoding code_page);


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
    _In_opt_z_ const encoding code_page);


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
    _In_opt_z_ const encoding reserved);


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
    _In_opt_z_ const encoding code_page);


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
/// <returns>The converted string.</returns>
/// <exception cref="std::system_error">If the conversion failed.
/// </exception>
template<class TTo, class TFrom>
std::enable_if_t<sizeof(TTo) == sizeof(TFrom), std::basic_string<TTo>>
convert_string(_In_reads_or_z_opt_(cnt_src) const TFrom *src,
    _In_ int cnt_src, _In_opt_z_ const encoding);


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
/// <returns>A new string containing <paramref name="src" /> or a substring
/// thereof.</returns>
template<class TChar> inline std::basic_string<TChar> convert_string(
        _In_reads_or_z_opt_(cnt_src) const TChar *src,
        _In_ int cnt_src,
        _In_opt_z_ const encoding) {
    return convert_string<TChar>(src, cnt_src, nullptr);
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
/// <returns><paramref name="src" /> or a substring thereof.</returns>
template<class TChar, class TTraits, class TAlloc>
std::basic_string<TChar, TTraits, TAlloc> convert_string(
    _In_ const std::basic_string<TChar, TTraits, TAlloc>& src,
    _In_ const int cnt,
    _In_opt_z_ const encoding);

FPD_NAMESPACE_END

#include "fpd/convert_string.inl"
