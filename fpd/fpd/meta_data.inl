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
        _Out_ meta_data& output,
        _In_ const void *data,
        _In_ const std::size_t cnt) {
    if (data == nullptr) {
        throw std::invalid_argument("The header data must be valid.");
    }
    if (cnt < fixed_header_size) {
        throw std::invalid_argument("The provided header data are not "
            "large enough to determine the overall header size.");
    }

    // Check for the magic cookie.
    auto bytes = static_cast<const std::uint8_t *>(data);
    if ((bytes[0] != 'F') || (bytes[1] != 'P') || (bytes[2] != 'D')) {
        throw std::invalid_argument("The provided header data does "
            "not start with the FPD magic cookie.");
    }
    bytes += 3 * sizeof(char);

    // Retrieve the version, which comes directly after the cookie.
    output._version.first = *bytes++;
    output._version.second = *bytes++;

    // Determine the endian, which we need to potentially swap the number
    // we return.
    output._endian = *reinterpret_cast<const endian_type *>(bytes);
    bytes += sizeof(endian_type);

    // Determine the overall size of the header. If necessary, swap its byte
    // order to obtain something the caller can use.
    const auto retval = swap_endian(*reinterpret_cast<const std::uint32_t *>(
        bytes), output._endian);

    return retval;
}


/*
 * FPD_NAMESPACE::meta_data<TChar>::parse
 */
template<class TChar>
FPD_NAMESPACE::meta_data<TChar>& FPD_NAMESPACE::meta_data<TChar>::parse(
        _Inout_ meta_data& meta_data,
        _In_ const void *data,
        _In_ const std::size_t cnt) {
    auto cur = static_cast<const std::uint8_t * >(data);
    const auto end = cut + cnt;
    auto deferred_format = std::make_pair(static_cast<const void *>(nullptr),
        static_cast<std::size_t>(0));

    while (cur + sizeof(header_segment_type) < end) {
        // Interpret the 32-bit header of the header segment.
        auto header = swap_endian_as<std::uint32_t>(cur, meta_data._endian);
        cur += sizeof(header);

        // Split the header_segments and the length of the segment.
        constexpr const decltype(header) header_segment_mask = (0xFF << 24);
        const auto segment = header_segment_mask & header;
        const auto length = ~header_segment_mask & header;

        // Parse the segment as the recognised type or ignore it allthogether.
        switch (static_cast<header_segment>(segment)) {
            case header_segment::frame_table:
                parse_frame_table(meta_data, cur, length);
                break;

            case header_segment::format_description:
                try {
                    parse_format_description(meta_data, cur, length);
                } catch (...) {
                    // The format will fail if there are user-defined semantics
                    // and we have not yet seen these. Therefore, we postpone
                    // parsing it again until the end.
                    deferred_format.first = cur;
                    deferred_format.second = length;
                }
                break;

            case header_segment::user_semantics:
                parse_user_semantics(meta_data, cur, length);
                break;

            case header_segment::description:
                parse_description(meta_data, cur, length);
                break;

            case header_segment::bounding_box:
                parse_bounding_box(meta_data, cur, length);
                break;

            case header_sgement::global_particle_properties:
                parse_global_particle_properties(meta_data, cur, length);
                break;
        }

        cur += length;
    }

    // If we have postponed parsing the format, do it now. If it fails again,
    // the input is invalid.
    if (deferred_format.first != nullptr) {
        parse_format_description(meta_data, deferred_format.first,
            deferred_format.second);
    }

    return meta_data;
}


/*
 * FPD_NAMESPACE::meta_data meta_data<TChar>::parse
 */
