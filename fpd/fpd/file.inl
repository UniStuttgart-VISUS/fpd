// <copyright file="file.inl" company="Visualisierungsinstitut der Universität Stuttgart">
// Copyright © 2024 - 2025 Visualisierungsinstitut der Universität Stuttgart.
// Licensed under the MIT licence. See LICENCE file for details.
// </copyright>
// <author>Christoph Müller</author>


/*
 * FPD_NAMESPACE::file<TChar, THandle>::read
 */
template<class TChar, class THandle>
std::size_t FPD_NAMESPACE::file<TChar, THandle>::read(
        _Out_writes_all_(cnt) std::uint8_t *dst,
        _In_ const std::size_t cnt,
        _In_ std::size_t from) const {
    std::size_t retval = 0;
    auto rem = cnt;

    while (retval < cnt) {
        const auto read = io_type::read(dst + retval, rem,
            this->_io, from + retval);
        if (read == 0) {
            // EOF was reached.
            break;
        }

        retval += read;
        rem -= read;
    }

    return retval;
}


/*
 * FPD_NAMESPACE::file<TChar, THandle>::read_meta_data
 */
template<class TChar, class THandle>
void FPD_NAMESPACE::file<TChar, THandle>::read_meta_data(
        _In_ const std::size_t expected_header_size) {
    std::vector<std::uint8_t> header((std::max)(expected_header_size,
        meta_data_type::fixed_header_size));
    this->read(header.data(), header.size(), 0);

    auto flex_header_size = meta_data_type::check_header_size(this->_meta_data,
        header.data(), header.size());
    if (flex_header_size > header.size() - meta_data_type::fixed_header_size) {
        // The guess was bad, so we need to read the flexible headers in
        // an additional request.
        assert(header.size() >= meta_data_type::fixed_header_size);
        header.resize(flex_header_size);
        this->read(header.data(),
            header.size(),
            meta_data_type::fixed_header_size);
        meta_data_type::parse(this->_meta_data,
            header.data(),
            flex_header_size);

    } else {
        // The guess was good, so we can parse the flexible headers from
        // what we already have.
        meta_data_type::parse(this->_meta_data,
            header.data() + meta_data_type::fixed_header_size,
            flex_header_size);
    }
}
