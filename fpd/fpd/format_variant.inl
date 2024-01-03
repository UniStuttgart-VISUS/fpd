// <copyright file="format_variant.inl" company="Visualisierungsinstitut der Universität Stuttgart">
// Copyright © 2023 Visualisierungsinstitut der Universität Stuttgart.
// Licensed under the MIT licence. See LICENCE file for details.
// </copyright>
// <author>Christoph Müller</author>


#define _FPD_VARIANT_FORMATS FPD_NAMESPACE::format_list<Formats...>

/*
 * ...::basic_format_variant<_FPD_VARIANT_FORMATS>::basic_format_variant
 */
template<FPD_NAMESPACE::format... Formats>
template<class TValue>
FPD_NAMESPACE::basic_format_variant<_FPD_VARIANT_FORMATS>::basic_format_variant(
        TValue&& value) : _format(format_reverse_traits<TValue>::format) {
    auto dst = this->try_get<TValue>();
    if (dst == nullptr) {
        throw std::invalid_argument("The variant does not support the "
            "specified kind of value.");
    } else {
        new (dst) TValue(std::forward<TValue>(value));
    }
}


/*
 * FPD_NAMESPACE::basic_format_variant<_FPD_VARIANT_FORMATS>::get
 */
template<FPD_NAMESPACE::format... Formats>
template<class TValue>
const TValue& FPD_NAMESPACE::basic_format_variant<_FPD_VARIANT_FORMATS>::get(
        void) const {
    auto retval = this->try_get<TValue>();
    if (retval == nullptr) {
        throw std::logic_error("The variant does not hold the expected kind of "
            "data.");
    } else {
        return *retval;
    }
}


/*
 * FPD_NAMESPACE::basic_format_variant<_FPD_VARIANT_FORMATS>::get
 */
template<FPD_NAMESPACE::format... Formats>
template<class TValue>
TValue& FPD_NAMESPACE::basic_format_variant<_FPD_VARIANT_FORMATS>::get(void) {
    auto retval = this->try_get<TValue>();
    if (retval == nullptr) {
        throw std::logic_error("The variant does not hold the expected kind of "
            "data.");
    } else {
        return *retval;
    }
}

#undef _FPD_VARIANT_FORMATS
