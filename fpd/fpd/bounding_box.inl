// <copyright file="bounding_box.inl" company="Visualisierungsinstitut der Universität Stuttgart">
// Copyright © 2023 - 2025 Visualisierungsinstitut der Universität Stuttgart.
// Licensed under the MIT licence. See LICENCE file for details.
// </copyright>
// <author>Christoph Müller</author>


/*
 * FPD_NAMESPACE::basic_bounding_box<TByte>::from_data
 */
template<class TByte>
FPD_NAMESPACE::basic_bounding_box<TByte>
FPD_NAMESPACE::basic_bounding_box<TByte>::from_data(
        _In_reads_(cnt * stride) const void *data,
        _In_ const std::size_t cnt,
        _In_ const std::size_t stride,
        _In_ const format_type format,
        _In_ const endian endian) {
    if ((data == nullptr) && (cnt > 0)) {
        throw std::invalid_argument("A valid pointer to particle positions "
            "must be provided.");
    }

    const auto comps = (std::min)(std::size_t(3), to_components(format));
    const auto size = to_size(format);
    if ((comps == 0) || (size == 0)) {
        throw std::invalid_argument("The specified format of the positional "
            "data is invalid.");
    }

    if (stride < size) {
        throw std::invalid_argument("The stride must be at least the size of "
            "a single position.");
    }

    return dispatch_format<basic_bounding_box>(format, [=](auto tag) {
        typedef std::remove_pointer_t<std::decay_t<decltype(tag)>> tag_type;
        typedef detail::format_reverse_traits<tag_type> traits;
        typedef typename traits::scalar value_type;

        // Initialise the bounding box empty on all axes.
        std::array<value_type, 6> bounds;
        std::fill(bounds.begin(), bounds.end(), static_cast<value_type>(0));

        // Determine the range on all valid axes.
        for (std::size_t c = 0; c < comps; ++c) {
            bounds[c] = (std::numeric_limits<value_type>::max)();
            bounds[c + 3] = (std::numeric_limits<value_type>::lowest)();
        }

        auto cur = static_cast<const byte_type *>(data);
        for (std::size_t i = 0; i < cnt; ++i) {
            const auto values = reinterpret_cast<const value_type *>(cur);

            for (std::size_t c = 0; c < comps; ++c) {
                const auto v = swap_endian_as<value_type>(values + c, endian);
                if (v < bounds[c]) {
                    bounds[c] = v;
                }
                if (v > bounds[c + 3]) {
                    bounds[c + 3] = v;
                }
            }

            cur += stride;
        }

        // Create the bounding box from the computed system-endian values and
        // swap the endian if the data were different.
        basic_bounding_box retval(detail::format_v<value_type>,
            reinterpret_cast<const byte_type *>(bounds.data()),
            bounds.size() * sizeof(value_type));
        swap_endian(reinterpret_cast<value_type *>(retval._values.data()),
            bounds.size(), endian);

        return retval;
    });
}


/*
 * FPD_NAMESPACE::basic_bounding_box<TByte>::basic_bounding_box
 */
template<class TByte>
template<class TValue>
FPD_NAMESPACE::basic_bounding_box<TByte>::basic_bounding_box(
        _In_ const TValue min_x, _In_ const TValue min_y,
        _In_ const TValue min_z, _In_ const TValue max_x,
        _In_ const TValue max_y, _In_ const TValue max_z)
        : _format(detail::format_v<TValue>) {
    static_assert(std::is_arithmetic<TValue>::value, "A bounding box must "
        "comprise only scalar arithmetic values.");
    this->_values.resize(6 * to_size(this->_format));
    assert(this->_values.size() == 6 * sizeof(TValue));
    assert(*this);

    auto values = reinterpret_cast<TValue *>(this->_values.data());
    values[static_cast<std::size_t>(value::min_x)] = (std::min)(min_x, max_x);
    values[static_cast<std::size_t>(value::min_y)] = (std::min)(min_y, max_y);
    values[static_cast<std::size_t>(value::min_z)] = (std::min)(min_z, max_z);
    values[static_cast<std::size_t>(value::max_x)] = (std::max)(min_x, max_x);
    values[static_cast<std::size_t>(value::max_y)] = (std::max)(min_y, max_y);
    values[static_cast<std::size_t>(value::max_z)] = (std::max)(min_z, max_z);
}


