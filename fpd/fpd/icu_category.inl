// <copyright file="icu_category.inl" company="Visualisierungsinstitut der Universität Stuttgart">
// Copyright © 2025 Visualisierungsinstitut der Universität Stuttgart.
// Licensed under the MIT licence. See LICENCE file for details.
// </copyright>
// <author>Christoph Müller</author>


/*
 * FPD_DETAIL_NAMESPACE::icu_error_category<TErrorCode>::default_error_condition
 */
template<class TErrorCode>
std::error_condition
FPD_DETAIL_NAMESPACE::icu_error_category<TErrorCode>::default_error_condition(
        _In_ const int error) const noexcept {
    return std::error_condition(error, icu_category());
}


/*
 * FPD_DETAIL_NAMESPACE::icu_error_category<TErrorCode>::message
 */
template<class TErrorCode>
std::string FPD_DETAIL_NAMESPACE::icu_error_category<TErrorCode>::message(
        _In_ const int error) const {
    const auto code = static_cast<UErrorCode>(error);
    const auto name = ::u_errorName(code);

    if (name != nullptr) {
        return name;
    } else {
        return std::to_string(error);
    }
    ////    (UChar *)ures_getStringByKey(gBundle, errname, &msgLen, &subErr);
}


/*
 * FPD_DETAIL_NAMESPACE::icu_error_category<TErrorCode>::name
 */
template<class TErrorCode>
_Ret_z_ const char *FPD_DETAIL_NAMESPACE::icu_error_category<TErrorCode>::name(
        void) const noexcept {
    return "ICU";
}


/*
 * FPD_DETAIL_NAMESPACE::throw_if_icu_failed
 */
template<class TErrorCode>
void FPD_DETAIL_NAMESPACE::throw_if_icu_failed(_In_ const TErrorCode error,
        _In_opt_z_ const char *msg) {
    if (U_FAILURE(error)) {
        if (msg != nullptr) {
            throw std::system_error(error, icu_category(), msg);
        } else {
            throw std::system_error(error, icu_category());
        }
    }
}
