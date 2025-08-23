// <copyright file="element.inl" company="Visualisierungsinstitut der Universität Stuttgart">
// Copyright © 2023 - 2025 Visualisierungsinstitut der Universität Stuttgart.
// Licensed under the MIT licence. See LICENCE file for details.
// </copyright>
// <author>Christoph Müller</author>


/*
 * FPD_NAMESPACE::element<TChar>::size_on_disk
 */
template<class TChar>
constexpr const std::size_t FPD_NAMESPACE::element<TChar>::size_on_disk;


/*
 * FPD_NAMESPACE::element<TChar>::element
 */
template<class TChar>
FPD_NAMESPACE::element<TChar>::element(
        _In_ const semantic_type semantic,
        _In_opt_z_ const char_type *semantic_name,
        _In_ const format_type format,
        _In_ const std::uint16_t offset,
        _In_ const std::uint16_t size)
    : _format(format),
        _offset(offset),
        _semantic(semantic),
        _semantic_name(null_coalesce(semantic_name)),
        _size(size_coalesce(size, format)) {
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
 * FPD_NAMESPACE::element<TChar>::element
 */
template<class TChar>
FPD_NAMESPACE::element<TChar>::element(
        _In_ const semantic_type semantic,
        _In_ const format_type format,
        _In_ const std::uint16_t offset,
        _In_ const std::uint16_t size)
    : _format(format),
        _offset(offset),
        _semantic(semantic),
        _semantic_name(to_string<TChar>(semantic)),
        _size(size_coalesce(size, format)) {
    if (this->_semantic_name.empty()) {
        throw std::invalid_argument("Only built-in semantics can be "
            "can be used without providing a name.");
    }

    if (this->_size == 0) {
        throw std::invalid_argument("A valid size of the element must be "
            "provided it the size cannot be derived from the format.");
    }
}


/*
 * FPD::NAMESPACE::element<TChar>::element
 */
template<class TChar>
FPD_NAMESPACE::element<TChar>::element(_In_ const initialiser_type& initialiser,
        _In_ const std::uint16_t offset_override)
    : _format(initialiser._format),
        _offset(initialiser._offset),
        _semantic(initialiser._semantic),
        _semantic_name(null_coalesce(initialiser._semantic_name)),
        _size(size_coalesce(initialiser._size, initialiser._format)) {
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

    if (this->_offset == 0) {
        this->_offset = offset_override;
    }
}
