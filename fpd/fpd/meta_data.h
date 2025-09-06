// <copyright file="meta_data.h" company="Visualisierungsinstitut der Universität Stuttgart">
// Copyright © 2023 - 2025 Visualisierungsinstitut der Universität Stuttgart.
// Licensed under the MIT licence. See LICENCE file for details.
// </copyright>
// <author>Christoph Müller</author>

#if !defined(_FPD_META_DATA_H)
#define _FPD_META_DATA_H
#pragma once

#include <cstdlib>
#include <map>
#include <memory>

#include "fpd/bounding_box.h"
#include "fpd/compression_meta_data.h"
#include "fpd/convert_string.h"
#include "fpd/endian.h"
#include "fpd/format_description.h"
#include "fpd/format_variant.h"
#include "fpd/header_segment.h"
#include "fpd/hierarchy_meta_data.h"
#include "fpd/io.h"
#include "fpd/swap_bytes.h"
#include "fpd/with_encoding.h"


FPD_NAMESPACE_BEGIN

/// <summary>
/// A container for all meta data in an FPD file.
/// </summary>
/// <typeparam name="TChar">The type of a character in a string.</typeparam>
template<class TChar>
class meta_data final : protected detail::with_encoding<TChar> {

public:

    /// <summary>
    /// The type of a character in a string.
    /// </summary>
    typedef TChar char_type;

    /// <summary>
    /// The type of an element in the format descriptor.
    /// </summary>
    typedef typename format_description<char_type>::element_type element_type;

    /// <summary>
    /// The type used to specify the endian of the data.
    /// </summary>
    typedef FPD_NAMESPACE::endian endian_type;

    /// <summary>
    /// The type used to specify the format of data.
    /// </summary>
    typedef FPD_NAMESPACE::format format_type;

    /// <summary>
    /// The type of the particle format descriptor.
    /// </summary>
    typedef format_description<char_type> format_description_type;

    /// <summary>
    /// The type used to represent the number of particles in each frame.
    /// </summary>
    typedef std::vector<std::uint64_t> frame_table_type;

    /// <summary>
    /// The type of a string.
    /// </summary>
    typedef std::basic_string<char_type> string_type;

    /// <summary>
    /// The type of a semantic.
    /// </summary>
    typedef typename format_description_type::semantic_type semantic_type;

    /// <summary>
    /// The type used to express sizes and positive offsets.
    /// </summary>
    typedef frame_table_type::value_type size_type;

    /// <summary>
    /// The type used to represent the version of the file.
    /// </summary>
    typedef std::pair<std::uint8_t, std::uint8_t> version_type;

    /// <summary>
    /// Checks whether <paramref name="data" /> designates a valid FPD header
    /// and, in case of success, returns the overall size of the header
    /// segments.
    /// </summary>
    /// <param name="output">Receives the file version and the endian parsed
    /// from the <paramref name="data" />.</param>
    /// <param name="data">A pointer to the begin of the file header.</param>
    /// <param name="cnt">The length of the file header in bytes. As of now,
    /// the caller must provide at least <see cref="fixed_header_size" /> bytes
    /// of data for the method to succeed.</param>
    /// <returns>The size of the header segments following the file header,
    /// in bytes. This might include data already passed to the method.
    /// </returns>
    /// <exception cref="std::invalid_argument">If <pararmef name="data" /> is
    /// <c>nullptr</c>, or if <paramref name="cnt" /> does not contain
    /// sufficient header data, or if the given data are obviously invalid.
    /// </exception>
    static std::size_t check_header_size(_Out_ meta_data& output,
        _In_ const void *data,
        _In_ const std::size_t cnt);

