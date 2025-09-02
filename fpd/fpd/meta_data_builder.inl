// <copyright file="meta_data_builder.inl" company="Visualisierungsinstitut der Universität Stuttgart">
// Copyright © 2023 - 2025 Visualisierungsinstitut der Universität Stuttgart.
// Licensed under the MIT licence. See LICENCE file for details.
// </copyright>
// <author>Christoph Müller</author>


/*
 * FPD_NAMESPACE::meta_data_builder<TChar>::meta_data_builder
 */
template<class TChar>
FPD_NAMESPACE::meta_data_builder<TChar>::meta_data_builder(void)
    : with_code_page<TChar>(encodings::oem),
      _version(std::make_pair(FPD_FILE_MAJOR, FPD_FILE_MINOR)) { }


/*
 * FPD_NAMESPACE::meta_data meta_data_builder<TChar>::build
 */
template<class TChar>
typename FPD_NAMESPACE::meta_data_builder<TChar>::meta_data_type
FPD_NAMESPACE::meta_data_builder<TChar>::build(void) {
    if (!*this) {
        throw std::logic_error("The meta data builder does not contain "
            "all data that are requried to build the meta data.");
    }

    meta_data_type retval;

    // Preserve the measurements before we deconstruct the builder in the
    // subsequent process.
    retval._header_size = this->size();
    retval._particle_size = this->_format.size();

    if (meta_data_builder::has_code_page) {
        retval.code_page(this->code_page());
    }

    if (this->_bounding_boxes.empty()) {
        retval._bounding_box = bounding_box();
    } else {
        retval._bounding_box = std::move(this->_bounding_boxes[0]);
        this->_bounding_boxes.erase(this->_bounding_boxes.begin());
    }

    // TODO: Why?!
    //this->code_page(encodings::oem);

    retval._description = std::move(this->_description);
    assert(this->_description.empty());

    retval._endian = endian::system;

    retval._format_description = std::move(this->_format);
    assert(!this->_format);

    retval._frame_bounding_boxes = std::move(this->_bounding_boxes);
    assert(this->_bounding_boxes.empty());

    retval._frame_table = std::move(this->_frame_table);
    assert(this->_frame_table.empty());
    meta_data_type::calc_frame_offsets(retval);
    assert(retval._frame_table.size() == retval._frame_offsets.size());

    retval._global_particle_properties = std::move(
        this->_global_particle_properties);
    assert(this->_global_particle_properties.empty());

    retval._user_defined_semantics = std::move(this->_user_defined_semantics);
    assert(this->_user_defined_semantics.empty());

    retval._version = this->_version;
    this->_version = std::make_pair(FPD_FILE_MAJOR, FPD_FILE_MINOR);

    return retval;
}


/*
 * FPD_NAMESPACE::meta_data_builder<TChar>::size
 */
template<class TChar>
typename FPD_NAMESPACE::meta_data_builder<TChar>::size_type
FPD_NAMESPACE::meta_data_builder<TChar>::size(void) const {
    if (!*this) {
        throw std::logic_error("The size of the meta data can only be computed "
            "as long as all mandatory data are valid.");
    }

    // Magic cookie, version, BOM and overall size of segments.
    size_type retval = meta_data_type::fixed_header_size;

    // Mandatory frame table (0x01).
    retval += sizeof(header_segment);
    retval += this->_frame_table.size() * sizeof(frame_table_type::value_type);

    // Mandatory format description (0x02).
    retval += sizeof(header_segment);
    retval += this->_format.count_elements() * element_type::size_on_disk;

    // Optional user semantic description (0x03).
    if (!this->_user_defined_semantics.empty()) {
        retval += sizeof(header_segment);
        for (auto& s : this->_user_defined_semantics) {
            if (meta_data_builder::has_code_page) {
                retval += convert_string(
                    static_cast<char *>(nullptr), 0, this->code_page(),
                    s.c_str(), -1, encodings::utf8);
            } else {
                assert(sizeof(char_type) > 1);
                retval += convert_string(
                    static_cast<char *>(nullptr), 0,
                    s.c_str(), -1, encodings::utf8);
            }
        }
    }

    // Optional file description (0x04).
    if (!this->_description.empty()) {
        retval += sizeof(header_segment);
        if (meta_data_builder::has_code_page) {
            retval += convert_string(
                static_cast<char_type *>(nullptr), 0, this->code_page(),
                this->_description.c_str(), -1, encodings::utf8);
        } else {
            assert(sizeof(char_type) > 1);
            retval += convert_string(
                static_cast<char_type *>(nullptr), 0,
                this->_description.c_str(), -1, encodings::utf8);
        }
    }

    // Optional bounding boxes (0x05).
    if (!this->_bounding_boxes.empty()) {
        retval += sizeof(header_segment);
        retval += this->_bounding_boxes.front().size()
            * this->_bounding_boxes.size();
    }

    // Optional global particle properties (0x06).
    if (!this->_global_particle_properties.empty()) {
        retval += sizeof(header_segment);
        for (auto& p : this->_global_particle_properties) {
            retval += sizeof(semantic);
            retval += sizeof(format);
            retval += to_size(p.second.format());
        }
    }

    return retval;
}


/*
 * FPD_NAMESPACE::meta_data_builder<TChar>::with_bounding_box
 */
template<class TChar>
FPD_NAMESPACE::meta_data_builder<TChar>&
FPD_NAMESPACE::meta_data_builder<TChar>::with_bounding_box(
        _In_ const bounding_box& bounding_box) {
    if (!bounding_box) {
        throw std::invalid_argument("The global bounding box must be valid.");
    }

    if (this->_bounding_boxes.empty()) {
        this->_bounding_boxes.push_back(bounding_box);
    } else {
        this->_bounding_boxes[0] = bounding_box;
    }

    return *this;
}


