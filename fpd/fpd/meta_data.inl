// <copyright file="meta_data.inl" company="Visualisierungsinstitut der Universität Stuttgart">
// Copyright © 2023 Visualisierungsinstitut der Universität Stuttgart.
// Licensed under the MIT licence. See LICENCE file for details.
// </copyright>
// <author>Christoph Müller</author>


/*
 * FPD_NAMESPACE::meta_data<TChar>::meta_data
 */
template<class TChar>
FPD_NAMESPACE::meta_data<TChar>::meta_data(void) noexcept
    : _endian(endian_type::undefined), _file_size(0), _version(0, 0) { }


/*
 * FPD_NAMESPACE::meta_data<TChar>::frame_offset
 */
template<class TChar>
typename FPD_NAMESPACE::meta_data<TChar>::size_type
FPD_NAMESPACE::meta_data<TChar>::frame_offset(
        _In_ const std::size_t frame) const {
    if (frame >= this->_frame_table.size()) {
        throw std::range_error("The specified frame is out of range.");
    }

    return this->_frame_table[frame];
}


/*
 * FPD_NAMESPACE::meta_data<TChar>::frame_size
 */
template<class TChar>
typename FPD_NAMESPACE::meta_data<TChar>::size_type
FPD_NAMESPACE::meta_data<TChar>::frame_size(
        _In_ const std::size_t frame) const {
    if (frame >= this->_frame_table.size()) {
        throw std::range_error("The specified frame is out of range.");
    }

    const auto cur = this->_frame_table[frame];
    const auto nxt = ((frame + 1) < this->_frame_table.size())
        ? this->_frame_table[frame + 1]
        : this->_file_size;

    if (nxt < cur) {
        throw std::logic_error("The meta data block does not hold the correct "
            "size of the file or the frame table is not sorted.");
    }

    return (nxt - cur);
}


/*
 * FPD_NAMESPACE::meta_data<TChar>::group_offset
 */
template<class TChar>
typename FPD_NAMESPACE::meta_data<TChar>::size_type
FPD_NAMESPACE::meta_data<TChar>::group_offset(_In_ const std::size_t frame,
        _In_ const std::size_t group) const {
    if (group >= this->_format_description.count()) {
        throw std::range_error("The specified element group is out of range.");
    }

    const auto particles = this->particles(frame);
    size_type retval = 0;

    for (std::size_t i = 0; i < group; ++i) {
        retval += particles * this->_format_description[i].size();
    }

    return retval;
}


/*
 * FPD_NAMESPACE::meta_data<TChar>::user_defined_semantic
 */
template<class TChar>
const typename FPD_NAMESPACE::meta_data<TChar>::string_type&
FPD_NAMESPACE::meta_data<TChar>::user_defined_semantic(
        _In_ const semantic_type semantic) const {
    const auto f = static_cast<std::size_t>(semantic::user_defined);
    const auto i = static_cast<std::size_t>(semantic);

    if (i < f) {
        throw std::invalid_argument("Only user-defined semantics are stored "
            "in the global semantics table.");
    }
    if (i >= this->_user_defined_semantics.size()) {
        throw std::invalid_argument("The specified user-defined semantic is "
            "invalid.");
    }

    return this->_user_defined_semantics[i - f];
}
