// <copyright file="basic_bounding_box.h" company="Visualisierungsinstitut der Universität Stuttgart">
// Copyright © 2023 - 2025 Visualisierungsinstitut der Universität Stuttgart.
// Licensed under the MIT licence. See LICENCE file for details.
// </copyright>
// <author>Christoph Müller</author>

#if !defined(_FPD_BOUNDING_BOX_H)
#define _FPD_BOUNDING_BOX_H
#pragma once

#include <algorithm>
#include <cassert>
#include <cstring>
#include <stdexcept>
#include <type_traits>
#include <vector>

#include "fpd/endian.h"
#include "fpd/format_traits.h"


FPD_NAMESPACE_BEGIN

/// <summary>
/// Representation of a 3D bounding box.
/// </summary>
/// <remarks>
/// The bounding box class type-erases the actual type of the coordinates.
/// Before retrieving data, the <see cref="format" /> must be checked to
/// interpret the data correctly.
/// </remarks>
/// <typeparam name="TByte">The type used to represent a byte.</typeparam>
template<class TByte = std::uint8_t> class basic_bounding_box final {

    static_assert(sizeof(TByte) == 1, "The byte type must be exactly one "
        "byte in size.");

public:

    /// <summary>
    /// Defines the valid coordinates that can be used to index into the
    /// bounding box.
    /// </summary>
    enum class value {

        /// <summary>
        /// The location of the minimum position on the x-axis.
        /// </summary>
        min_x = 0,

        /// <summary>
        /// The location of the minimum position on the y-axis.
        /// </summary>
        min_y = 1,

        /// <summary>
        /// The location of the minimum position on the z-axis.
        /// </summary>
        min_z = 2,

        /// <summary>
        /// The location of the maximum position on the x-axis.
        /// </summary>
        max_x = 3,

        /// <summary>
        /// The location of the maximum position on the y-axis.
        /// </summary>
        max_y = 4,

        /// <summary>
        /// The location of the maximum position on the z-axis.
        /// </summary>
        max_z = 5,
    };

    /// <summary>
    /// The type used to represent a byte.
    /// </summary>
    typedef TByte byte_type;

    /// <summary>
    /// The type used to specify the format of the components.
    /// </summary>
    typedef FPD_NAMESPACE::format format_type;

    /// <summary>
    /// Initialises a new instance.
    /// </summary>
    inline basic_bounding_box(void) noexcept : _format(format_type::unknown) { }

    /// <summary>
    /// Initialises an empty bounding box of the given format.
    /// </summary>
    /// <param name="format">The scalar format of the bounding box.</param>
    explicit inline basic_bounding_box(_In_ const format_type format)
            : _format(format) {
        constexpr auto zero = static_cast<byte_type>(0);
        this->_values.resize(6 * to_size(this->_format));
        assert(*this);
        std::fill(this->_values.begin(), this->_values.end(), zero);
        assert(this->empty());
    }

    /// <summary>
    /// Initialises a new instance from the given bounds.
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
    template<class TValue> basic_bounding_box(
        _In_ const TValue min_x, _In_ const TValue min_y,
        _In_ const TValue min_z, _In_ const TValue max_x,
        _In_ const TValue max_y, _In_ const TValue max_z);

    /// <summary>
    /// Initialises a new instance.
    /// </summary>
    /// <typeparam name="TInput">The type used to represent an input byte.
    /// </typeparam>
    /// <param name="format">The scalar format of the bounding box.</param>
    /// <param name="values">A pointer to at least six values of the specified
    /// format.</param>
    /// <param name="cnt">The size of the buffer designated by
    /// <paramref name="values" />, in bytes.</param>
    /// <param name="endian">The endian in which <paramref name="values" /> are
    /// represented. If this is not the system endian, the constructor will
    /// convert the data. This parameter defaults to the system endian.</param>
    template<class TInput>
    basic_bounding_box(
        _In_ const std::enable_if_t<sizeof(TInput) == 1, format_type> format,
        _In_reads_bytes_(cnt) const TInput *values,
        _In_ std::size_t cnt,
        _In_ endian endian = endian::system);

    /// <summary>
    /// Initialise a copy.
    /// </summary>
    /// <param name="rhs">The object to be cloned.</param>
    inline basic_bounding_box(_In_ const basic_bounding_box& rhs)
        : _format(rhs._format), _values(rhs._values) { }

    /// <summary>
    /// Initialise a copy.
    /// </summary>
    /// <typeparam name="TOther">The byte type used by <paramref name="rhs" />.
    /// </typeparam>
    /// <param name="rhs">The object to be cloned.</param>
    template<class TOther>
    basic_bounding_box(_In_ const basic_bounding_box<TOther>& rhs);

    /// <summary>
    /// Initialise from move.
    /// </summary>
    /// <param name="rhs">The object to be moved.</param>
    inline basic_bounding_box(_Inout_ basic_bounding_box&& rhs) noexcept
            : _format(rhs._format), _values(std::move(rhs._values)) {
        rhs._format = format::unknown;
    }

    /// <summary>
    /// Answer the values of the bounding box in linear memory.
    /// </summary>
    /// <returns>The values of the bounding box, or <c>nullptr</c> if the
    /// bounding box is not valid.</returns>
    inline _Ret_maybenull_ const void *data(void) const noexcept {
        return (*this) ? this->_values.data() : nullptr;
    }

    /// <summary>
    /// Answer whether the bounding box is empty.
    /// </summary>
    /// <returns><c>true</c> if the bounding box is all-zero.</returns>
    inline bool empty(void) const noexcept {
        return this->_values.empty() || std::all_of(this->_values.begin(),
            this->_values.end(),
            [](const byte_type v) { return (v == 0); });
    }

    /// <summary>
    /// Gets the format of the coordinates.
    /// </summary>
    /// <returns>The scalar format of the coordinates.</returns>
    inline format_type format(void) const noexcept {
        return this->_format;
    }

    /// <summary>
    /// Answer the value of the given coordinate.
    /// </summary>
    /// <typeparam name="TValue">The type of the value, which must be the
    /// type matching <see cref="format" />.</typeparam>
    /// <param name="v">The value to retrieve.</param>
    /// <returns>The requested value.</returns>
    /// <exception cref="std::logic_error">If <paramre name="TValue" /> is not
    /// the type used to represent <see cref="format" />, or if the bounding
    /// box is invalid.</exception>
    template<class TValue> TValue get(_In_ const value v) const;

    /// <summary>
    /// Answer the maximum coordinate along the given axis.
    /// </summary>
    /// <typeparam name="TValue">The type of the coordinate, which must be the
    /// type matching <see cref="format" />.</typeparam>
    /// <param name="axis">The zero-based index of the axi.</param>
    /// <returns>The maximum along <paramref name="axis" />.</returns>
    /// <exception cref="std::logic_error">If <paramre name="TValue" /> is not
    /// the type used to represent <see cref="format" />, or if the bounding
    /// box is invalid.</exception>
    /// <exception cref="std::out_of_range">If <paramref name="axis" />
    /// is not within [0, 2].</exception>
    template<class TValue> TValue (max)(_In_ std::size_t axis) const;

    /// <summary>
    /// Sets the maximum value along the given <paramref name="axis" /> to the
    /// specified <paramref name="value" />.
    /// </summary>
    /// <typeparam name="TValue">The type of the value to be set. If this is not
    /// the type of the elements of the bounding box, it will be cast to this
    /// type. which might cause loss of precision.</typeparam>
    /// <param name="axis">The zero-based index of the axis to set.</param>
    /// <param name="value">The value to be set.</param>
    /// <exception cref="std::logic_error">If the bounding box is invalid, i.e.
    /// has no components allocated, or if <paramref name="component" /> was
    /// forced to an invalid value.</exception>
    template<class TValue>
    void (max)(_In_ std::size_t axis, _In_ const TValue value);

    /// <summary>
    /// The maximum x-coordinate covered by the data.
    /// </summary>
    /// <typeparam name="TValue">The type of the coordinate, which must be the
    /// type matching <see cref="format" />.</typeparam>
    /// <returns>The maximum x-coordinate.</returns>
    /// <exception cref="std::logic_error">If <paramre name="TValue" /> is not
    /// the type used to represent <see cref="format" />, or if the bounding
    /// box is invalid.</exception>
    template<class TValue> inline TValue max_x(void) const {
        return this->get<TValue>(value::max_x);
    }

    /// <summary>
    /// The maximum y-coordinate covered by the data.
    /// </summary>
    /// <typeparam name="TValue">The type of the coordinate, which must be the
    /// type matching <see cref="format" />.</typeparam>
    /// <returns>The maximum y-coordinate.</returns>
    /// <exception cref="std::logic_error">If <paramre name="TValue" /> is not
    /// the type used to represent <see cref="format" />, or if the bounding
    /// box is invalid.</exception>
    template<class TValue> inline TValue max_y(void) const {
        return this->get<TValue>(value::max_y);
    }

    /// <summary>
    /// The maximum z-coordinate covered by the data.
    /// </summary>
    /// <typeparam name="TValue">The type of the coordinate, which must be the
    /// type matching <see cref="format" />.</typeparam>
    /// <returns>The maximum z-coordinate.</returns>
    /// <exception cref="std::logic_error">If <paramre name="TValue" /> is not
    /// the type used to represent <see cref="format" />, or if the bounding
    /// box is invalid.</exception>
    template<class TValue> inline TValue max_z(void) const {
        return this->get<TValue>(value::max_z);
    }

    /// <summary>
    /// Answer the minimum coordinate along the given axis.
    /// </summary>
    /// <typeparam name="TValue">The type of the coordinate, which must be the
    /// type matching <see cref="format" />.</typeparam>
    /// <param name="axis">The zero-based index of the axi.</param>
    /// <returns>The minimum along <paramref name="axis" />.</returns>
    /// <exception cref="std::logic_error">If <paramre name="TValue" /> is not
    /// the type used to represent <see cref="format" />, or if the bounding
    /// box is invalid.</exception>
    /// <exception cref="std::out_of_range">If <paramref name="axis" />
    /// is not within [0, 2].</exception>
    template<class TValue> TValue (min)(_In_ std::size_t axis) const;

    /// <summary>
    /// Sets the minimum value along the given <paramref name="axis" /> to the
    /// specified <paramref name="value" />.
    /// </summary>
    /// <typeparam name="TValue">The type of the value to be set. If this is not
    /// the type of the elements of the bounding box, it will be cast to this
    /// type. which might cause loss of precision.</typeparam>
    /// <param name="axis">The zero-based index of the axis to set.</param>
    /// <param name="value">The value to be set.</param>
    /// <exception cref="std::logic_error">If the bounding box is invalid, i.e.
    /// has no components allocated, or if <paramref name="component" /> was
    /// forced to an invalid value.</exception>
    template<class TValue>
    void (min)(_In_ std::size_t axis, _In_ const TValue value);

    /// <summary>
    /// The minimum x-coordinate covered by the data.
    /// </summary>
    /// <typeparam name="TValue">The type of the coordinate, which must be the
    /// type matching <see cref="format" />.</typeparam>
    /// <returns>The minimum x-coordinate.</returns>
    /// <exception cref="std::logic_error">If <paramre name="TValue" /> is not
    /// the type used to represent <see cref="format" />, or if the bounding
    /// box is invalid.</exception>
    template<class TValue> inline TValue min_x(void) const {
        return this->get<TValue>(value::min_x);
    }

    /// <summary>
    /// The minimum y-coordinate covered by the data.
    /// </summary>
    /// <typeparam name="TValue">The type of the coordinate, which must be the
    /// type matching <see cref="format" />.</typeparam>
    /// <returns>The minimum y-coordinate.</returns>
    /// <exception cref="std::logic_error">If <paramre name="TValue" /> is not
    /// the type used to represent <see cref="format" />, or if the bounding
    /// box is invalid.</exception>
    template<class TValue> inline TValue min_y(void) const {
        return this->get<TValue>(value::min_y);
    }

    /// <summary>
    /// The minimum z-coordinate covered by the data.
    /// </summary>
    /// <typeparam name="TValue">The type of the coordinate, which must be the
    /// type matching <see cref="format" />.</typeparam>
    /// <returns>The minimum z-coordinate.</returns>
    /// <exception cref="std::logic_error">If <paramre name="TValue" /> is not
    /// the type used to represent <see cref="format" />, or if the bounding
    /// box is invalid.</exception>
    template<class TValue> inline TValue min_z(void) const {
        return this->get<TValue>(value::min_z);
    }

    /// <summary>
    /// Sets the specified <paramref name="component" /> of the bounding box to
    /// the specified <paramref name="value" />.
    /// </summary>
    /// <typeparam name="TValue">The type of the value to be set. If this is not
    /// the type of the elements of the bounding box, it will be cast to this
    /// type. which might cause loss of precision.</typeparam>
    /// <param name="component">The component to be set.</param>
    /// <param name="value">The value to be set.</param>
    /// <exception cref="std::logic_error">If the bounding box is invalid, i.e.
    /// has no components allocated, or if <paramref name="component" /> was
    /// forced to an invalid value.</exception>
    template<class TValue>
    void set(_In_ const value component, _In_ const TValue value);

    /// <summary>
    /// Answer the size of the bounding box in bytes.
    /// </summary>
    /// <returns>The size of the bounding box in bytes.</returns>
    inline std::size_t size(void) const noexcept {
        return this->_values.size();
    }

    /// <summary>
    /// Extend this bounding box to include the given other bounding box.
    /// </summary>
    /// <typeparam name="TOther">The byte representation used by
    /// <paramref name="other" />.</typeparam>
    /// <param name="other">Another bounding box with the same format.</param>
    template<class TOther>
    inline void merge(_In_ const basic_bounding_box<TOther>& other);

    /// <summary>
    /// Assignment.
    /// </summary>
    /// <typeparam name="TOther">The byte type used by <paramref name="rhs" />.
    /// </typeparam>
    /// <param name="rhs">The right-hand-side operand.</param>
    /// <returns>*<see langword="this" />.</returns>
    template<class TOther>
    basic_bounding_box& operator =(_In_ const basic_bounding_box<TOther>& rhs);

    /// <summary>
    /// Move assignment.
    /// </summary>
    /// <param name="rhs">The right-hand-side operand.</param>
    /// <returns>*<see langword="this" />.</returns>
    basic_bounding_box& operator =(_Inout_ basic_bounding_box&& rhs) noexcept;

    /// <summary>
    /// Answer whether the bounding box is valid.
    /// </summary>
    /// <returns><c>true</c> if the bounding box has a valid format and valid
    /// data for that format, <c>false</c> otherwise.</returns>
    inline operator bool(void) const noexcept {
        return (to_components(this->_format) == 1)
            && (this->_values.size() == 6 * to_size(this->_format));
    }

    /// <summary>
    /// Answer the specified <see cref="value" />  of the bounding box.
    /// </summary>
    /// <param name="v">The value to retrieve.</param>
    /// <returns>A pointer to the begin of the requested coordinate,
    /// or <c>nullptr</c> if the bounding box is not valid.</returns>
    inline _Ret_maybenull_ const void *operator [](
            _In_ const value v) const noexcept {
        const auto offset = this->offset(v);
        const auto retval = offset.second
            ? this->_values.data() + offset.first
            : nullptr;
        assert((retval != nullptr) || !*this);
        return retval;
    }

private:

    /// <summary>
    /// Answer the offset of <paramref name="v" /> in bytes. The second
    /// component of the pair indicates whether the offset is valid.
    /// </summary>
    inline std::pair<std::size_t, bool> offset(
            _In_ const value v) const noexcept {
        const auto size = to_size(this->_format);
        const auto start = static_cast<std::size_t>(v) * size;
        const auto end = start + size;
        return std::make_pair(start, end <= this->_values.size());
    }

    std::vector<byte_type> _values;
    format_type _format;

    template<class T> friend class meta_data;

public:

    /// <summary>
    /// Answer an iterator for the first byte of the bounding box.
    /// </summary>
    /// <returns>An iterator for the begin of the in-memory representation of
    /// the bounding box.</returns>
    inline auto begin(void) const noexcept -> decltype(this->_values.begin()) {
        return this->_values.begin();
    }

    /// <summary>
    /// Answer an iterator past the last byte of the bounding box.
    /// </summary>
    /// <returns>An iterator for the end of the in-memory representation of
    /// the bounding box.</returns>
    inline auto end(void) const noexcept -> decltype(this->_values.end()) {
        return this->_values.end();
    }
};


/// <summary>
/// Representation of a 3D bounding box.
/// </summary>
typedef basic_bounding_box<> bounding_box;

FPD_NAMESPACE_END

#include "fpd/bounding_box.inl"

#endif /* !defined(_FPD_BOUNDING_BOX_H) */