/*
 * FPD_NAMESPACE::meta_data_builder<TChar>::with_bounding_box
 */
template<class TChar>
FPD_NAMESPACE::meta_data_builder<TChar>&
FPD_NAMESPACE::meta_data_builder<TChar>::with_bounding_box(
        _Inout_ bounding_box&& bounding_box) {
    if (!bounding_box) {
        throw std::invalid_argument("The global bounding box must be valid.");
    }

    if (this->_bounding_boxes.empty()) {
        this->_bounding_boxes.push_back(std::move(bounding_box));
    } else {
        this->_bounding_boxes[0] = std::move(bounding_box);
    }

    return *this;
}


/*
 * FPD_NAMESPACE::meta_data_builder<TChar>::with_description
 */
template<class TChar>
FPD_NAMESPACE::meta_data_builder<TChar> &
FPD_NAMESPACE::meta_data_builder<TChar>::with_description(
        _In_opt_z_ const char_type *description) {
    if (description == nullptr) {
        this->_description.clear();
    } else {
        this->_description = description;
    }

    return *this;
}


/*
 * FPD_NAMESPACE::meta_data_builder<TChar>::with_frames
 */
template<class TChar>
FPD_NAMESPACE::meta_data_builder<TChar>&
FPD_NAMESPACE::meta_data_builder<TChar>::with_frames(
        _In_ const std::initializer_list<size_type>& particles) {
    if (particles.size() < 1) {
        throw std::invalid_argument("An FPD file must contain at least one "
            "frame.");
    }

    this->_frame_table.clear();
    this->_frame_table.reserve(particles.size());

    for (auto it = particles.begin(), end = particles.end(); it != end; ++it) {
        this->_frame_table.push_back(*it);
    }

    if (this->_bounding_boxes.size() > (this->_frame_table.size() + 1)) {
        this->_bounding_boxes.resize(this->_frame_table.size() + 1);
    }

    return *this;
}


/*
 * FPD_NAMESPACE::meta_data_builder<TChar>::with_uniform_frames
 */
template<class TChar>
FPD_NAMESPACE::meta_data_builder<TChar>&
FPD_NAMESPACE::meta_data_builder<TChar>::with_uniform_frames(
        _In_ const std::size_t frames,
        _In_ const size_type particles) {
    if (frames < 1) {
        throw std::invalid_argument("An FPD file must contain at least one "
            "frame.");
    }

    this->_frame_table.resize(frames);

    for (std::size_t i = 0; i < this->_frame_table.size(); ++i) {
        this->_frame_table[i] = particles;
    }

    if (this->_bounding_boxes.size() > (this->_frame_table.size() + 1)) {
        this->_bounding_boxes.resize(this->_frame_table.size() + 1);
    }

    return *this;
}


/*
 * FPD_NAMESPACE::meta_data_builder<TChar>::with_version
 */
template<class TChar>
FPD_NAMESPACE::meta_data_builder<TChar>&
FPD_NAMESPACE::meta_data_builder<TChar>::with_version(
        _In_ const version_type version) {
    if (!check_version(version)) {
        throw std::invalid_argument("The specified version is later than the "
            "currently supported ones.");
    }

    this->_version = version;
    return *this;
}


/*
 * FPD_NAMESPACE::meta_data_builder<TChar>::operator bool
 */
template<class TChar>
FPD_NAMESPACE::meta_data_builder<TChar>::operator bool(void) const noexcept {
    if (!check_version(this->_version)) {
        // File version is not supported by the library.
        return false;
    }

    if (this->_frame_table.empty()) {
        // At least one frame is required.
        return false;
    }

    if (this->_bounding_boxes.size() > 1) {
        // If more than one bounding box is given, we need one for each frame
        // (plus the global one, which must be the first).
        const auto required = 1 + this->_frame_table.size();

        if (this->_bounding_boxes.size() != required) {
            return false;
        }
    }

    if (!this->_bounding_boxes.empty()) {
        // For more than one bounding box is given, all bounding boxes must have
        // the same format (for perfomance reasons, we check whether the bbox
        // is valid at the same time).
        const auto format = this->_bounding_boxes.front().format();
        const auto mismatch = std::any_of(
            this->_bounding_boxes.begin(),
            this->_bounding_boxes.end(),
            [format](const bounding_box &b) {
                return !b || (b.format() != format);
            });

        if (mismatch) {
            return false;
        }
    }

    if (!this->_format) {
        // The format must contain something. We do not make any assumption of
        // what semantic there should be, but we want to have at least one.
        return false;
    }

    // No problem found at this point.
    return true;
}


/*
 * FPD_NAMESPACE::meta_data_builder<TChar>::check_version
 */
template<class TChar>
constexpr bool FPD_NAMESPACE::meta_data_builder<TChar>::check_version(
        _In_ const version_type v) {
    if (v.first > FPD_FILE_MAJOR) {
        return false;
    } else {
        return (v.first < FPD_FILE_MAJOR) || (v.second <= FPD_FILE_MINOR);
    }
}


/*
 * FPD_NAMESPACE::meta_data_builder<TChar>::add_user_defined_semantic
 */
template<class TChar>
void FPD_NAMESPACE::meta_data_builder<TChar>::add_user_defined_semantic(
        _In_ const semantic s, _In_opt_z_ const char_type *d) {
    if ((s >= semantic::user_defined) && (d != nullptr)) {
        auto i = static_cast<std::size_t>(s);
        i -= static_cast<std::size_t>(semantic::user_defined);

        if (this->_user_defined_semantics.size() <= i) {
            this->_user_defined_semantics.resize(i + 1);
        }

        this->_user_defined_semantics[i] = d;
    }
}