    /// <summary>
    /// Parses the header segments starting at <paramref name="data" /> using
    /// the file information provided in <paramref name="meta_data" />.
    /// </summary>
    /// <param name="meta_data">The static meta data (file version and endian)
    /// as obtained by <see cref="check_header_size" />. On exit, this object
    /// has received all header segments the parser understands.</param>
    /// <param name="data">A pointer to the flexible header segments.</param>
    /// <param name="cnt">The number of bytes in <paramref name="data" />, which
    /// form the header segments. This must the amount of bytes returned by
    /// <see cref="check_header_size" />. Note that <paramref name="cnt" /> must
    /// not exceed the actual number of bytes in header segments!</param>
    /// <returns><paramref name="meta_data" />.</returns>
    static meta_data& parse(_Inout_ meta_data& meta_data, _In_ const void *data,
        _In_ const std::size_t cnt);

    /// <summary>
    /// Parses the meta data from the given header block.
    /// </summary>
    /// <param name="data">A pointer to the begin of the file header.</param>
    /// <param name="cnt">The length of the file header in bytes.</param>
    /// <returns>The meta data parsed from the given bytes.</returns>
    /// <exception cref="std::invalid_argument">If <pararmef name="data" /> is
    /// <c>nullptr</c>, or if <paramref name="cnt" /> does not comprise a full
    /// FPD header, or if the given data are obviously invalid.</exception>
    static meta_data parse(_In_ const void *data, _In_ const std::size_t cnt);

    /// <summary>
    /// The size if the fixed file header, which is the minimum of data that
    /// needs to be provided to <see cref="check_header_size" />.
    /// </summary>
    static constexpr const std::size_t fixed_header_size
        = 3 * sizeof(char)
        + 2 * sizeof(std::uint8_t)
        + sizeof(endian_type)
        + sizeof(std::uint32_t);

    /// <summary>
    /// Initialises a new instance.
    /// </summary>
    /// <param name="encoding">The code page to convert strings to if
    /// <see cref="char_type" /> does not implicitly carry an encoding. Note
    /// to MegaMol users: <c>char</c> is not UTF-8 by definition, it <i>can</i>
    /// be UTF-8 if the data are handled accordingly.</param>
    explicit inline meta_data(_In_opt_ const FPD_NAMESPACE::encoding encoding
            = FPD_NAMESPACE::encodings::oem) noexcept
        : detail::with_encoding<TChar>(encoding),
            _compression(std::make_shared<compression_meta_data<
                compression::none>>()),
            _endian(endian_type::undefined),
            _header_size(0),
            _hierarchy(std::make_shared<hierarchy_meta_data<
                spatial_hierarchy::none>>()),
            _particle_size(0),
            _version(std::uint8_t(0), std::uint8_t(0)) { }

    /// <summary>
    /// Answer the compression method used for the particle data.
    /// </summary>
    /// <remarks>
    /// The method returns an interface that allows for at least determining the
    /// compression method. Depending on this method, the interface can be
    /// casted to a more specific specialisation of
    /// <see cref="compression_meta_data" /> that provides additional
    /// information on how to decompress the data.
    /// </remarks>
    /// <returns>The compression method.</returns>
    inline const compression_meta_data_base& compression(void) const noexcept {
        assert(this->_compression != nullptr);
        return *this->_compression;
    }

    /// <summary>
    /// Gets, if available a human-readable description of the file.
    /// </summary>
    /// <returns>The description of the file.</returns>
    inline const string_type& description(void) const noexcept {
        return this->_description;
    }

    /// <summary>
    /// Gets the endian of the file.
    /// </summary>
    /// <returns>The endian of the data in the file.</returns>
    inline endian_type endian(void) const noexcept {
        return this->_endian;
    }

    /// <summary>
    /// Gets the description of the particle format.
    /// </summary>
    /// <returns>The description of the particle format.</returns>
    inline const format_description_type& format_description(
            void) const noexcept {
        return this->_format_description;
    }

    /// <summary>
    /// Provides access to the <paramref name="element" />th element of the
    /// <paramref name="group" />th group of elements in the format description.
    /// </summary>
    /// <remarks>
    /// This is a convenience method that provides a shortcut for a path
    /// otherwise requiring to call <see cref="format_description()" />.
    /// </remarks>
    /// <param name="group">The zero-based index of the group from which to
    /// retrieve the element.</param>
    /// <param name="element">THe zero-based index of the element within the
    /// group.</param>
    /// <returns>A reference to the specified element.</returns>
    inline const element_type& format_description(
            _In_ const std::size_t group,
            _In_ const std::size_t element) const {
        return this->_format_description(group, element);
    }