template<class TChar>
FPD_NAMESPACE::meta_data<TChar> FPD_NAMESPACE::meta_data<TChar>::parse(
        _In_ const void *data, _In_ const std::size_t cnt) {
    meta_data retval;

    // Find out how many bytes of header segments are in the file and check
    // whether the caller provided at least this amount of data.
    const auto required = check_header_size(retval, data, cnt);

    if (required + fixed_header_size > cnt) {
        throw std::invalid_argument("Insufficient header data have been "
            "provided to parse all header segments.");
    }

    return parse(retval,
        static_cast<const std::uint8_t *>(data) + fixed_header_size,
        required);
}


/*
 * FPD_NAMESPACE::meta_data<TChar>::fixed_header_size
 */
template<class TChar>
const std::size_t FPD_NAMESPACE::meta_data<TChar>::fixed_header_size;


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


/*
 * FPD_NAMESPACE::meta_data<TChar>::parse_bounding_box
 */
template<class TChar>
void FPD_NAMESPACE::meta_data<TChar>::parse_bounding_box(
        _Inout_ meta_data& meta_data,
        _In_ const void *data,
        _In_ const std::size_t cnt) {
    if (meta_data._bounding_box || !meta_data._frame_bounding_boxes.empty()) {
        throw std::invalid_argument("The bounding boxes have already been "
            "parsed.");
    }
    if (cnt < sizeof(format)) {
        throw std::invalid_argument("The bounding boxes must contain a format "
            "specification.")´;
    }

    // Determine the format of the bounding boxes and how big one is on disk.
    const auto fmt = static_cast<format>(
        swap_endian_as<std::underlying_type<format>::type>(data,
        meta_data._endian));
    const auto stride = 6 * to_size(fmt);

    if (cnt < sizeof(format) + stride) {
        throw std::invalid_argument("At least the global bounding box must be "
            "provided in the bounding box header segment.");
    }

    // Read the global bounding box, which must always be present if the header
    // segment exists.
    auto cur = static_cast<const std::uint8_t *>(data);
    const auto end = cur + cnt;
    cur += sizeof(format);
    meta_data._bounding_box = bounding_box(fmt, cur);
    cur += stride;

    // Read all optional per-frame bounding boxes.
    if (((cur - end) % stride) != 0) {
        throw std::invalid_argument("The bounding box header has an unexpected "
            "size.");
    }
    meta_data._frame_bounding_boxes.reserve((end - cur) / stride);
    while (cur < end) {
        meta_data._frame_bounding_boxes.emplace_back(fmt, cur);
        cur += stride;
    }
}


/*
 * FPD_NAMESPACE::meta_data<TChar>::parse_description
 */
template<class TChar>
void FPD_NAMESPACE::meta_data<TChar>::parse_description(
        _Inout_ meta_data& meta_data,
        _In_ const void *data,
        _In_ const std::size_t cnt) {
    if (!meta_data._description.empty()) {
        throw std::invalid_argument("The file description has already been "
            "parsed.");
    }
    if ((cnt % sizeof(char16_t)) != 0) {
        throw std::invalid_argument("The description has an unexpected size.");
    }

    meta_data._description = to_string(data, cnt);
}


/*
 * FPD_NAMESPACE::meta_data<TChar>::parse_format_description
 */
template<class TChar>
void FPD_NAMESPACE::meta_data<TChar>::parse_format_description(
        _Inout_ meta_data& meta_data,
        _In_ const void *data,
        _In_ const std::size_t cnt) {
    if (!meta_data._format_description) {
        throw std::invalid_argument("The format description has already been "
            "parsed.");
    }

    throw "TODO";
}


/*
 * FPD_NAMESPACE::meta_data<TChar>::parse_frame_table
 */
template<class TChar>
void FPD_NAMESPACE::meta_data<TChar>::parse_frame_table(
        _Inout_ meta_data& meta_data,
        _In_ const void *data,
        _In_ const std::size_t cnt) {
    if (!meta_data._frame_table.empty()) {
        throw std::invalid_argument("The frame table has already been parsed.");
    }
    if ((cnt % sizeof(frame_table_type::value_type)) != 0) {
        throw std::invalid_argument("The frame table has an unexpected size.");
    }

    auto b = reinterpret_cast<const frame_table_type::value_type *>(data);
    auto e = reinterpret_cast<const frame_table_type::value_type *>(data + cnt);
    meta_data._frame_table.reserve(e - b);
    std::copy(b, e, std::back_inserter(meta_data._frame_table));
}


