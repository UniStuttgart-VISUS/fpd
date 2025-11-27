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
        _In_reads_bytes_(cnt) const void *data,
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

    // Preserve the overall size of the header for subsequent computation of
    // offsets into the file.
    output._header_size = retval + fixed_header_size;

    return retval;
}


/*
 * FPD_NAMESPACE::meta_data<TChar>::parse
 */
template<class TChar>
std::uint64_t FPD_NAMESPACE::meta_data<TChar>::parse(
        _Inout_ meta_data& meta_data,
        _In_reads_bytes_(cnt_header) const void *header,
        _In_ const std::size_t cnt_header,
        _In_reads_bytes_opt_(cnt_footer) const void *footer,
        _In_ const std::size_t cnt_footer) {
    assert(meta_data._endian != endian::undefined);
    assert((header != nullptr) || (cnt_header == 0));
    assert((footer != nullptr) || (cnt_footer == 0));
    std::array<std::pair<const std::uint8_t *, const std::uint8_t *>, 2> data {
        std::make_pair(static_cast<const std::uint8_t *>(header), nullptr),
        std::make_pair(static_cast<const std::uint8_t *>(footer), nullptr)
    };
    data[0].second = data[0].first + cnt_header;
    if (data[1].first != nullptr) {
        data[1].second = data[1].first + cnt_footer;
    }
    auto deferred_compression = std::make_pair(
        static_cast<const void *>(nullptr),
        static_cast<std::size_t>(0));
    auto deferred_format = std::make_pair(
        static_cast<const void *>(nullptr),
        static_cast<std::size_t>(0));
    std::uint64_t retval = 0;

    for (auto& d : data) {
        auto cur = d.first;

        while (cur + sizeof(header_segment) < d.second) {
            // Interpret the 32-bit header of the header segment.
            auto h = swap_endian_as<std::uint32_t>(cur, meta_data._endian);
            cur += sizeof(h);

            // Split the header_segments and the length of the segment.
            constexpr const decltype(h) header_segment_mask = (0xFFu << 24);
            const auto segment = header_segment_mask & h;
            const auto length = ~header_segment_mask & h;

            // Parse the segment as the recognised type or ignore it
            // allthogether.
            switch (static_cast<header_segment>(segment)) {
                case header_segment::frame_table:
                    parse_frame_table(meta_data, cur, length);
                    break;

                case header_segment::format_description:
                    try {
                        parse_format_description(meta_data, cur, length);
                    } catch (...) {
                        // The format will fail if there are user-defined
                        // semantics and we have not yet seen these. Therefore,
                        // we postpone parsing it again until the end.
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
                    try {
                        parse_compression(meta_data, cur, length);
                    } catch (...) {
                        // The compression might depend on the frame table
                        // already being processed. Therefore, we schedule a
                        // "second chance" in case this information might still
                        // come later on.
                        deferred_compression.first = cur;
                        deferred_compression.second = length;
                    }
                    break;

                case header_segment::footer:
                    retval = parse_footer(cur, length);
                    if ((retval > 0) && (cnt_footer == 0)) {
                        // If we have a footer, we can bail out immediately as
                        // it might not be possible to complete parsing the
                        // header segments without access to the footer as well.
                        return retval;
                    }
                    break;
            }

            cur += length;
        } /* while (cur + sizeof(header_segment) < d.second) */
    } /* for (auto& d : data) */

    // If we have postponed parsing the format, do it now. If it fails again,
    // the input is invalid.
    if (deferred_format.first != nullptr) {
        parse_format_description(meta_data,
            deferred_format.first,
            deferred_format.second);
    }

    // If we have postponed parsing the compression, do it now. If it fails
    // again, the input is invalid.
    if (deferred_compression.first != nullptr) {
        parse_compression(meta_data,
            deferred_compression.first,
            deferred_compression.second);
    }

    assert(retval == 0);
    return retval;
}


///*
// * FPD_NAMESPACE::meta_data meta_data<TChar>::parse
// */
//template<class TChar>
//FPD_NAMESPACE::meta_data<TChar> FPD_NAMESPACE::meta_data<TChar>::parse(
//        _In_reads_bytes_(cnt) const void *data, _In_ const std::size_t cnt) {
//    meta_data retval;
//
//    // Find out how many bytes of header segments are in the file and check
//    // whether the caller provided at least this amount of data.
//    const auto required = check_header_size(retval, data, cnt);
//    retval._header_size = required + fixed_header_size;
//
//    if (retval._header_size > cnt) {
//        throw std::invalid_argument("Insufficient header data have been "
//            "provided to parse all header segments.");
//    }
//
//    return parse(retval,
//        static_cast<const std::uint8_t *>(data) + fixed_header_size,
//        retval._header_size);
//}


/*
 * FPD_NAMESPACE::meta_data<TChar>::bounding_box
 */
template<class TChar>
const typename FPD_NAMESPACE::meta_data<TChar>::bounding_box_type&
FPD_NAMESPACE::meta_data<TChar>::bounding_box(
        _In_ std::size_t frame) const noexcept {
    assert(!this->_bounding_boxes.empty());
    if (frame + 1 > this->_bounding_boxes.size()) {
        // Fall back to global bounding box.
        frame = 0;
    }

    return this->_bounding_boxes[frame];
}


/*
 * FPD_NAMESPACE::meta_data<TChar>::compression_as
 */
template<class TChar>
template<FPD_NAMESPACE::compression Compression>
const FPD_NAMESPACE::compression_meta_data<Compression>&
FPD_NAMESPACE::meta_data<TChar>::compression_as(void) const {
    assert(this->_compression);
    typedef compression_meta_data<Compression> return_type;
    auto retval = std::dynamic_pointer_cast<return_type>(this->_compression);

    if (retval == nullptr) {
        throw std::runtime_error("The actual compression method does not match "
            "the expected one.");
    }

    return *retval;
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
        _In_ const std::size_t frame,
        _In_ const std::size_t group,
        _In_ const bool decompressed) const {
    assert(this->_particle_size > 0);
    const auto calc_size = [this](const std::size_t f,
            const std::size_t g,
            const bool d,
            const std::size_t p) {
        if (d) {
            // The decompressed size can be computed from the particle size and
            // the number of particles in the frame.
            return this->particles(f) * p;
        }

        switch (this->compression().method()) {
            case compression::none:
                // This is equivalent to decompressed data.
                return this->particles(f) * p;

            case compression::gdeflate:
                return this->frame_size(
                    this->compression_as<compression::gdeflate>(),
                    f,
                    g);

            default:
                throw std::logic_error("The specified compression method is "
                    "not supported.");
        }
    };

    const auto groups = this->_format_description.count_groups();

    if ((group >= groups) || (groups == 1)) {
        // The size of the whole frame was requested.
        return calc_size(frame, group, decompressed, this->_particle_size);

    } else {
        // Figure out how big the part of a particle in the specified element
        // group is.
        const auto particle_size = this->_format_description[group].size();
        return calc_size(frame, group, decompressed, particle_size);
    }
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
 * FPD_NAMESPACE::meta_data<TChar>::hierarchy_as
 */
template<class TChar>
template<FPD_NAMESPACE::spatial_hierarchy Hierarchy>
const FPD_NAMESPACE::hierarchy_meta_data<Hierarchy>&
FPD_NAMESPACE::meta_data<TChar>::hierarchy_as(void) const {
    assert(this->_hierarchy);
    typedef hierarchy_meta_data<Hierarchy> return_type;
    auto retval = std::dynamic_pointer_cast<return_type>(this->_hierarchy);

    if (retval == nullptr) {
        throw std::runtime_error("The actual type of the spatial hierarchy "
            "does not match the expected one.");
    }

    return *retval;
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
        const auto size = static_cast<segment_size>(this->_frame_table.size()
            * sizeof(frame_table_type::value_type));
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
            static_cast<int>(this->_description.size()));
        const auto size = static_cast<segment_size>(data.size());
        retval += write(file, header_segment::description, size, this->_endian);
        retval += detail::write_all(file, data.data(), size);
    }

    // Write the (optional) bounding boxes.
    if (!this->_bounding_boxes.empty() && (this->_bounding_boxes.front())) {
        assert((this->_bounding_boxes.size() == 1)
            || (this->_bounding_boxes.size() == this->_frame_table.size() + 1));
        const auto bbox_format = this->_bounding_boxes.front().format();
        const auto bbox_size = this->_bounding_boxes.front().size();
        const auto size = static_cast<segment_size>(sizeof(format)
            + this->_bounding_boxes.size() * bbox_size);
        retval += write(file, header_segment::bounding_box, size,
            this->_endian);
        retval += write(file, bbox_format, this->_endian);

        for (auto& b : this->_bounding_boxes) {
            assert(b);
            assert(b.format() == bbox_format);
            retval += write(file, b, this->_endian);
        }
    } /* if (this->_bounding_box) */

    // Write the spatial hierarchy meta data.
    if (this->_hierarchy != nullptr) {
        retval += meta_data::write<spatial_hierarchy, hierarchy_meta_data>(
            file,
            this->_hierarchy->type(),
            this->_hierarchy,
            this->_endian,
            detail::spatial_hierarchies());
    }

    // Write the compression meta data.
    if (this->_compression != nullptr) {
        retval += write<FPD_NAMESPACE::compression, compression_meta_data>(
            file,
            this->_compression->method(),
            this->_compression,
            this->_endian,
            detail::compressions());
    }

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
    assert(meta_data._frame_offsets.empty());
    assert(meta_data._header_size > 0);
    assert(meta_data._particle_size > 0);
    assert(meta_data._compression->method() == compression::none);

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
        _In_reads_bytes_(cnt) const void *data,
        _In_ const std::size_t cnt) {
    if (cnt < sizeof(format)) {
        throw std::invalid_argument("The bounding boxes must contain a format "
            "specification.");
    }

    auto cur = static_cast<const std::uint8_t *>(data);
    const auto end = cur + cnt;

    // Determine the format of the bounding boxes and how big one is on disk.
    const auto fmt = static_cast<format>(
        swap_endian_as<std::underlying_type<format>::type>(cur,
        meta_data._endian));
    cur += sizeof(format);
    const auto stride = 6 * to_size(fmt);

    if (cnt < sizeof(format) + stride) {
        throw std::invalid_argument("At least the global bounding box must be "
            "provided in the bounding box header segment.");
    }

    // Read the bounding boxes per-frame bounding boxes.
    if (((end - cur) % stride) != 0) {
        throw std::invalid_argument("The bounding box header has an unexpected "
            "size. The number of elements in the header must be divisble by "
            "the size of a bounding box.");
    }
    meta_data._bounding_boxes.clear();
    meta_data._bounding_boxes.reserve((end - cur) / stride);
    while (cur < end) {
        const auto rem = end - cur;
        meta_data._bounding_boxes.emplace_back(fmt, cur, rem,
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
        _In_reads_bytes_(cnt) const void *data,
        _In_ const std::size_t cnt) {
    typedef FPD_NAMESPACE::compression enum_type;
    typedef compression_meta_data<enum_type::gdeflate> gdeflate_type;

    auto cur = static_cast<const std::uint8_t *>(data);
    const auto end = cur + cnt;

    if ((end - cur) < sizeof(enum_type)) {
        throw std::invalid_argument("The compression header segment contains "
            "incomplete data.");
    }
    const auto type = swap_enum_endian<enum_type>(cur, meta_data._endian);
    cur += sizeof(type);

    switch (type) {
        case enum_type::none:
            break;

        case enum_type::gdeflate: {
            // GDeflate has an uint32 table of block sizes that tell us how
            // many bytes we should decompress at once. Each frame may comprise
            // one or more blocks, but all frames must be split into the same
            // number of blocks.
            if ((end - cur) % sizeof(std::uint32_t) != 0) {
                throw std::invalid_argument("The size of the GDeflate "
                    "compressed blocks is invalid.");
            }

            auto md = std::make_shared<gdeflate_type>();

            std::copy(reinterpret_cast<const std::uint32_t *>(cur),
                reinterpret_cast<const std::uint32_t *>(end),
                std::back_inserter(md->_blocks));
            if (md->_blocks.size() % meta_data.frames() != 0) {
                throw std::invalid_argument("GDeflate compression requires "
                    "that all frames are split into an equal number of "
                    "blocks.");
            }

            swap_endian(md->_blocks.data(), md->_blocks.size(),
                meta_data._endian);
            meta_data._compression = md;

            // At this point, we can compute the frame offsets.
            assert(meta_data._header_size > fixed_header_size);
            meta_data._frame_offsets.clear();
            meta_data._frame_offsets.reserve(meta_data._frame_table.size());
            md->calc_frame_offsets(std::back_inserter(meta_data._frame_offsets),
                meta_data.frames(), meta_data._header_size);
            } break;

        default:
            throw std::invalid_argument("The particle data are compressed "
                "using an unsupported method.");
    }
}


/*
 * FPD_NAMESPACE::meta_data<TChar>::parse_description
 */
template<class TChar>
void FPD_NAMESPACE::meta_data<TChar>::parse_description(
        _Inout_ meta_data& meta_data,
        _In_reads_bytes_(cnt) const void *data,
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
        _In_reads_bytes_(cnt) const void *data,
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
    // particles such that we can seek directly later on. Note that compression
    // methods result in frame sizes that are not computable from the number of
    // particles. Therefore, these must have been handled while parsing the
    // compression header.
    if (meta_data._frame_offsets.empty()) {
        calc_frame_offsets(meta_data);
    }
}


/*
 * FPD_NAMESPACE::meta_data<TChar>::parse_footer
 */
template<class TChar>
std::uint64_t FPD_NAMESPACE::meta_data<TChar>::parse_footer(
        _In_reads_bytes_(cnt) const void *data,
        _In_ const std::size_t cnt) {
    if (cnt != sizeof(std::uint64_t)) {
        throw std::invalid_argument("The footer sigment has an unexpected "
            "size.");
    }

    return *static_cast<const std::uint64_t *>(data);
}


/*
 * FPD_NAMESPACE::meta_data<TChar>::parse_frame_table
 */
template<class TChar>
void FPD_NAMESPACE::meta_data<TChar>::parse_frame_table(
        _Inout_ meta_data& meta_data,
        _In_reads_bytes_(cnt) const void *data,
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
        _In_reads_bytes_(cnt) const void *data,
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

        const auto l = to_size(f);
        _FPD_THROW_IF_INCOMPLETE(l);
        meta_data._global_particle_properties[s] = format_variant(
            f, cur, l, meta_data.endian());
        cur += l;
    }

#undef _FPD_THROW_IF_INCOMPLETE
}


/*
 * FPD_NAMESPACE::meta_data<TChar>::parse_spatial_hierarchy
 */
template<class TChar>
void FPD_NAMESPACE::meta_data<TChar>::parse_spatial_hierarchy(
        _Inout_ meta_data& meta_data,
        _In_reads_bytes_(cnt) const void *data,
        _In_ const std::size_t cnt) {
#define _FPD_THROW_IF_INCOMPLETE(req) \
    if ((end - cur) < req) { \
        throw std::invalid_argument("The spatial hierarchy header segment "\
            "contains incomplete data."); \
    }

    typedef FPD_NAMESPACE::spatial_hierarchy enum_type;
    typedef hierarchy_meta_data<enum_type::ospray_pkd_tree> ospray_type;

    auto cur = static_cast<const std::uint8_t *>(data);
    const auto end = cur + cnt;

    _FPD_THROW_IF_INCOMPLETE(sizeof(enum_type));
    const auto type = swap_enum_endian<enum_type>(cur, meta_data._endian);
    cur += sizeof(type);

    switch (type) {
        case enum_type::none:
            break;

        case enum_type::ospray_pkd_tree: {
            _FPD_THROW_IF_INCOMPLETE(sizeof(std::uint8_t));
            auto md = std::make_shared<ospray_type>();
            assert(sizeof(std::uint8_t) == sizeof(md->_axis_component));
            md->_axis_component = *cur++;
            meta_data._hierarchy = md;
            } break;

        default:
            throw std::invalid_argument("The particle data are spatially "
                "organised in an unsupported way.");
    }
}


/*
 * FPD_NAMESPACE::meta_data<TChar>::parse_user_semantics
 */
template<class TChar>
void FPD_NAMESPACE::meta_data<TChar>::parse_user_semantics(
        _Inout_ meta_data& meta_data,
        _In_reads_bytes_(cnt) const void *data,
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
 * FPD_NAMESPACE::meta_data<TChar>::size_on_disk
 */
template<class TChar>
template<class TEnum, template<TEnum> class TDerived, class TBase, TEnum... Es>
std::size_t FPD_NAMESPACE::meta_data<TChar>::size_on_disk(
        _In_ const TEnum type,
        _In_ const TBase& meta_data,
        _In_ const detail::enum_dispatch_list<TEnum, Es...>) noexcept {
    typedef std::function<std::size_t(const TBase&)> dispatcher;
    typedef detail::enum_dispatch_list<TEnum, Es...> dispatch_list;

    // Note: this can be static, because the lambda has no capture.
    static const std::array<dispatcher, sizeof...(Es)> Dispatchers = {
        ([](const TBase& md) {
            typedef TDerived<Es> derived;
            auto m = std::dynamic_pointer_cast<derived>(md);
            assert(m != nullptr);
            return size_on_disk(*m);
        })...
    };

    const auto idx = index_of(type, dispatch_list(), 0);
    if (idx < Dispatchers.size()) {
        assert(meta_data != nullptr);
        return Dispatchers[idx](meta_data);
    }

    return 0;
}


/*
 *  FPD_NAMESPACE::meta_data<TChar>::size_on_disk
 */
template<class TChar>
std::size_t FPD_NAMESPACE::meta_data<TChar>::size_on_disk(
        _In_ const compression_type& compression) noexcept {
    if (compression == nullptr) {
        return 0;
    }

    return size_on_disk<FPD_NAMESPACE::compression, compression_meta_data>(
        compression->method(), compression, detail::compressions());
}


/*
 *  FPD_NAMESPACE::meta_data<TChar>::size_on_disk
 */
template<class TChar>
std::size_t FPD_NAMESPACE::meta_data<TChar>::size_on_disk(
        _In_ const hierarchy_type& hierarchy) noexcept {
    if (hierarchy == nullptr) {
        return 0;
    }

    return size_on_disk<spatial_hierarchy, hierarchy_meta_data>(
        hierarchy->type(), hierarchy, detail::spatial_hierarchies());
}


/*
 * FPD_NAMESPACE::meta_data<TChar>::write
 */
template<class TChar>
template<class TFile, class TByte>
std::size_t FPD_NAMESPACE::meta_data<TChar>::write(
        _In_ TFile& file,
        _In_ const basic_bounding_box<TByte>& bbox,
        _In_ const endian_type endian) {
    assert(bbox);

    if (requires_conversion(endian)) {
        std::vector<TByte> buffer(bbox.begin(), bbox.end());
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
        _In_ const endian_type endian) {
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
        _In_ const endian_type endian) {
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
template<class TEnum, template<TEnum> class TDerived, class TFile,
        class TBase, TEnum... Es>
std::size_t FPD_NAMESPACE::meta_data<TChar>::write(_In_ TFile& file,
        _In_ const TEnum type,
        _In_ const TBase& meta_data,
        _In_ const endian_type endian,
        _In_ const detail::enum_dispatch_list<TEnum, Es...>) {
    typedef std::function<std::size_t(TFile&, const TBase&,
        const endian_type)> dispatcher;
    typedef detail::enum_dispatch_list<TEnum, Es...> dispatch_list;

    // Note: this can be static, because the lambda has no capture.
    static const std::array<dispatcher, sizeof...(Es)> Dispatchers = {
        ([](TFile& f, const TBase& md, const endian_type e) {
            typedef TDerived<Es> derived;
            auto m = std::dynamic_pointer_cast<derived>(md);
            assert(m != nullptr);
            return meta_data::write(f, *m, e);
        })...
    };

    const auto idx = index_of(type, dispatch_list(), 0);
    if (idx < Dispatchers.size()) {
        assert(meta_data != nullptr);
        return Dispatchers[idx](file, meta_data, endian);
    }

    return 0;
}


/*
 * FPD_NAMESPACE::meta_data<TChar>::write
 */
template<class TChar>
template<class TFile, class TValue>
std::size_t FPD_NAMESPACE::meta_data<TChar>::write(_In_ TFile& file,
        _In_ const std::vector<TValue>& values,
        _In_ const endian_type endian) {
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
 * FPD_NAMESPACE::meta_data<TChar>::frame_size
 */
template<class TChar>
template<FPD_NAMESPACE::compression C, class S>
typename FPD_NAMESPACE::meta_data<TChar>::size_type
FPD_NAMESPACE::meta_data<TChar>::frame_size(
        _In_ const detail::uniform_block_compression_meta_data<C, S>& md,
        _In_ const std::size_t frame,
        _In_ const std::size_t group) const {
    const auto groups = this->_format_description.count_groups();
    const auto blocks = md.check(this->frames(), groups);
    auto begin = frame * blocks;
    auto end = (frame + 1) * blocks;

    if ((group < groups) && (groups > 1)) {
        // Only collect the blocks that correspond to 'group'.
        assert(blocks % groups == 0);
        const auto blocks_per_group = blocks / groups;
        begin += group * blocks_per_group;
        end = begin + (group + 1) * blocks_per_group;
    }

    assert(begin < md.blocks());
    assert(end <= md.blocks());
    size_type retval = 0;
    for (std::size_t b = begin; b < end; ++b) {
        retval += md[b];
    }

    return retval;
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
