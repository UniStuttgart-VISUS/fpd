// <copyright file="element_group.inl" company="Visualisierungsinstitut der Universität Stuttgart">
// Copyright © 2023 Visualisierungsinstitut der Universität Stuttgart.
// Licensed under the MIT licence. See LICENCE file for details.
// </copyright>
// <author>Christoph Müller</author>


/*
 * FPD_NAMESPACE::element_group<TChar>::size
 */
template<class TChar>
std::uint16_t FPD_NAMESPACE::element_group<TChar>::size(void) const noexcept {
    auto retval = static_cast<std::uint16_t>(0);
    auto i = this->_elements.empty()
        ? this->_elements.size()
        : this->_elements.size() - 1;

    // Search the first element with an explicit offset.
    while ((i > 0) && (retval == 0)) {
        if ((retval = this->_elements[i].offset()) == 0) {
            --i;
        }
    }

    // Accumulate the sizes of the elements.
    for (; i < this->_elements.size(); ++i) {
        assert(this->_elements[i].size() > 0);
        retval += this->_elements[i].size();
    }

    return retval;
}