/*
 * FPD_NAMESPACE::basic_bounding_box<TByte>::basic_bounding_box
 */
template<class TByte>
template<class TInput>
FPD_NAMESPACE::basic_bounding_box<TByte>::basic_bounding_box(
        _In_ const std::enable_if_t<sizeof(TInput) == 1, format_type> format,
        _In_reads_bytes_(cnt) const TInput *values,
        _In_ std::size_t cnt,
        _In_ endian endian)
        : _format(format) {
    constexpr auto elements = 6;
    const auto size = to_size(this->_format);
    this->_values.resize(elements * size);

    if ((values == nullptr) || (cnt < this->_values.size())) {
        throw std::invalid_argument("The specified bounds are invalid.");
    }

    swap_endian_as(this->_values.data(),
        reinterpret_cast<const byte_type *>(values), elements, size,
        endian);
}


/*
 * FPD_NAMESPACE::basic_bounding_box<TByte>::basic_bounding_box
 */
template<class TByte>
template<class TOther>
FPD_NAMESPACE::basic_bounding_box<TByte>::basic_bounding_box(
        _In_ const basic_bounding_box<TOther> &rhs)
        : _format(rhs._format) {
    this->_values.resize(rhs._values.size());
    std::transform(rhs._values.begin(),
        rhs._values.end(),
        this->_values.begin(),
        [](const TOther o) { return static_cast<byte_type>(o); });
}


/*
 * FPD_NAMESPACE::basic_bounding_box<TByte>::get
 */
template<class TByte>
template<class TValue>
TValue FPD_NAMESPACE::basic_bounding_box<TByte>::get(_In_ const value v) const {
    if (detail::format_v<TValue> != this->_format) {
        throw std::logic_error("The requested format of the values does "
            "not match their actual format.");
    }

    auto retval = (*this)[v];
    if (retval == nullptr) {
        throw std::logic_error("The bounding box is not in a valid state.");
    }

    return *static_cast<const TValue *>(retval);
}


/*
 * FPD_NAMESPACE::basic_bounding_box<TByte>::longest_axis
 */
template<class TByte>
std::size_t FPD_NAMESPACE::basic_bounding_box<TByte>::longest_axis(
        void) const noexcept {
    if (this->_format == format::unknown) {
        return 0;
    }

    return detail::dispatch_format<std::size_t>(this->_format, [this](auto t) {
        typedef std::remove_pointer_t<std::decay_t<decltype(t)>> type;
        std::size_t retval = 0;
        assert(this->max<type>(retval) >= this->min<type>(retval));
        auto len = this->max<type>(retval) - this->min<type>(retval);

        for (std::size_t i = 1; i < 3; ++i) {
            assert(this->max<type>(i) >= this->min<type>(i));
            const auto l = this->max<type>(i) - this->min<type>(i);
            if (l > len) {
                len = l;
                retval = i;
            }
        }

        return retval;
    }, scalar_formats());
}


/*
 * FPD_NAMESPACE::basic_bounding_box<TByte>::max
 */
template<class TByte>
template<class TValue>
TValue(FPD_NAMESPACE::basic_bounding_box<TByte>::max)(
        _In_ std::size_t axis) const {
    if (axis >= 3) {
        throw std::out_of_range("The specified axis is out of range.");
    }

    axis += static_cast<std::size_t>(value::max_x);
    return this->get<TValue>(static_cast<value>(axis));
}


/*
 * FPD_NAMESPACE::basic_bounding_box<TByte>::max
 */
template<class TByte>
template<class TValue>
void (FPD_NAMESPACE::basic_bounding_box<TByte>::max)(_In_ std::size_t axis,
        _In_ const TValue value) {
    if (axis >= 3) {
        throw std::out_of_range("The specified axis is out of range.");
    }

    axis += static_cast<std::size_t>(value::max_x);
    this->set(static_cast<basic_bounding_box<TByte>::value>(axis), value);
}


/*
 * FPD_NAMESPACE::basic_bounding_box<TByte>::min
 */
template<class TByte>
template<class TValue>
TValue(FPD_NAMESPACE::basic_bounding_box<TByte>::min)(
        _In_ std::size_t axis) const {
    if (axis >= 3) {
        throw std::out_of_range("The specified axis is out of range.");
    }

    axis += static_cast<std::size_t>(value::min_x);
    return this->get<TValue>(static_cast<value>(axis));
}


