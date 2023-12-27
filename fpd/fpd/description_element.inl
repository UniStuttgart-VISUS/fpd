// <copyright file="description_element.inl" company="Visualisierungsinstitut der Universität Stuttgart">
// Copyright © 2023 Visualisierungsinstitut der Universität Stuttgart.
// Licensed under the MIT licence. See LICENCE file for details.
// </copyright>
// <author>Christoph Müller</author>


/*
 * FPD_NAMESPACE::description_element<TChar>::description_element
 */
template<class TChar>
FPD_NAMESPACE::description_element<TChar>::description_element(
        _In_ const semantic_type semantic,
        _In_ const string_type& semantic_name,
        _In_ const format_type format,
        _In_ const std::uint16_t offset,
        _In_ const std::uint16_t size)
    : _format(format),
        _offset(offset),
        _semantic(semantic),
        _semantic_name(semantic_name),
        _size((size > 0) ? size : static_cast<std::uint16_t>(to_size(format))) {
    if (this->_semantic >= semantic_type::user_defined) {
        // If we have a user-defined semantic, the caller must provide a
        // human-readable description for it.
        if (this->_semantic_name.empty()) {
            throw std::invalid_argument("A valid name of the semantic must "
                "be provided for all user-defined semantics.");
        }
    } else {
        // If we have a built-in semantic, force our built-in name.
        this->_semantic_name = to_string<TChar>(this->_semantic);
    }

    if (this->_size == 0) {
        throw std::invalid_argument("A valid size of the element must be "
            "provided it the size cannot be derived from the format.");
    }
}


/*
 * FPD_NAMESPACE::description_element<TChar>::description_element
 */
template<class TChar>
FPD_NAMESPACE::description_element<TChar>::description_element(
        _In_ const semantic_type semantic,
        _In_ const format_type format,
        _In_ const std::uint16_t offset,
        _In_ const std::uint16_t size)
    : _format(format),
        _offset(offset),
        _semantic(semantic),
        _semantic_name(to_string<TChar>(semantic)),
        _size((size > 0) ? size : static_cast<std::uint16_t>(to_size(format))) {
    if (this->_semantic_name.empty()) {
        throw std::invalid_argument("Only built-in semantics can be "
            "can be used without providing a name.");
    }

    if (this->_size == 0) {
        throw std::invalid_argument("A valid size of the element must be "
            "provided it the size cannot be derived from the format.");
    }
}
