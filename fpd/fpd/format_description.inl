// <copyright file="format_description.inl" company="Visualisierungsinstitut der Universität Stuttgart">
// Copyright © 2023 - 2025 Visualisierungsinstitut der Universität Stuttgart.
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


/*
 * FPD_NAMESPACE::format_description<TChar>::operator ()
 */
template<class TChar>
template<class TPointer>
_Ret_valid_ std::enable_if_t<sizeof(TPointer) == 1, TPointer *>
FPD_NAMESPACE::format_description<TChar>::operator ()(
        _In_ TPointer *particles,
        _In_ const std::size_t cnt,
        _In_ const std::size_t group,
        _In_ const std::size_t particle) const {
    static_assert(sizeof(TPointer) == 1, "The input pointer must be byte-sized "
        "in order for the pointer arithmetics to work.");
    if (particles == nullptr) {
        throw std::invalid_argument("A valid pointer to particle data must be "
            "provided.");
    }
    if (group >= this->_groups.size()) {
        throw std::out_of_range("The specified group index is out of range.");
    }
    if (particle >= cnt) {
        throw std::out_of_range("The specified particle index is out of "
            "range.");
    }

    // Compute the per-particle offset of 'group'.
    std::size_t offset = 0;
    for (std::size_t i = 0; i < group; ++i) {
        assert(i < this->_groups.size());
        offset += this->_groups[i].size();
    }

    // Compute the offset of the group itself.
    offset *= cnt;

    // Add the offset of the particle within the group.
    offset += particle * this->_groups[group].size();

    return particles + offset;
}
