// <copyright file="format_description.inl" company="Visualisierungsinstitut der Universität Stuttgart">
// Copyright © 2023 Visualisierungsinstitut der Universität Stuttgart.
// Licensed under the MIT licence. See LICENCE file for details.
// </copyright>
// <author>Christoph Müller</author>


/*
 * FPD_NAMESPACE::format_description<TChar>::format_description
 */
template<class TChar>
FPD_NAMESPACE::format_description<TChar>::format_description(
        _In_ const std::initializer_list<initialiser_type>& elements,
        _In_ const bool eval_offsets) {
    for (auto& e : elements) {
        this->add(e, eval_offsets);
    }
}


/*
 * FPD_NAMESPACE::format_description<TChar>::add
 */
template<class TChar>
FPD_NAMESPACE::format_description<TChar>&
FPD_NAMESPACE::format_description<TChar>::add(
        _In_ const semantic_type semantic,
        _In_opt_z_ const char_type *semantic_name,
        _In_ const format_type format,
        _In_ const std::size_t group,
        _In_ const std::uint16_t offset,
        _In_ const std::uint16_t size,
        _In_ const bool eval_offset) {
    if (this->_groups.size() <= group) {
        this->_groups.resize(group + 1);
    }

    this->_groups[group].add(semantic, semantic_name, format, offset, size,
        eval_offset);
    return *this;
}


/*
 * FPD_NAMESPACE::format_description<TChar>::add
 */
template<class TChar>
FPD_NAMESPACE::format_description<TChar>&
FPD_NAMESPACE::format_description<TChar>::add(
        _In_ const initialiser_type& initialiser,
        _In_ const bool eval_offset) {
    if (this->_groups.size() <= initialiser._group) {
        this->_groups.resize(initialiser._group + 1);
    }

    this->_groups[initialiser._group].add(initialiser, eval_offset);
    return *this;
}
