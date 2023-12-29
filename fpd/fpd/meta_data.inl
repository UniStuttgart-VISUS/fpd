// <copyright file="meta_data.inl" company="Visualisierungsinstitut der Universität Stuttgart">
// Copyright © 2023 Visualisierungsinstitut der Universität Stuttgart.
// Licensed under the MIT licence. See LICENCE file for details.
// </copyright>
// <author>Christoph Müller</author>


/*
 * FPD_NAMESPACE::meta_data<TChar>::check_header_size
 */
template<class TChar>
std::size_t FPD_NAMESPACE::meta_data<TChar>::check_header_size(
        _In_ const void *data, _In_ const std::size_t cnt) {
    if (data == nullptr) {
        throw std::invalid_argument("The header data must be valid.");
    }
    if (cnt < fixed_header_size) {
        throw std::invalid_argument("The provided header data are not "
            "large enough to determine the overall header size.");
    }

    // Check for the magic cookie and skip the version as we are not interested
    // in it at this point.
    const auto bytes = static_cast<const std::uint8_t *>(data);
    if ((bytes[0] != 'F') || (bytes[1] != 'P') || (bytes[2] != 'D')) {
        throw std::invalid_argument("The provided header data does "
            "not start with the FPD magic cookie.");
    }

    // Determine the endian, which we need to potentially swap the number
    // we return.
    const auto file_endian = *reinterpret_cast<const endian_type *>(bytes + 5);

    // Determine the overall size of the header. If necessary, swap its byte
    // order to obtain something the caller can use.
    const auto retval = swap_endian(*reinterpret_cast<const std::uint32_t *>(
        bytes + 3 * sizeof(char) + 2 * sizeof(std::uint8_t)
        + sizeof(file_endian)),
        file_endian);

    return retval;
}


/*
 * FPD_NAMESPACE::meta_data meta_data<TChar>::parse
 */
template<class TChar>
FPD_NAMESPACE::meta_data<TChar> FPD_NAMESPACE::meta_data<TChar>::parse(
        _In_ const void *data, _In_ const std::size_t cnt) {
    const auto segments_size = check_header_size(data, cnt);
    throw "TODO";
}


/*
 * FPD_NAMESPACE::meta_data<TChar>::fixed_header_size
 */
template<class TChar>
std::size_t FPD_NAMESPACE::meta_data<TChar>::fixed_header_size;


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
