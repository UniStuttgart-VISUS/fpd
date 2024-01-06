// <copyright file="meta_data_builder.h" company="Visualisierungsinstitut der Universität Stuttgart">
// Copyright © 2023 - 2024 Visualisierungsinstitut der Universität Stuttgart.
// Licensed under the MIT licence. See LICENCE file for details.
// </copyright>
// <author>Christoph Müller</author>

#pragma once

#include <algorithm>

#include "fpd/convert_string.h"
#include "fpd/meta_data.h"


FPD_NAMESPACE_BEGIN

/// <summary>
/// The meta data builder is for incrementally collecting meta data when writing
/// an FPD file.
/// </summary>
/// <typeparam name="TChar">The type of a character in a string.</typeparam>
template<class TChar>
class meta_data_builder final : protected with_code_page<TChar> {

public:

    /// <summary>
    /// The type of a character in a string.
    /// </summary>
    typedef TChar char_type;

    /// <summary>
    /// The type of meta data we are building here.
    /// </summary>
    typedef meta_data<char_type> meta_data_type;

    /// <summary>
    /// The type of the particle format descriptor.
    /// </summary>
    typedef typename meta_data_type::format_description_type
        format_description_type;

    /// <summary>
    /// The type of an initialiser for a format element.
    /// </summary>
    typedef typename format_description_type::initialiser_type
        element_initialiser_type;

    /// <summary>
    /// The type of a format element.
    /// </summary>
    typedef typename format_description_type::element_type element_type;

    /// <summary>
    /// The type used to represent the absolute offsets of the frames into
    /// the file.
    /// </summary>
    typedef typename meta_data_type::frame_table_type frame_table_type;

    /// <summary>
    /// The type of a string.
    /// </summary>
    typedef typename meta_data_type::string_type string_type;

    /// <summary>
    /// The type used to express file sizes and offsets.
    /// </summary>
    typedef typename meta_data_type::size_type size_type;

    /// <summary>
    /// The type used to represent the version of the file.
    /// </summary>
    typedef typename meta_data_type::version_type version_type;

    /// <summary>
    /// Initialises a new version.
    /// </summary>
    meta_data_builder(void);

    /// <summary>
    /// Appends an element to the format description.
    /// </summary>
    /// <param name="semantic">The semantic of the element to be added.</param>
    /// <param name="semantic_name">The human-readable name of the semantic to
    /// be added. This can be <c>nullptr</c> if the <paramref name="semantic" />
    /// is a built-in one or if the name of the semantic has been previously
    /// registered. If it is not <c>nullptr</c> and <paramref name="semantic" />
    /// is a user-defined semantic, the name will be registered and will
    /// possibly overwrite previous names for the same semantic.</param>
    /// <param name="group">The zero-based index of the group to add the element
    /// to. If this group does not yet exist, it will be created.</param>
    /// <param name="format">The format of the element.</param>
    /// <param name="offset">The offset of the element from the begin of the
    /// group. This can be zero if the element starts immediately after the
    /// previous one without any padding.</param>
    /// <param name="size">The size of the element. This can be zero if the
    /// <paramref name="format" /> is one with a known size.</param>
    /// <param name="eval_offset">If <c>true</c>, add an explicit offset to the
    /// element based on the current content of the group.</param>
    /// <returns><c>*this</c>.</returns>
    inline meta_data_builder& add_format_element(
            _In_ const semantic semantic,
            _In_opt_z_ const char_type *semantic_name,
            _In_ const format format,
            _In_ const std::size_t group = 0,
            _In_ const std::uint16_t offset = 0,
            _In_ const std::uint16_t size = 0,
            _In_ const bool eval_offset = false) {
        this->_format.add(semantic, semantic_name, format, group, offset, size,
            eval_offset);
        this->add_user_defined_semantic(semantic, semantic_name);
        return *this;
    }

    /// <summary>
    /// Appends an element to the format description, which must use a built-in
    /// semantic.
    /// </summary>
    /// <param name="semantic">The semantic of the element to be added.</param>
    /// <param name="group">The zero-based index of the group to add the element
    /// to. If this group does not yet exist, it will be created.</param>
    /// <param name="format">The format of the element.</param>
    /// <param name="offset">The offset of the element from the begin of the
    /// group. This can be zero if the element starts immediately after the
    /// previous one without any padding.</param>
    /// <param name="size">The size of the element. This can be zero if the
    /// <paramref name="format" /> is one with a known size.</param>
    /// <param name="eval_offset">If <c>true</c>, add an explicit offset to the
    /// element based on the current content of the group.</param>
    /// <returns><c>*this</c>.</returns>
    inline meta_data_builder& add_format_element(
            _In_ const semantic semantic,
            _In_ const format format,
            _In_ const std::size_t group = 0,
            _In_ const std::uint16_t offset = 0,
            _In_ const std::uint16_t size = 0,
            _In_ const bool eval_offset = false) {
        this->_format.add(semantic, format, group, offset, size, eval_offset);
        return *this;
    }

