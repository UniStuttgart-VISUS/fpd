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
    if (!std::is_arithmetic<TValue>::value
            || (to_components(this->_format) != 1)) {
        throw std::logic_error("A bounding box must comprise only scalar "
            "arithmetic values.");
    }

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
