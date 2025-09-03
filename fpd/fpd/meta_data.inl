// <copyright file="meta_data.inl" company="Visualisierungsinstitut der Universität Stuttgart">
// Copyright © 2023 - 2025 Visualisierungsinstitut der Universität Stuttgart.
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
    const auto retval = swap_endian_as<std::uint32_t>(bytes, output._endian);

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
    assert(meta_data._endian != endian::undefined);
    assert((data != nullptr) || (cnt == 0));
    auto cur = static_cast<const std::uint8_t * >(data);
    const auto end = cur + cnt;
    auto deferred_format = std::make_pair(static_cast<const void *>(nullptr),
        static_cast<std::size_t>(0));

    while (cur + sizeof(header_segment) < end) {
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

            case header_segment::global_particle_properties:
                parse_global_particle_properties(meta_data, cur, length);
                break;

            case header_segment::spatial_hierarchy:
                parse_spatial_hierarchy(meta_data, cur, length);
                break;

            case header_segment::compression:
                parse_compression(meta_data, cur, length);
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
    retval._header_size = required + fixed_header_size;

    if (retval._header_size > cnt) {
        throw std::invalid_argument("Insufficient header data have been "
            "provided to parse all header segments.");
    }

    return parse(retval,
        static_cast<const std::uint8_t *>(data) + fixed_header_size,
        retval._header_size);
}


/*
 * FPD_NAMESPACE::meta_data<TChar>::frame_offset
 */
template<class TChar>
typename FPD_NAMESPACE::meta_data<TChar>::size_type
FPD_NAMESPACE::meta_data<TChar>::frame_offset(
        _In_ const std::size_t frame) const {
    assert(this->_frame_offsets.size() == this->_frame_table.size());
    if (frame >= this->_frame_offsets.size()) {
        throw std::range_error("The specified frame is out of range.");
    }
    return this->_frame_offsets[frame];
}


/*
 * FPD_NAMESPACE::meta_data<TChar>::frame_size
 */
