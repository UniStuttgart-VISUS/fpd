// <copyright file="icu_category.h" company="Visualisierungsinstitut der Universität Stuttgart">
// Copyright © 2024 Visualisierungsinstitut der Universität Stuttgart.
// Licensed under the MIT licence. See LICENCE file for details.
// </copyright>
// <author>Christoph Müller</author>

#pragma once

#include <system_error>

#if defined(ICU_FOUND)
#include <unicode/ucnv.h>
#include <unicode/unistr.h>
#include <unicode/ustring.h>
#include <unicode/utypes.h>

#include "fpd/abi.h"
#include "fpd/annotations.h"


FPD_NAMESPACE_BEGIN

/// <summary>
/// An error category for error codes from libicu, which we use for string
/// conversion on non-Windows systems.
/// </summary>
class icu_error_category final : public std::error_category {

public:

    using std::error_category::error_category;

    /// <summary>
    /// Converts the error code into a portable description.
    /// </summary>
    /// <param name="error">The error code to be converted.</param>
    /// <returns>The portable description of the ICU error.</returns>
    inline std::error_condition default_error_condition(
            _In_ int error) const noexcept override {
        const auto code = static_cast<UErrorCode>(error);
        return std::error_condition(code, icu_category());
    }

    /// <summary>
    /// Convert the given error code into a string.
    /// </summary>
    /// <param name="error">The error code to get the message for.</param>
    /// <returns>The error message associated with the error code.
    /// </returns>
    inline std::string message(_In_ int error) const override {
        const auto code = static_cast<UErrorCode>(error);
        return std::to_string(error);
        //    const auto error_name = ::u_errorName(code);
        //
        //    if (error_name == nullptr) {
        //        return std::to_string(error);
        //    }
        //    
        ////    (UChar *)ures_getStringByKey(gBundle, errname, &msgLen, &subErr);
    }

    /// <summary>
    /// Answer the name of the error category.
    /// </summary>
    /// <returns>The name of the category.</returns>
    inline _Ret_z_ const char *name(void) const noexcept override {
        return "ICU";
    }
};


/// <summary>
/// Answer the one and only <see cref="icu_error_category" />.
/// </summary>
/// <returns>The only instance of <see cref="icu_error_category" />.
/// </returns>
inline _Ret_valid_ const std::error_category& icu_category(void) noexcept {
    // TODO: Does inline + static work?
    static const icu_error_category retval;
    return retval;
}

FPD_NAMESPACE_END

#endif /* !defined(ICU_FOUND) */