/*
 * FPD_NAMESPACE::basic_bounding_box<TByte>::min
 */
template<class TByte>
template<class TValue>
void (FPD_NAMESPACE::basic_bounding_box<TByte>::min)(_In_ std::size_t axis,
        _In_ const TValue value) {
    if (axis >= 3) {
        throw std::out_of_range("The specified axis is out of range.");
    }

    axis += static_cast<std::size_t>(value::min_x);
    this->set(static_cast<basic_bounding_box<TByte>::value>(axis), value);
}


/*
 * FPD_NAMESPACE::basic_bounding_box<TByte>::set
 */
template<class TByte>
template<class TValue>
void FPD_NAMESPACE::basic_bounding_box<TByte>::set(_In_ const value component,
        _In_ const TValue value) {
    const auto offset = this->offset(component);
    if (!offset.second) {
        throw std::out_of_range("The specified component is out of range.");
    }

    void *dst = this->_values.data() + offset.first;

    if (detail::format_v<TValue> == this->_format) {
        *static_cast<TValue *>(dst) = value;

    } else {
        detail::dispatch_format<void>(this->_format, [dst, value](
                const auto tag) {
            typedef std::decay_t<decltype(tag)> pointer_type;
            typedef std::remove_pointer<pointer_type>::type value_type;
            *static_cast<pointer_type>(dst) = static_cast<value_type>(value);
        }, scalar_formats());
    }
}


/*
 * FPD_NAMESPACE::basic_bounding_box<TByte>::merge
 */
template<class TByte>
template<class TOther>
void FPD_NAMESPACE::basic_bounding_box<TByte>::merge(
        _In_ const basic_bounding_box<TOther>& other) {
    if (this->_format != other._format) {
        throw std::invalid_argument("Only bounding boxes of the same format "
            "can be merged.");
    }

    auto dst = this->_values.data();
    auto src = other._values.data();

    detail::dispatch_format<void>(this->_format, [dst, src](const auto tag) {
        typedef std::decay_t<decltype(tag)> pointer_type;
        typedef std::remove_pointer_t<pointer_type> value_type;
        auto that = reinterpret_cast<pointer_type>(dst);
        auto other = reinterpret_cast<const value_type *>(src);

        for (auto i = static_cast<std::size_t>(value::min_x);
                i <= static_cast<std::size_t>(value::min_z); ++i){
            if (other[i] < that[i]) {
                that[i] = other[i];
            }
        }

        for (auto i = static_cast<std::size_t>(value::max_x);
            i <= static_cast<std::size_t>(value::max_z); ++i) {
            if (other[i] > that[i]) {
                that[i] = other[i];
            }
        }
    }, scalar_formats());
}


/*
 * FPD_NAMESPACE::basic_bounding_box<TByte>::operator =
 */
template<class TByte>
FPD_NAMESPACE::basic_bounding_box<TByte>&
FPD_NAMESPACE::basic_bounding_box<TByte>::operator =(
        _In_ const basic_bounding_box& rhs) {
    if (this != std::addressof(rhs)) {
        this->_format = rhs._format;
        this->_values = rhs._values;
    }

    return *this;
}


/*
 * FPD_NAMESPACE::basic_bounding_box<TByte>::operator =
 */
template<class TByte>
template<class TOther>
FPD_NAMESPACE::basic_bounding_box<TByte>&
FPD_NAMESPACE::basic_bounding_box<TByte>::operator =(
        _In_ const basic_bounding_box<TOther>& rhs) {
    if (this != std::addressof(rhs)) {
        this->_format = rhs._format;
        this->_values.resize(rhs._values.size());
        std::transform(rhs._values.begin(),
            rhs._values.end(),
            this->_values.begin(),
            [](const TOther o) { return static_cast<byte_type>(o); });
    }

    return *this;
}


/*
 * FPD_NAMESPACE::basic_bounding_box<TByte>::operator =
 */
template<class TByte>
FPD_NAMESPACE::basic_bounding_box<TByte>&
FPD_NAMESPACE::basic_bounding_box<TByte>::operator =(
        _Inout_ basic_bounding_box&& rhs) noexcept {
    if (this != std::addressof(rhs)) {
        this->_format = rhs._format;
        rhs._format = format_type::unknown;
        this->_values = std::move(rhs._values);
    }

    return *this;
}
