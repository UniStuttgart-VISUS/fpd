// <copyright file="bounding_box.h" company="Visualisierungsinstitut der Universität Stuttgart">
// Copyright © 2023 - 2025 Visualisierungsinstitut der Universität Stuttgart.
// Licensed under the MIT licence. See LICENCE file for details.
// </copyright>
// <author>Christoph Müller</author>

#if !defined(_FPD_BOUNDING_BOX_H_)
#define _FPD_BOUNDING_BOX_H_
#pragma once

#include <algorithm>
#include <cassert>
#include <cstring>
#include <stdexcept>
#include <type_traits>
#include <vector>

#include "fpd/annotations.h"
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
class bounding_box final {

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
    /// The type used to specify the format of the components.
    /// </summary>
    typedef FPD_NAMESPACE::format format_type;

    /// <summary>
    /// Initialises a new instance.
    /// </summary>
    inline bounding_box(void) noexcept : _format(format_type::unknown) { }

    /// <summary>
    /// Initialises an empty bounding box of the given format.
    /// </summary>
    /// <param name="format">The scalar format of the bounding box.</param>
    explicit inline bounding_box(_In_ const format_type format)
            : _format(format) {
        this->_values.resize(6 * to_size(this->_format));
        assert(*this);
        std::fill(this->_values.begin(), this->_values.end(), 0);
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
    template<class TValue>
    bounding_box(_In_ const TValue min_x, _In_ const TValue min_y,
        _In_ const TValue min_z, _In_ const TValue max_x,
        _In_ const TValue max_y, _In_ const TValue max_z);

    /// <summary>
    /// Initialises a new instance.
    /// </summary>
    /// <param name="format">The scalar format of the bounding box.</param>
    /// <param name="values">A pointer to at least six values of the specified
    /// format.</param>
    /// <param name="cnt">The size of the buffer designated by
    /// <paramref name="values" />, in bytes.</param>
    inline bounding_box(_In_ const format_type format,
            _In_reads_bytes_(cnt)  const void *values,
            _In_ const std::size_t cnt)
            : _format(format) {
        this->_values.resize(6 * to_size(this->_format));
        if ((values == nullptr) || (cnt < this->_values.size())) {
            throw std::invalid_argument("The specified bounds are invalid.");
        }
        ::memcpy(this->_values.data(), values, this->_values.size());
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
            [](const std::uint8_t v) { return (v == 0); });
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
    /// Answer the size of the bounding box in bytes.
    /// </summary>
    /// <returns>The size of the bounding box in bytes.</returns>
    inline std::size_t size(void) const noexcept {
        return this->_values.size();
    }

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
        const auto size = to_size(this->_format);
        const auto offset = static_cast<std::size_t>(v) * size;
        const auto retval = (offset + size <= this->_values.size())
            ? this->_values.data() + offset
            : nullptr;
        assert((retval != nullptr) || !*this);
        return retval;
    }

private:

    std::vector<std::uint8_t> _values;
    format_type _format;

    template<class T> friend class meta_data;
};

FPD_NAMESPACE_END

#include "fpd/bounding_box.inl"

#endif /* !defined(_FPD_BOUNDING_BOX_H_) */
