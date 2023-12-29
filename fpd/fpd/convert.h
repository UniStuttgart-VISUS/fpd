// <copyright file="convert.h" company="Visualisierungsinstitut der Universität Stuttgart">
// Copyright © 2023 Visualisierungsinstitut der Universität Stuttgart.
// Licensed under the MIT licence. See LICENCE file for details.
// </copyright>
// <author>Christoph Müller</author>

#pragma once

#include <string>
#include <system_error>

#if defined(ICU_FOUND)
#include <unicode/ucnv.h>
#include <unicode/unistr.h>
#include <unicode/ustring.h>
#include <unicode/utypes.h>
//#include "icu_error_category.h"
#endif /* !defined(ICU_FOUND) */

#include "fpd/code_page.h"


FPD_NAMESPACE_BEGIN

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
std::size_t convert(_Out_writes_to_opt_(cnt_dst, return) wchar_t *dst,
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
std::size_t convert(_Out_writes_to_opt_(cnt_dst, return) char *dst,
    _In_ std::size_t cnt_dst,
    _In_reads_or_z_(cnt_src) const wchar_t *src,
    _In_ const std::ptrdiff_t cnt_src,
    _In_ const code_page code_page);

FPD_NAMESPACE_END

#include "fpd/convert.inl"
