// <copyright file="compression_meta_data.inl" company="Visualisierungsinstitut der Universität Stuttgart">
// Copyright © 2025 Visualisierungsinstitut der Universität Stuttgart.
// Licensed under the MIT licence. See LICENCE file for details.
// </copyright>
// <author>Christoph Müller</author>


/*
 * ...::compression_meta_data<compression::gdeflate>::calc_frame_offsets
 */
template<FPD_NAMESPACE::compression Compression, class TSize>
template<class TIterator>
void FPD_DETAIL_NAMESPACE::uniform_block_compression_meta_data<Compression,
        TSize>
::calc_frame_offsets(_In_ TIterator oit,
        _In_ const std::size_t frames,
        _In_ const std::size_t header_size) const {
    const auto blocks = this->blocks(frames);
    auto offset = header_size;

    for (std::size_t f = 0; f < frames; ++f) {
        *oit++ = offset;
        for (std::size_t b = 0; b < blocks; ++b) {
            offset += static_cast<std::size_t>(this->_blocks[f * blocks + b]);
        }
    }
}
