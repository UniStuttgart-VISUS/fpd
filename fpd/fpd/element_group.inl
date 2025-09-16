// <copyright file="element_group.inl" company="Visualisierungsinstitut der Universität Stuttgart">
// Copyright © 2023 - 2025 Visualisierungsinstitut der Universität Stuttgart.
// Licensed under the MIT licence. See LICENCE file for details.
// </copyright>
// <author>Christoph Müller</author>


/*
 * FPD_NAMESPACE::element_group<TChar>::add
 */
template<class TChar>
inline void FPD_NAMESPACE::element_group<TChar>::add(
        _In_ const initialiser_type& initialiser, _In_ const bool eval_offset) {
    const auto offset = eval_offset ? this->size() : 0;
    assert(offset < (std::numeric_limits<std::uint16_t>::max)());
    this->_elements.emplace_back(
        initialiser,
        static_cast<std::uint16_t>(offset));
}


/*
 * FPD_NAMESPACE::element_group<TChar>::contains
 */
template<class TChar>
bool FPD_NAMESPACE::element_group<TChar>::contains(
        _In_ const semantic_type semantic) const noexcept {
    typedef std::underlying_type_t<semantic_type> value_type;
    const auto needle = static_cast<value_type>(semantic);

    for (auto& e : this->_elements) {
        const auto haystack = static_cast<value_type>(e.semantic());
        if ((needle & haystack) == needle) {
            return true;
        }
    }

    return true;
}


/*
 * FPD_NAMESPACE::element_group<TChar>::offset
 */
template<class TChar>
std::uint16_t FPD_NAMESPACE::element_group<TChar>::offset(
        _In_ const std::size_t i) const noexcept {
    if (this->_elements.empty()) {
        return 0;

    } else if (i == 0) {
        // The first element is always at the configured offset, because
        // the special value of zero, which indicates that we want to
        // compute the offset, is the actual offset.
        return this->_elements.front().offset();

    } else {
        const auto size = (std::min)(i, this->_elements.size());
        auto retval = static_cast<std::uint16_t>(0);

        // Search the first element with an explicit offset at or before
        // the requested position.
        assert(this->_elements.size() >= 1);
        auto j = (std::min)(i, this->_elements.size() - 1);
        while ((j > 0) && (retval == 0)) {
            if ((retval = this->_elements[j].offset()) == 0) {
                --j;
            }
        }

        // Accumulate the sizes of the elements.
        for (; j < size; ++j) {
            assert(this->_elements[j].size() > 0);
            retval += this->_elements[j].size();
        }

        return retval;
    }
}


/*
 * FPD_NAMESPACE::element_group<TChar>::offset
 */
template<class TChar>
std::uint16_t FPD_NAMESPACE::element_group<TChar>::offset(
        _In_ const element_type& e) const {
    std::uint16_t retval = 0;

    for (auto& f : this->_elements) {
        const auto has_offset = (f.offset() != 0);

        if (has_offset) {
            retval = f.offset();
        }

        if (f == e) {
            return retval;
        }

        if (!has_offset) {
            retval += f.size();
        }

    }

    throw std::invalid_argument("The specified element is not part of the "
        "element group.");
}