template<class TChar>
typename FPD_NAMESPACE::meta_data<TChar>::size_type
FPD_NAMESPACE::meta_data<TChar>::frame_size(
        _In_ const std::size_t frame) const {
    assert(this->_particle_size > 0);
    return this->particles(frame) * this->_particle_size;
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
 * FPD_NAMESPACE::meta_data<TChar>::particles
 */
template<class TChar>
typename FPD_NAMESPACE::meta_data<TChar>::size_type
FPD_NAMESPACE::meta_data<TChar>::particles(_In_ const std::size_t frame) const {
    assert(this->_frame_offsets.size() == this->_frame_table.size());
    if (frame >= this->_frame_table.size()) {
        throw std::range_error("The specified frame is out of range.");
    }
    return this->_frame_table[frame];
}


/*
 * FPD_NAMESPACE::meta_data<TChar>::save
 */
template<class TChar>
template<class TFile>
std::size_t FPD_NAMESPACE::meta_data<TChar>::save(_In_ TFile& file) const {
    typedef std::underlying_type_t<header_segment> segment_size;

    // Write the fixed-size part of the header.
    auto retval = detail::write_all(file, "FPD", 3);

    retval += detail::write_all(file, this->_version.first);
    retval += detail::write_all(file, this->_version.second);

    retval += detail::write_all(file, this->_endian);

    assert(this->_header_size > fixed_header_size);
    {
        const auto header_size = swap_endian(
            static_cast<std::uint32_t>(this->_header_size - fixed_header_size),
            this->_endian);
        retval += detail::write_all(file, header_size);
    }

    // Write the frame table (# of particles only).
    {
        const auto size = this->_frame_table.size()
            * sizeof(frame_table_type::value_type);
        retval += write(file, header_segment::frame_table, size,
            this->_endian);
        retval += write(file, this->_frame_table, this->_endian);
    }

    // Write the user-defined semantics. These are optional, but we write them
    // before the format description in order to allow the reader to process
    // everything in one pass without deferring parsing the format to after the
    // user-defined semantics have been read.
    if (!this->_user_defined_semantics.empty()) {
        const auto data = this->to_utf8(this->_user_defined_semantics);
        const auto size = static_cast<segment_size>(data.size());
        retval += write(file, header_segment::user_semantics, size,
            this->_endian);
        retval += detail::write_all(file, data.data(), size);
    }

    // Write the format description.
    {
        const auto size = static_cast<segment_size>(element_type::size_on_disk
            * this->_format_description.count_elements());
        retval += write(file, header_segment::format_description, size,
            this->_endian);

        assert(this->_format_description.count_groups()
            < (std::numeric_limits<std::uint16_t>::max)());
        const auto groups = static_cast<std::uint16_t>(
            this->_format_description.count_groups());
        for (std::uint16_t g = 0; g < groups; ++g) {
            for (auto& e : this->_format_description[g]) {
                retval += write(file, e, g, this->_endian);
            }
        }
    }

    // Write the optional file description. Note that the description is a
    // single string, so it does not require the terminating null as per the
    // specification.
    if (!this->_description.empty()) {
        const auto data = this->to_utf8(this->_description.c_str(),
            this->_description.size());
        const auto size = static_cast<segment_size>(data.size());
        retval += write(file, header_segment::description, size, this->_endian);
        retval += detail::write_all(file, data.data(), size);
    }

    // Write the (optional) bounding boxes.
    if (this->_bounding_box) {
        assert((this->_frame_bounding_boxes.size() == this->_frame_table.size())
            || (this->_frame_bounding_boxes.size() == 0));
        const auto size = static_cast<segment_size>(sizeof(format)
            + (1 + this->_frame_bounding_boxes.size())
            * this->_bounding_box.size());
        retval += write(file, header_segment::bounding_box, size,
            this->_endian);
        retval += write(file, this->_bounding_box.format(), this->_endian);
        retval += write(file, this->_bounding_box, this->_endian);

        for (auto& b : this->_frame_bounding_boxes) {
            assert(b.format() == this->_bounding_box.format());
            retval += write(file, b, this->_endian);
        }
    } /* if (this->_bounding_box) */

    // Write the spatial hierarchy meta data.
    // TODO

    // Write the compression meta data.
    // TODO

    assert(retval == this->_header_size);
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
 * FPD_NAMESPACE::meta_data<TChar>::calc_frame_offsets
 */
template<class TChar>
void FPD_NAMESPACE::meta_data<TChar>::calc_frame_offsets(
        _Inout_ meta_data& meta_data) {
    assert(meta_data._header_size > 0);
    assert(meta_data._particle_size > 0);

    auto offset = meta_data._header_size;
    meta_data._frame_offsets.resize(meta_data._frame_table.size());

    for (std::size_t i = 0; i < meta_data._frame_table.size(); ++i) {
        meta_data._frame_offsets[i] = offset;
        offset += meta_data._frame_table[i] * meta_data._particle_size;
    }
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
            "specification.");
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
    meta_data._bounding_box = bounding_box(fmt, cur, end - cur);
    cur += stride;

    // Read all optional per-frame bounding boxes.
    if (((cur - end) % stride) != 0) {
        throw std::invalid_argument("The bounding box header has an unexpected "
            "size.");
    }
    meta_data._frame_bounding_boxes.reserve((end - cur) / stride);
    while (cur < end) {
        meta_data._frame_bounding_boxes.emplace_back(fmt, cur, end - cur,
            meta_data._endian);
        cur += stride;
    }
}


/*
 * FPD_NAMESPACE::meta_data<TChar>::parse_compression
 */
template<class TChar>
void FPD_NAMESPACE::meta_data<TChar>::parse_compression(
        _Inout_ meta_data& meta_data,
        _In_ const void *data,
        _In_ const std::size_t cnt) {
    throw "TODO";
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
    meta_data._description = meta_data.to_string(data, cnt);
}


/*
 * FPD_NAMESPACE::meta_data<TChar>::parse_format_description
 */
template<class TChar>
void FPD_NAMESPACE::meta_data<TChar>::parse_format_description(
        _Inout_ meta_data& meta_data,
        _In_ const void *data,
        _In_ const std::size_t cnt) {
#define _FPD_THROW_IF_INCOMPLETE(req) \
    if ((end - cur) < req) { \
        throw std::invalid_argument("The format description header "\
            "segment contains incomplete data."); \
    }

    if (meta_data._format_description) {
        throw std::invalid_argument("The format description has already been "
            "parsed.");
    }
    if ((cnt % format_description_type::element_type::size_on_disk) != 0) {
        throw std::invalid_argument("The format description has an unexpected "
            "size.");
    }

    auto cur = static_cast<const std::uint8_t *>(data);
    const auto end = cur + cnt;

    while (cur < end) {
        _FPD_THROW_IF_INCOMPLETE(sizeof(semantic));
        const auto s = swap_enum_endian<semantic>(cur, meta_data._endian);
        cur += sizeof(s);

        _FPD_THROW_IF_INCOMPLETE(sizeof(format));
        const auto f = swap_enum_endian<format>(cur, meta_data._endian);
        cur += sizeof(f);

        _FPD_THROW_IF_INCOMPLETE(sizeof(std::uint16_t));
        const auto g = swap_endian_as<std::uint16_t>(cur, meta_data._endian);
        cur += sizeof(g);

        _FPD_THROW_IF_INCOMPLETE(sizeof(std::uint16_t));
        const auto o = swap_endian_as<std::uint16_t>(cur, meta_data._endian);
        cur += sizeof(o);

        _FPD_THROW_IF_INCOMPLETE(sizeof(std::uint16_t));
        const auto b = swap_endian_as<std::uint16_t>(cur, meta_data._endian);
        cur += sizeof(b);

        if (s < semantic::user_defined) {
            meta_data._format_description.add(s, f, g, o, b, true);

        } else {
            // If this is a user-defined semantic, find the description in the
            // table of user-defined semantics provided with the meta data
            // object. If that fails, we raise an exception and the caller must
            // parse the meta data again once the table has been read. In order
            // for this to succeed, any existing data must be cleared.
            auto i = static_cast<std::size_t>(s);
            i -= static_cast<std::size_t>(semantic::user_defined);

            if (i >= meta_data._user_defined_semantics.size()) {
                meta_data._format_description = format_description_type();
                throw std::logic_error("The specified user-defined semantic "
                    "has no valid description.");
            }

            auto d = meta_data._user_defined_semantics[i].c_str();

            meta_data._format_description.add(s, d, f, g, o, b, true);
        }
    } /* while (cur < end) */
#undef _FPD_THROW_IF_INCOMPLETE

    // Cache the size of a single particle, because we need that a lot.
    meta_data._particle_size = meta_data._format_description.size();

    // We now pre-compute the frame offsets in the file from the number of
    // particles such that we can seek directly later on.
    calc_frame_offsets(meta_data);
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
    auto e = reinterpret_cast<const frame_table_type::value_type *>(
        static_cast<const std::uint8_t *>(data) + cnt);
    meta_data._frame_table.reserve(e - b);
    std::copy(b, e, std::back_inserter(meta_data._frame_table));
    swap_endian(meta_data._frame_table.data(),
        meta_data._frame_table.size(),
        meta_data._endian);
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
    if (static_cast<std::size_t>(end - cur) < req) { \
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
        const auto s = swap_enum_endian<semantic_type>(cur, meta_data.endian());
        cur += sizeof(s);

        _FPD_THROW_IF_INCOMPLETE(sizeof(format_type));
        const auto f = swap_enum_endian<format_type>(cur, meta_data.endian());
        cur += sizeof(f);

        const auto cnt = to_size(f);
        _FPD_THROW_IF_INCOMPLETE(cnt);
        meta_data._global_particle_properties[s] = format_variant(
            f, cur, cnt, meta_data.endian());
        cur += cnt;
    }

#undef _FPD_THROW_IF_INCOMPLETE
}


/*
 * FPD_NAMESPACE::meta_data<TChar>::parse_spatial_hierarchy
 */
template<class TChar>
void FPD_NAMESPACE::meta_data<TChar>::parse_spatial_hierarchy(
        _Inout_ meta_data& meta_data,
        _In_ const void *data,
        _In_ const std::size_t cnt) {
    throw "TODO";
}


/*
 * FPD_NAMESPACE::meta_data<TChar>::parse_user_semantics
 */
template<class TChar>
void FPD_NAMESPACE::meta_data<TChar>::parse_user_semantics(
        _Inout_ meta_data& meta_data,
        _In_ const void *data,
        _In_ const std::size_t cnt) {
    auto cur = static_cast<const char *>(data);
    const auto end = cur + cnt;

    while (cur < end) {
        auto next = cur;
        while (*next++ != 0);
        auto str = meta_data.to_string(cur, next);
        meta_data._user_defined_semantics.push_back(std::move(str));
        cur = next;
    }
}


/*
 * FPD_NAMESPACE::meta_data<TChar>::write
 */
template<class TChar>
template<class TFile>
std::size_t FPD_NAMESPACE::meta_data<TChar>::write(
        _In_ TFile& file,
        _In_ const bounding_box& bbox,
        _In_ const FPD_NAMESPACE::endian endian) {
    assert(bbox);

    if (requires_conversion(endian)) {
        std::vector<std::uint8_t> buffer(bbox.begin(), bbox.end());
        const auto stride = to_size(bbox.format());
        assert(buffer.size() % stride == 0);
        const auto cnt = buffer.size() / stride;
        swap_bytes(buffer.data(), cnt, stride);
        return detail::write_all(file, buffer.data(), buffer.size());

    } else {
        // System endian requires no further processing.
        return detail::write_all(file, bbox.data(), bbox.size());
    }
}


/*
 * FPD_NAMESPACE::meta_data<TChar>::write
 */
template<class TChar>
template<class TFile>
std::size_t FPD_NAMESPACE::meta_data<TChar>::write(_In_ TFile& file,
        _In_ const element_type& element,
        _In_ std::uint16_t group,
        _In_ const FPD_NAMESPACE::endian endian) {
    std::size_t retval = 0;

    auto semantic = swap_enum_endian(element.semantic(), endian);
    retval += detail::write_all(file, semantic);

    auto format = swap_enum_endian(element.format(), endian);
    retval += detail::write_all(file, format);

    group = swap_endian(group, endian);
    retval += detail::write_all(file, group);

    auto offset = swap_endian(element.offset(), endian);
    retval += detail::write_all(file, offset);

    auto size = swap_endian(element.size(), endian);
    retval += detail::write_all(file, size);

    return retval;
}


/*
 * FPD_NAMESPACE::meta_data<TChar>::write
 */
template<class TChar>
template<class TFile>
std::size_t FPD_NAMESPACE::meta_data<TChar>::write(
        _In_ TFile& file,
        _In_ const header_segment segment,
        _In_ const std::underlying_type_t<header_segment> size,
        _In_ const FPD_NAMESPACE::endian endian) {
    constexpr const decltype(size) size_mask = 0x00FFFFFF;
    assert(size < size_mask);
    auto value = static_cast<decltype(size)>(segment);
    value |= (size & size_mask);
    value = swap_endian(value, endian);
    return detail::write_all(file, value);
}


/*
 * FPD_NAMESPACE::meta_data<TChar>::write
 */
template<class TChar>
template<class TFile, class TValue>
std::size_t FPD_NAMESPACE::meta_data<TChar>::write(_In_ TFile& file,
        _In_ const std::vector<TValue>& values,
        _In_ const FPD_NAMESPACE::endian endian) {
    static_assert(std::is_arithmetic_v<TValue>, "This method is only intended "
        "for arrays of numbers.");
    if (requires_conversion(endian)) {
        auto buffer = values;
        swap_bytes(buffer.data(), buffer.size());
        return detail::write_all(file,
            buffer.data(),
            buffer.size() * sizeof(TValue));

    } else {
        return detail::write_all(file,
            values.data(),
            values.size() * sizeof(TValue));
    }
}


/*
 * FPD_NAMESPACE::meta_data<TChar>::to_string
 */
template<class TChar>
typename FPD_NAMESPACE::meta_data<TChar>::string_type
FPD_NAMESPACE::meta_data<TChar>::to_string(_In_ const char *begin,
        _In_ const char *end) const {
    assert(begin != nullptr);
    assert(end != nullptr);
    assert(end >= begin);
    constexpr auto zero = static_cast<TChar>(0);
    const auto cnt_src = static_cast<int>(end - begin);

    // Measure how many *bytes* we need to store the converted string.
    auto dst = static_cast<std::uint8_t *>(nullptr);
    auto cnt_dst = convert_string(
        dst, 0, this->code_page(),
        begin, cnt_src, encodings::utf8);

    // Allocate the output string.
    assert(cnt_dst % sizeof(TChar) == 0);
    string_type retval(cnt_dst / sizeof(TChar), zero);
    dst = reinterpret_cast<std::uint8_t *>(std::addressof(retval[0]));
    convert_string(
        dst, cnt_dst, this->code_page(),
        begin, cnt_src, encodings::utf8);

    // Erase all bogus zeros from the output. This might be the case, because
    // the specification allows for optionally null-terminated strings at some
    // places.
    retval.erase(std::remove(retval.begin(), retval.end(), zero),
        retval.end());

    return retval;
}


/*
 * FPD_NAMESPACE::meta_data<TChar>::to_string
 */
template<class TChar>
typename FPD_NAMESPACE::meta_data<TChar>::string_type
FPD_NAMESPACE::meta_data<TChar>::to_string(_In_reads_(cnt) const void *data,
        _In_ const std::size_t cnt) const {
    assert(data != nullptr);
    auto beg = static_cast<const char *>(data);
    auto end = beg + cnt;
    return this->to_string(beg, end);
}


/*
 * FPD_NAMESPACE::meta_data<TChar>::to_utf8
 */
template<class TChar>
std::string FPD_NAMESPACE::meta_data<TChar>::to_utf8(
        _In_reads_or_z_opt_(cnt) const char_type *str,
        _In_ const int cnt) const {
    constexpr auto zero = static_cast<char>(0);
    std::string retval(convert_string(
        static_cast<char *>(nullptr), 0, encodings::utf8,
        str, cnt, this->code_page()), zero);
    convert_string(std::addressof(retval[0]), retval.size(), encodings::utf8,
        str, cnt, this->code_page());
    retval.erase(std::remove(retval.begin(), retval.end(), zero), retval.end());
    return retval;
}


/*
 * FPD_NAMESPACE::meta_data<TChar>::to_utf8
 */
template<class TChar>
std::vector<char> FPD_NAMESPACE::meta_data<TChar>::to_utf8(
        _In_ const std::vector<string_type>& strings) const {
    std::vector<char> retval;

    for (const auto& s : strings) {
        const auto offset = retval.size();
        const auto cnt = convert_string(
            static_cast<char *>(nullptr), 0, encodings::utf8,
            s.c_str(), -1, this->code_page());
        retval.resize(offset + cnt);
        convert_string(retval.data() + offset, cnt, encodings::utf8,
            s.c_str(), -1, this->code_page());
        assert(retval.back() == static_cast<char>(0));
    }

    return retval;
}
