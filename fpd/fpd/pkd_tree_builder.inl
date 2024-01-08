// <copyright file="pkd_tree_builder.inl" company="Visualisierungsinstitut der Universität Stuttgart">
// Copyright © 2024 Visualisierungsinstitut der Universität Stuttgart.
// Licensed under the MIT licence. See LICENCE file for details.
// </copyright>
// <author>Christoph Müller</author>


/*
 * FPD_NAMESPACE::pkd_tree_builder<TChar>::pkd_tree_builder
 */
template<class TChar>
FPD_NAMESPACE::pkd_tree_builder<TChar>::pkd_tree_builder(void) noexcept
    : _dimensions(0),
    _format(format::unknown),
    _group(0),
    _offset(0),
    _stride(0, 0) { }


/*
 * FPD_NAMESPACE::pkd_tree_builder<TChar>::build
 */
template<class TChar>
FPD_NAMESPACE::pkd_tree_builder<TChar>&
FPD_NAMESPACE::pkd_tree_builder<TChar>::build(_In_ void *particles,
        _In_ const std::size_t cnt) {
    if ((cnt > 0) && (particles == nullptr)) {
        throw std::invalid_argument("A valid pointer to the particles must be "
            "provided.");
    }

    auto positions = positions(particles, cnt);

    throw "TODO";
}


/*
 * FPD_NAMESPACE::pkd_tree_builder<TChar>::for_format
 */
template<class TChar>
FPD_NAMESPACE::pkd_tree_builder<TChar>&
FPD_NAMESPACE::pkd_tree_builder<TChar>::for_format(
        _In_ const format_description_type& desc) {
    // Search all flexible elements for one with position semantics. If we have
    // that, we know the group, the offset and the format of the position, which
    // also encodes the dimensionality of the space.
    this->_dimensions = 0;
    this->_format = format::unknown;
    this->_group = 0;
    this->_offset = 0;
    this->_stride.first = 0;

    for (auto& g : desc) {
        auto e = g.find(semantic::position);
        if (e != g.end()) {
            this->_format = e->format();
            this->_dimensions = to_components(this->_format);
            this->_offset += g.offset(*e);
            this->_stride.second = g.size();

            // If this is a combined position + radius semantic, we need to
            // subtract one dimension that is used by the radius.
            if (e->has_semantic(semantic::radius)) {
                assert(this->_dimensions > 1);
                --this->_dimensions;
            }
            break;
        }

        ++this->_group;
        this->_stride.first += g.size();
    }

    if (this->_format == format::unknown) {
        throw std::invalid_argument("The specified format description does not "
            "hold a usable position.");
    }

    assert(this->_format != format::unknown);
    assert(this->_dimensions > 0);
    assert(this->_group >= 0);
    assert(this->_group < desc.count_groups());
    assert(this->_stride.second > 0);
    return *this;
}
