// <copyright file="com_category.h" company="Visualisierungsinstitut der Universität Stuttgart">
// Copyright © 2024 Visualisierungsinstitut der Universität Stuttgart.
// Licensed under the MIT licence. See LICENCE file for details.
// </copyright>
// <author>Christoph Müller</author>

#pragma once

#include <system_error>

#if defined(_WIN32)
#include <comdef.h>
#include <Windows.h>

#include "fpd/convert_string.h"


FPD_NAMESPACE_BEGIN


/* Forward declarations. */
_Ret_valid_ const std::error_category& com_category(void) noexcept;


/// <summary>
/// An error category for COM error codes.
/// </summary>
class com_error_category final : public std::error_category {

public:

    using std::error_category::error_category;

    /// <summary>
    /// Converts the error code into a portable description.
    /// </summary>
    /// <param name="hr">The error code to be converted.</param>
    /// <returns>The portable description of the COM error.</returns>
    inline std::error_condition default_error_condition(
            _In_ int hr) const noexcept override {
        auto error = HRESULT_CODE(hr);

        if (error || (hr == ERROR_SUCCESS)) {
            // 'hr' is a default Windows code mapped to HRESULT.
            return std::system_category().default_error_condition(error);
        } else {
            // This is an actual COM error.
            return std::error_condition(hr, com_category());
        }
    }

    /// <summary>
    /// Convert the given error code into a string.
    /// </summary>
    /// <param name="hr">The error code to get the message for.</param>
    /// <returns>The error message associated with the error code.
    /// </returns>
    inline std::string message(_In_ int hr) const override {
        _com_error e(hr);
#if (defined(UNICODE) || defined(_UNICODE))
        return convert_string<char>(e.ErrorMessage(), -1, encodings::oem);
#else /* (defined(UNICODE) || defined(_UNICODE)) */
        return e.ErrorMessage();
#endif /* (defined(UNICODE) || defined(_UNICODE)) */
    }

    /// <summary>
    /// Answer the name of the error category.
    /// </summary>
    /// <returns>The name of the category.</returns>
    inline _Ret_z_ const char *name(void) const noexcept override {
        return "Component Object Model";
    }
};


/// <summary>
/// Answer the one and only <see cref="com_error_category" />.
/// </summary>
/// <returns>The only instance of <see cref="com_error_category" />.
/// </returns>
inline _Ret_valid_ const std::error_category& com_category(void) noexcept {
    // TODO: Does inline + static work?
    static const com_error_category retval;
    return retval;
}

FPD_NAMESPACE_END

#endif /* !defined(_WIN32) */
