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
    : _format(format), _offset(offset), _semantic(semantic),
        _semantic_name(semantic_name), _size(size) {
    if (this->_semantic_name.empty()) {
        if (this->_semantic >= semantic_type::user_defined) {
            throw std::invalid_argument("A valid name of the semantic must "
                "be provided for all user-defined semantics.");
        } else {
            this->_semantic_name = to_string(this->_semantic);
        }
    }
}
