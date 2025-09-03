// <copyright file="bounding_box.inl" company="Visualisierungsinstitut der Universität Stuttgart">
// Copyright © 2023 - 2025 Visualisierungsinstitut der Universität Stuttgart.
// Licensed under the MIT licence. See LICENCE file for details.
// </copyright>
// <author>Christoph Müller</author>



/*
 * FPD_NAMESPACE::bounding_box::bounding_box
 */
template<class TValue>
FPD_NAMESPACE::bounding_box::bounding_box(
        _In_ const TValue min_x, _In_ const TValue min_y,
        _In_ const TValue min_z, _In_ const TValue max_x,
        _In_ const TValue max_y, _In_ const TValue max_z)
        : _format(detail::format_v<TValue>) {
    static_assert(std::is_arithmetic<TValue>::value, "A bounding box must "
        "comprise only scalar arithmetic values.");
    this->_values.resize(6 * to_size(this->_format));
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
 * FPD_NAMESPACE::bounding_box::bounding_box
 */
template<class TByte>
FPD_NAMESPACE::bounding_box::bounding_box(
        _In_ const std::enable_if_t<sizeof(TByte) == 1, format_type> format,
        _In_reads_bytes_(cnt) const TByte *values,
        _In_ std::size_t cnt,
        endian endian)
        : _format(format) {
    const auto size = to_size(this->_format);
    this->_values.resize(6 * size);

    if ((values == nullptr) || (cnt < this->_values.size())) {
        throw std::invalid_argument("The specified bounds are invalid.");
    }

    swap_endian_as(this->_values.data(),
        reinterpret_cast<const std::uint8_t *>(values),
        this->_values.size(), size,
        endian);
}


/*
 * FPD_NAMESPACE::bounding_box::get
 */
template<class TValue>
TValue FPD_NAMESPACE::bounding_box::get(_In_ const value v) const {
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
 * FPD_NAMESPACE::bounding_box::max
 */
template<class TValue>
TValue(FPD_NAMESPACE::bounding_box::max)(_In_ std::size_t axis) const {
    if (axis >= 3) {
        throw std::out_of_range("The specified axis is out of range.");
    }

    axis += static_cast<std::size_t>(value::max_x);
    return this->get<TValue>(static_cast<value>(axis));
}


/*
 * FPD_NAMESPACE::bounding_box::max
 */
template<class TValue>
void (FPD_NAMESPACE::bounding_box::max)(_In_ std::size_t axis,
        _In_ const TValue value) {
    if (axis >= 3) {
        throw std::out_of_range("The specified axis is out of range.");
    }

    axis += static_cast<std::size_t>(value::max_x);
    this->set(static_cast<bounding_box::value>(axis), value);
}


/*
 * FPD_NAMESPACE::bounding_box::min
 */
template<class TValue>
TValue(FPD_NAMESPACE::bounding_box::min)(_In_ std::size_t axis) const {
    if (axis >= 3) {
        throw std::out_of_range("The specified axis is out of range.");
    }

    axis += static_cast<std::size_t>(value::min_x);
    return this->get<TValue>(static_cast<value>(axis));
}

/*
 * FPD_NAMESPACE::bounding_box::min
 */
template<class TValue>
void (FPD_NAMESPACE::bounding_box::min)(_In_ std::size_t axis,
        _In_ const TValue value) {
    if (axis >= 3) {
        throw std::out_of_range("The specified axis is out of range.");
    }

    axis += static_cast<std::size_t>(value::min_x);
    this->set(static_cast<bounding_box::value>(axis), value);
}



/*
 * FPD_NAMESPACE::bounding_box::set
 */
template<class TValue>
void FPD_NAMESPACE::bounding_box::set(_In_ const value component,
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
 * FPD_NAMESPACE::bounding_box::merge
 */
void FPD_NAMESPACE::bounding_box::merge(_In_ const bounding_box& other) {
    if (this->_format != other._format) {
        throw std::invalid_argument("Only bounding boxes of the same format "
            "can be merged.");
    }

    auto dst = this->_values.data();
    auto src = other._values.data();

    detail::dispatch_format<void>(this->_format, [dst, src](const auto tag) {
        typedef std::decay_t<decltype(tag)> pointer_type;
        typedef std::remove_pointer<pointer_type>::type value_type;
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