    /// <summary>
    /// Adds a new element to the format description.
    /// </summary>
    /// <param name="initialiser">The initialiser for the new element. If the
    /// initialiser uses a user-defined <see cref="semantic" /> and provides
    /// a non-<c>nullptr</c> name for it, the method will register the name of
    /// the semantic and possibly overwrite a previous name for the same
    /// semantic.</param>
    /// <param name="eval_offset">If <c>true</c>, add an explicit offset to the
    /// element based on the current content of the group.</param>
    /// <returns><c>*this</c>.</returns>
    inline meta_data_builder& add_format_element(
            _In_ const element_initialiser_type& initialiser,
            _In_ const bool eval_offset = false) {
        this->_format.add(initialiser, eval_offset);
        this->add_user_defined_semantic(initialiser._semantic,
            initialiser._semantic_name);
        return *this;
    }

    /// <summary>
    /// Builds the <see cref="meta_data" /> and resets the builder.
    /// </summary>
    /// <returns>The meta data described by this builder.</returns>
    meta_data_type build(void);

    /// <summary>
    /// Compute the size of the meta data in bytes up to the first frame.
    /// </summary>
    /// <remkars>
    /// The size reported here comprises the static file header as well as the
    /// on-disk representation of everything in this builder. It is the offset
    /// of the first frame in the file.
    /// </remkars>
    /// <returns>The size of the meta data.</returns>
    size_type size(void) const;

    /// <summary>
    /// Sets the code page to be assumed for narrow strings.
    /// </summary>
    /// <remarks>
    /// The code page is not relevant if the application uses UTF-16 strings and
    /// will be discarded.
    /// </remarks>
    /// <param name="code_page">The code page of narrow strings.</param>
    /// <returns><c>*this</c>.</returns>
    meta_data_builder& using_code_page(
            _In_ const code_page_type code_page) noexcept {
        this->code_page(code_page);
        return *this;
    }

    /// <summary>
    /// Sets the global bounding box of the particles.
    /// </summary>
    /// <param name="bounding_box">The global bounding box of the particles.
    /// </param>
    /// <returns><c>*this</c></returns>
    /// <exception cref="std::invalid_argument">If the specified bounding
    /// box contains no data.</exception>
    meta_data_builder& with_bounding_box(_In_ const bounding_box& bounding_box);

    /// <summary>
    /// Sets the global bounding box of the particles.
    /// </summary>
    /// <param name="bounding_box">The global bounding box of the particles.
    /// </param>
    /// <returns><c>*this</c></returns>
    /// <exception cref="std::invalid_argument">If the specified bounding
    /// box contains no data.</exception>
    meta_data_builder& with_bounding_box(_Inout_ bounding_box&& bounding_box);

    /// <summary>
    /// Sets the global bounding box of the particles.
    /// </summary>
    /// <typeparam name="TValue">The type of the values.</typeparam>
    /// <param name="min_x">The minimum value on the x-axis.</param>
    /// <param name="min_y">The minimum value on the y-axis.</param>
    /// <param name="min_z">The minimum value on the z-axis.</param>
    /// <param name="max_x">The maximum value on the x-axis.</param>
    /// <param name="max_y">The maximum value on the y-axis.</param>
    /// <param name="max_z">The maximum value on the z-axis.</param>
    /// <exception cref="std::logic_error">If <typeparamref name="TValue" />
    /// is not a valid scalar arithmetic type.</exception>
    template<class TValue> inline meta_data_builder& with_bounding_box(
            _In_ const TValue min_x, _In_ const TValue min_y,
            _In_ const TValue min_z, _In_ const TValue max_x,
            _In_ const TValue max_y, _In_ const TValue max_z) {
        return this->with_bounding_box(bounding_box(min_x, min_y, min_z,
            max_x, max_y, max_z));
    }