    /// <summary>
    /// Answer the overall number of frames in the file.
    /// </summary>
    /// <returns>The overall number of frames in the file.</returns>
    inline std::size_t frames(void) const noexcept {
        return this->_frame_table.size();
    }

    /// <summary>
    /// Answer the offset of the given frame, in bytes, from the begin of the
    /// file.
    /// </summary>
    /// <param name="frame">The zero-based index of the frame.</param>
    /// <returns>The offset of the frame in bytes.</returns>
    /// <exception cref="std::range_error">If <paramref name="frame" /> is
    /// out of range of the <see cref="frame_table" />.</exception>
    size_type frame_offset(_In_ const std::size_t frame) const;

    /// <summary>
    /// Answer the size of the given frame in bytes.
    /// </summary>
    /// <param name="frame">The zero-based index of the frame.</param>
    /// <returns>The number of bytes in the frame.</returns>
    /// <exception cref="std::range_error">If <paramref name="frame" /> is
    /// out of range of the <see cref="frame_table" />.</exception>
    size_type frame_size(_In_ const std::size_t frame) const;

    /// <summary>
    /// Gets the table of frame sizes (in number of particles).
    /// </summary>
    /// <returns>The frame table.</returns>
    inline const frame_table_type& frame_table(void) const noexcept {
        return this->_frame_table;
    }

    /// <summary>
    /// Gets the list of properties that apply to all particles and are not
    /// specified on a per-particle basis.
    /// </summary>
    /// <returns>The list of global particle properties.</returns>
    inline const std::map<semantic, format_variant>& global_particle_properties(
            void) const noexcept {
        return this->_global_particle_properties;
    }

    /// <summary>
    /// Answer the offset of the given element <paramref name="group" />
    /// from the begin of the given <paramref name="frame" />.
    /// </summary>
    /// <param name="frame">The zero-based index of the frame to retrieve the
    /// offset for.</param>
    /// <param name="group">The zero-based index of the group to retrieve the
    /// offset for.</param>
    /// <returns>The offset of the group from the begin of the frame, in bytes.
    /// </returns>
    /// <exception cref="std::range_error">If <paramref name="frame" /> is
    /// out of range of the <see cref="frame_table" />, or if
    /// <paramref name="group" /> is out of the range of the
    /// <see cref="format_description" />.</exception>
    size_type group_offset(_In_ const std::size_t frame,
        _In_ const std::size_t group) const;

    /// <summary>
    /// Answer the spatial hierarchy used to organise the particle data.
    /// </summary>
    /// <remarks>
    /// The method returns an interface that allows for at least determining the
    /// type of the hierarchy. Depending on this type, the interface can be
    /// casted to a more specific specialisation of
    /// <see cref="hierarchy_meta_data" /> that provides additional information
    /// on how to interpret the data.
    /// </remarks>
    /// <returns>The type of the spatial hierarchy.</returns>
    inline const hierarchy_meta_data_base& hierarchy(void) const noexcept {
        assert(this->_hierarchy != nullptr);
        return *this->_hierarchy;
    }

    /// <summary>
    /// Answer the number of particles in the given frame.
    /// </summary>
    /// <param name="frame">The zero-based index of the frame to compute the
    /// number of particles for.</param>
    /// <returns>The number of particles in the frame.</returns>
    /// <exception cref="std::range_error">If <paramref name="frame" /> is
    /// out of range of the <see cref="frame_table" />.</exception>
    std::size_t particles(_In_ const std::size_t frame) const;

    /// <summary>
    /// Write the whole meta data to the specified output.
    /// </summary>
    /// <typeparam name="TFile">The I/O traits used for writing.</typeparam>
    /// <param name="file">The file to write to.</param>
    /// <returns>The number of bytes that have been written, which is always the
    /// overall size of the meta data block.</returns>
    template<class TFile> std::size_t save(_In_ TFile& file) const;

