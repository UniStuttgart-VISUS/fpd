// <copyright file="meta_data.h" company="Visualisierungsinstitut der Universität Stuttgart">
// Copyright © 2023 - 2024 Visualisierungsinstitut der Universität Stuttgart.
// Licensed under the MIT licence. See LICENCE file for details.
// </copyright>
// <author>Christoph Müller</author>

#pragma once

#include <cstdlib>
#include <map>

#include "fpd/annotations.h"
#include "fpd/bounding_box.h"
#include "fpd/convert_string.h"
#include "fpd/endian.h"
#include "fpd/format_description.h"
#include "fpd/format_variant.h"
#include "fpd/header_segment.h"
#include "fpd/swap_bytes.h"
#include "fpd/with_code_page.h"


FPD_NAMESPACE_BEGIN

/// <summary>
/// A container for all meta data in an FPD file.
/// </summary>
/// <typeparam name="TChar">The type of a character in a string.</typeparam>
template<class TChar> class meta_data final : protected with_code_page<TChar> {

public:

    /// <summary>
    /// The type of a character in a string.
    /// </summary>
    typedef TChar char_type;

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
    /// The type used to represent the absolute offsets of the frames into
    /// the file.
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
    /// The type used to express file sizes and offsets.
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
    /// <param name="meta_data">The static metadata (file version and endian)
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
    /// <param name="code_page">The code page to convert strings to if
    /// <see cref="char_type" /> does not implicitly carry an encoding. Note
    /// to MegaMol users: <c>char</c> is not UTF-8 by definition, it <i>can</i>
    /// be UTF-8 if the data are handled accordingly.</param>
    explicit inline meta_data(_In_ const FPD_NAMESPACE::code_page code_page
            = FPD_NAMESPACE::code_page::oem) noexcept
        : with_code_page<TChar>(code_page), _endian(endian_type::undefined),
            _file_size(0), _version(0, 0) { }

    /// <summary>
    /// Gets the endian of the file.
    /// </summary>
    /// <returns>The endian of the data in the file.</returns>
    inline endian_type endian(void) const noexcept {
        return this->_endian;
    }

    /// <summary>
    /// Answer the overall size of the FPD file in bytes.
    /// </summary>
    /// <returns>The size of the FPD file in bytes.</returns>
    inline size_type file_size(void) const noexcept {
        return this->_file_size;
    }

    /// <summary>
    /// Sets the overall size of the FPD file described by these meta data.
    /// </summary>
    /// <remarks>
    /// This information cannot be parsed from the file itself, but must be
    /// determined externally and set manually.
    /// </remarks>
    /// <param name="file_size">The file size in bytes.</param>
    /// <returns><c>*this</c>.</returns>
    inline meta_data& file_size(_In_ const size_type file_size) noexcept {
        this->_file_size = file_size;
        return *this;
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
    /// Answer the overall number of frames in the file.
    /// </summary>
    /// <returns>The overall number of frames in the file.</returns>
    inline std::size_t frames(void) const noexcept {
        return this->_frame_table.size();
    }

    /// <summary>
    /// Answer the offset of the given frame from the begin of the file.
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
    /// <returns>The size of the frame in bytes.</returns>
    /// <exception cref="std::range_error">If <paramref name="frame" /> is
    /// out of range of the <see cref="frame_table" />.</exception>
    /// <exception cref="std::logic_error">If <see cref="file_size" /> is
    /// obviously too small, most likely because it has not been set at all.
    /// This might also be the case if the frame table holds invalid offsets,
    /// i.e. if the frames are not sorted correctly.</exception>
    size_type frame_size(_In_ const std::size_t frame) const;

    /// <summary>
    /// Gets the offsets of the frames in the file.
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
    /// Answer the number of particles in the given frame.
    /// </summary>
    /// <param name="frame">The zero-based index of the frame to compute the
    /// number of particles for.</param>
    /// <returns>The number of particles in the frame.</returns>
    /// <exception cref="std::range_error">If <paramref name="frame" /> is
    /// out of range of the <see cref="frame_table" />.</exception>
    inline std::size_t particles(_In_ const std::size_t frame) const {
        const auto frame_size = this->frame_size(frame);
        const auto particle_size = this->_format_description.size();
        assert(frame_size % particle_size == 0);
        return static_cast<std::size_t>(frame_size / particle_size);
    }

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

    static void parse_bounding_box(_Inout_ meta_data& meta_data,
        _In_ const void *data, _In_ const std::size_t cnt);

    static void parse_description(_Inout_ meta_data& meta_data,
        _In_ const void *data, _In_ const std::size_t cnt);

    static void parse_format_description(_Inout_ meta_data& meta_data,
        _In_ const void *data, _In_ const std::size_t cnt);

    static void parse_frame_table(_Inout_ meta_data& meta_data,
        _In_ const void *data, _In_ const std::size_t cnt);

    static void parse_global_particle_properties(_Inout_ meta_data& meta_data,
        _In_ const void *data, _In_ const std::size_t cnt);

    static void parse_user_semantics(_Inout_ meta_data& meta_data,
        _In_ const void *data, _In_ const std::size_t cnt);

    static string_type to_string(_In_ const meta_data& meta_data,
        _In_ const char16_t *begin, _In_ const char16_t *end);

    static string_type to_string(_In_ const meta_data& meta_data,
        _In_reads_(cnt) const void *data, _In_ const std::size_t cnt);

    bounding_box _bounding_box;
    string_type _description;
    endian_type _endian;
    size_type _file_size;
    format_description_type _format_description;
    std::vector<bounding_box> _frame_bounding_boxes;
    frame_table_type _frame_table;
    std::map<semantic, format_variant> _global_particle_properties;
    std::vector<string_type> _user_defined_semantics;
    version_type _version;

    template<class> friend class meta_data_builder;
};

FPD_NAMESPACE_END

#include "fpd/meta_data.inl"