    /// <summary>
    /// Sets a description of the file.
    /// </summary>
    /// <param name="description">The description of the file, which may be
    /// <c>nullptr</c> to erase existing data.</param>
    /// <returns><c>*this</c>.</returns>
    meta_data_builder& with_description(
        _In_opt_z_ const char_type *description);

    /// <summary>
    /// Sets a description of the file.
    /// </summary>
    /// <param name="description">The description of the file.</param>
    /// <returns><c>*this</c>.</returns>
    meta_data_builder& with_description(_In_ const string_type& description) {
        this->_description = description;
        return *this;
    }

    /// <summary>
    /// Sets a description of the file.
    /// </summary>
    /// <param name="description">The description of the file.</param>
    /// <returns><c>*this</c>.</returns>
    meta_data_builder& with_description(
            _Inout_ string_type&& description) noexcept {
        this->_description = std::move(description);
        return *this;
    }

    /// <summary>
    /// Sets the type of endian expected for the data on disk.
    /// </summary>
    /// <remarks>
    /// <para>The endian will only be honoured when writing the meta data. The
    /// caller is make sure to perform any conversion necessary to match the
    /// endian of the particle data with the one configured here.</para>
    /// <para>It is not necessary to set the endian if the data are using the
    /// endian of the system the code is running on as this will be implicitly
    /// assumed.</para>
    /// </remarks>
    /// <param name="endian">The endian of the data on disk.</param>
    /// <returns><c>*this</c>.</returns>
    meta_data_builder& with_endian(_In_ const endian endian) noexcept;

    /// <summary>
    /// Sets the format of the particles.
    /// </summary>
    /// <param name="format">The description of the format of the particles.
    /// </param>
    /// <returns></returns>
    /// <returns><c>*this</c>.</returns>
    meta_data_builder& with_format(_In_ const format_description_type& format) {
        this->_format = format;
        return *this;
    }

    /// <summary>
    /// Sets the format of the particles.
    /// </summary>
    /// <param name="format">The description of the format of the particles.
    /// </param>
    /// <returns></returns>
    /// <returns><c>*this</c>.</returns>
    meta_data_builder& with_format(
            _Inout_ format_description_type&& format) noexcept {
        this->_format = std::move(format);
        return *this;
    }

    /// <summary>
    /// Sets the number of frames in the file.
    /// </summary>
    /// <param name="frames">The number of frames in the file.</param>
    /// <returns><c>*this</c>.</returns>
    /// <exception cref="std::invalid_argument">If the number of frames is less
    /// than one.</exception>
    meta_data_builder& with_frames(_In_ const std::size_t frames);

    /// <summary>
    /// Sets the version of the file to produce.
    /// </summary>
    /// <remarks>
    /// It is not necessary to set the current version as this will be
    /// implicitly used if no version is specified.
    /// </remarks>
    /// <param name="major">The major file version.</param>
    /// <param name="minor">The minor file version.</param>
    /// <returns><c>*this</c>.</returns>
    /// <exception cref="std::invalid_argument">If the version is later than
    /// the latest one supported by the library.</exception>
    inline meta_data_builder& with_version(_In_ const std::uint8_t major,
            _In_ const std::uint8_t minor) {
        return this->with_version(std::make_pair(major, minor));
    }

    /// <summary>
    /// Sets the version of the file to produce.
    /// </summary>
    /// <remarks>
    /// It is not necessary to set the current version as this will be
    /// implicitly used if no version is specified.
    /// </remarks>
    /// <param name="version">The version of the file to produce.</param>
    /// <returns><c>*this</c>.</returns>
    /// <exception cref="std::invalid_argument">If the version is later than
    /// the latest one supported by the library.</exception>
    meta_data_builder& with_version(_In_ const version_type version);

    /// <summary>
    /// Indicates whether the meta data currently held by the builder are in
    /// a consistent state.
    /// </summary>
    /// <returns><c>true</c> if the meta data are currently valid, <c>false</c>
    /// otherwise.</returns>
    operator bool(void) const noexcept;

private:

    static constexpr bool check_version(_In_ const version_type v);

    void add_user_defined_semantic(_In_ const semantic s,
        _In_opt_z_ const char_type *d);

    std::vector<bounding_box> _bounding_boxes;
    string_type _description;
    endian _endian;
    format_description_type _format;
    frame_table_type _frame_table;
    std::map<semantic, format_variant> _global_particle_properties;
    std::vector<string_type> _user_defined_semantics;
    version_type _version;
};

FPD_NAMESPACE_END

#include "fpd/meta_data_builder.inl"