    /// <summary>
    /// Gets the textual description of the specified semantic.
    /// </summary>
    /// <param name="semantic">The semantic to get the human-readable
    /// description for. This must be a user-defined semantic.</param>
    /// <returns>The human-readable description of the given user defined
    /// semantic.</returns>
    const string_type& user_defined_semantic(
        _In_ const semantic_type semantic) const;

    /// <summary>
    /// Gets the version of the file.
    /// </summary>
    /// <returns>The version of the file.</returns>
    inline version_type version(void) const noexcept {
        return this->_version;
    }

private:

    static void calc_frame_offsets(_Inout_ meta_data& meta_data);

    static void parse_bounding_box(_Inout_ meta_data& meta_data,
        _In_ const void *data, _In_ const std::size_t cnt);

    static void parse_compression(_Inout_ meta_data& meta_data,
        _In_ const void *data, _In_ const std::size_t cnt);

    static void parse_description(_Inout_ meta_data& meta_data,
        _In_ const void *data, _In_ const std::size_t cnt);

    static void parse_format_description(_Inout_ meta_data& meta_data,
        _In_ const void *data, _In_ const std::size_t cnt);

    static void parse_frame_table(_Inout_ meta_data& meta_data,
        _In_ const void *data, _In_ const std::size_t cnt);

    static void parse_global_particle_properties(_Inout_ meta_data& meta_data,
        _In_ const void *data, _In_ const std::size_t cnt);

    static void parse_spatial_hierarchy(_Inout_ meta_data& meta_data,
        _In_ const void *data, _In_ const std::size_t cnt);

    static void parse_user_semantics(_Inout_ meta_data& meta_data,
        _In_ const void *data, _In_ const std::size_t cnt);

    template<class TFile>
    static std::size_t write(_In_ TFile& file,
        _In_ const bounding_box& bbox,
        _In_ const FPD_NAMESPACE::endian endian);

    template<class TFile>
    static std::size_t write(_In_ TFile& file,
        _In_ const element_type& element,
        _In_ std::uint16_t group,
        _In_ const FPD_NAMESPACE::endian endian);

    template<class TFile>
    static inline std::size_t write(_In_ TFile& file,
            _In_ format format,
            _In_ const FPD_NAMESPACE::endian endian) {
        format = swap_enum_endian(format, endian);
        return detail::write_all(file, format);
    }

    template<class TFile>
    static std::size_t write(_In_ TFile& file,
        _In_ const header_segment segment,
        _In_ const std::underlying_type_t<header_segment> size,
        _In_ const FPD_NAMESPACE::endian endian);

    template<class TFile, class TValue>
    static std::size_t write(_In_ TFile& file,
        _In_ const std::vector<TValue>& values,
        _In_ const FPD_NAMESPACE::endian endian);

    string_type to_string(_In_ const char *begin, _In_ const char *end) const;

    string_type to_string(_In_reads_(cnt) const void *data,
        _In_ const std::size_t cnt) const;

    std::string to_utf8(_In_reads_or_z_opt_(cnt) const char_type *str,
        _In_ const int cnt = -1) const;

    std::vector<char> to_utf8(
        _In_ const std::vector<string_type>& strings) const;

    bounding_box _bounding_box;
    std::shared_ptr<compression_meta_data_base> _compression;
    string_type _description;
    endian_type _endian;
    format_description_type _format_description;
    std::vector<bounding_box> _frame_bounding_boxes;
    frame_table_type _frame_offsets;
    frame_table_type _frame_table;
    std::map<semantic, format_variant> _global_particle_properties;
    size_type _header_size;
    std::shared_ptr<hierarchy_meta_data_base> _hierarchy;
    size_type _particle_size;
    std::vector<string_type> _user_defined_semantics;
    version_type _version;

    template<class> friend class meta_data_builder;
};

FPD_NAMESPACE_END

#include "fpd/meta_data.inl"

#endif /* !defined(_FPD_META_DATA_H) */
