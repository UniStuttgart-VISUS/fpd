// <copyright file="icu_category.h" company="Visualisierungsinstitut der Universität Stuttgart">
// Copyright © 2025 Visualisierungsinstitut der Universität Stuttgart.
// Licensed under the MIT licence. See LICENCE file for details.
// </copyright>
// <author>Christoph Müller</author>

#if !defined(_FPD_ICU_CATEGORY_H_)
#define _FPD_ICU_CATEGORY_H_
#pragma once

#include <string>
#include <system_error>

#if defined(_WIN32)
#if (NTDDI_VERSION >= NTDDI_WIN10_RS2)
#include <icu.h>
#else /* (NTDDI_VERSION >= NTDDI_WIN10_RS2) */
#include <icucommon.h>
#include <icui18n.h>
#endif /* (NTDDI_VERSION >= NTDDI_WIN10_RS2) */

#else /* defined(_WIN32) */
#include <unicode/utypes.h>
#endif /* defined(_WIN32) */

#include "fpd/abi.h"


FPD_DETAIL_NAMESPACE_BEGIN

/// <summary>
/// An error category for error codes from libicu, which we use for string
/// conversion.
/// </summary>
/// <typeparam name="TErrorCode">The type of the error code used by libicu.
/// </typeparam>
template<class TErrorCode = UErrorCode>
class icu_error_category final : public std::error_category {

public:

    using std::error_category::error_category;

    /// <summary>
    /// Converts the error code into a portable description.
    /// </summary>
    /// <param name="error">The error code to be converted.</param>
    /// <returns>The portable description of the ICU error.</returns>
    std::error_condition default_error_condition(
        _In_ const int error) const noexcept override;

    /// <summary>
    /// Convert the given error code into a string.
    /// </summary>
    /// <param name="error">The error code to get the message for.</param>
    /// <returns>The error message associated with the error code.
    /// </returns>
    inline std::string message(_In_ const int error) const override;

    /// <summary>
    /// Answer the name of the error category.
    /// </summary>
    /// <returns>The name of the category.</returns>
    _Ret_z_ const char *name(void) const noexcept override;
};


/// <summary>
/// Answer the one and only <see cref="icu_error_category" />.
/// </summary>
/// <returns>The only instance of <see cref="icu_error_category" />.
/// </returns>
_Ret_valid_ inline const std::error_category& icu_category(void) noexcept {
    static const icu_error_category<> instance;
    return instance;
}

/// <summary>
/// Tests whether <paramref name="error" /> indicates failure and throws a
/// <see cref="std::system_error" /> if this is the case.
/// </summary>
/// <typeparam name="TErrorCode">The type of the error code used by libicu.
/// </typeparam>
/// <param name="error">The error code to be checked.</param>
/// <param name="msg">An optional custom error message.</param>
/// <exception cref="std::system_error">If <paramref name="error" /> indicates
/// failure.</exception>
template<class TErrorCode>
void throw_if_icu_failed(_In_ const TErrorCode error,
    _In_opt_z_ const char *msg = nullptr);

FPD_DETAIL_NAMESPACE_END

#include "fpd/icu_category.inl"

#endif /* !defined(_FPD_ICU_CATEGORY_H_) */