/*
 * FPD_NAMESPACE::meta_data<TChar>::parse_global_particle_properties
 */
template<class TChar>
void FPD_NAMESPACE::meta_data<TChar>::parse_global_particle_properties(
        _Inout_ meta_data& meta_data,
        _In_ const void *data,
        _In_ const std::size_t cnt) {
#define _FPD_THROW_IF_INCOMPLETE(req) \
    if ((end - cur) < req) { \
        throw std::invalid_argument("The global particle property "\
        "header segment contains incomplete data."); \
    }

    if (!meta_data._global_particle_properties.empty()) {
        throw std::invalid_argument("The list of global particle properties "
            "has already been parsed.");
    }

    auto cur = static_cast<const std::uint8_t *>(data);
    const auto end = cur + cnt;

    while (cur < end) {
        _FPD_THROW_IF_INCOMPLETE(sizeof(semantic_type));
        const auto s = swap_endian_as<semantic_type>(cur, meta_data.endian);
        cur += sizeof(s);

        _FPD_THROW_IF_INCOMPLETE(sizeof(format_type));
        const auto f = swap_endian_as<format_type>(cur, meta_data.endian);
        cur += sizeof(f);

        const auto cnt = to_size(f);
        _FPD_THROW_IF_INCOMPLETE(cnt);
        meta_data._global_particle_properties[s] = format_variant(f, cur, cnt);
        cur += cnt;
    }

#undef _FPD_THROW_IF_INCOMPLETE
}


/*
 * FPD_NAMESPACE::meta_data<TChar>::parse_user_semantics
 */
template<class TChar>
void FPD_NAMESPACE::meta_data<TChar>::parse_user_semantics(
        _Inout_ meta_data& meta_data,
        _In_ const void *data,
        _In_ const std::size_t cnt) {
    if ((cnt % sizeof(char16_t)) != 0) {
        throw std::invalid_argument("The table of user-semantics has an "
            "unexpected size.");
    }

    auto cur = static_cast<const char16_t *>(data);
    const auto end = cur + (cnt / sizeof(char16_t));

    while (cur < end) {
        auto next = cur;
        while (*next++ != 0);
        meta_data._user_defined_semantics.push_back(
            to_string(meta_data, cur, next));
        cur = next;
    }
}


/*
 * FPD_NAMESPACE::meta_data<TChar>::to_string
 */
template<class TChar>
typename FPD_NAMESPACE::meta_data<TChar>::string_type
FPD_NAMESPACE::meta_data<TChar>::to_string(_In_ const meta_data& meta_data,
        _In_ const char16_t *begin, _In_ const char16_t *end) {
    assert(begin != nullptr);
    assert(end != nullptr);
    assert(end >= begin);
    std::basic_string<char16_t> string;
    string.resize(end - begin);

    // Swap the endian according to what the meta data say.
    swap_endian(string.data(), begin, string.size());

    // Convert the character type or the meta data.
    return convert_string<char_type>(string.data(), string.size(),
        code_page::oem);
}


/*
 * FPD_NAMESPACE::meta_data<TChar>::to_string
 */
template<class TChar>
typename FPD_NAMESPACE::meta_data<TChar>::string_type
FPD_NAMESPACE::meta_data<TChar>::to_string(_In_ const meta_data& meta_data,
        _In_reads_(cnt) const void *data, _In_ const std::size_t cnt) {
    assert(data != nullptr);
    auto end = static_cast<std::uint8_t>(data) + cnt;
    return to_string(static_cast<const char16_t *>(data),
        reinterpret_cast<const char16_t *>(end));
}
