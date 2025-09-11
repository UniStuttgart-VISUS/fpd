// <copyright file="compression_meta_data.h" company="Visualisierungsinstitut der Universität Stuttgart">
// Copyright © 2025 Visualisierungsinstitut der Universität Stuttgart.
// Licensed under the MIT licence. See LICENCE file for details.
// </copyright>
// <author>Christoph Müller</author>

#if !defined(_FPD_COMPRESSION_META_DATA_H)
#define _FPD_COMPRESSION_META_DATA_H
#pragma once

#include <stdexcept>
#include <vector>

#include "fpd/compression.h"


FPD_NAMESPACE_BEGIN

/// <summary>
/// The interface for compression meta data.
/// </summary>
class compression_meta_data_base {

public:

    /// <summary>
    /// Finalises the instance.
    /// </summary>
    virtual ~compression_meta_data_base(void) = default;

    /// <summary>
    /// Answer the compression method that has been used.
    /// </summary>
    /// <returns>The compression method.</returns>
    virtual compression method(void) const noexcept = 0;

protected:

    /// <summary>
    /// Initialises a new instance.
    /// </summary>
    compression_meta_data_base(void) = default;
};


/// <summary>
/// Implements the <see cref="compression_meta_data_base" /> interface. This
/// default implementation only carries the information about the compression
/// method. If additional data are required to decompress the data, a
/// specialised implementation of the meta data class is required.
/// </summary>
/// <typeparam name="Compression">The compression type the data are for.
/// </typeparam>
template<compression Compression>
class compression_meta_data final : public compression_meta_data_base {

public:

    /// <inheritdoc />
    compression method(void) const noexcept override {
        return Compression;
    }
};


/// <summary>
/// Specialisation for GDeflate.
/// </summary>
template<> class compression_meta_data<compression::gdeflate> final
        : public compression_meta_data_base {

public:

    /// <summary>
    /// Answer the total number of blocks.
    /// </summary>
    /// <returns>The total number of blocks.</returns>
    inline std::size_t blocks(void) const noexcept {
        return this->_blocks.size();
    }

    /// <summary>
    /// Answer the number of blocks per frame given the total number of frames.
    /// </summary>
    /// <param name="frames">The total number of frames in the file.</param>
    /// <returns>The number of blocks per frame.</returns>
    inline const std::size_t blocks(_In_ const std::size_t frames) const {
        this->check(frames);
        return (this->_blocks.size() / frames);
    }

    /// <summary>
    /// Checks whether the meta data are valid for the assumed number of
    /// <paramref name="frames" />. This means that all frames must be split
    /// split into the same number of blocks.
    /// </summary>
    /// <param name="frames">The total number of frames in the file.</param>
    inline void check(_In_ const std::size_t frames) const {
        if ((this->_blocks.size() % frames) != 0) {
            throw std::invalid_argument("GDeflate compression requires that "
                "all frames are split into an equal number of blocks. The "
                "compression meta data or the assumed number of frames are "
                "invalid as this is not the case for the given data.");
        }
    }

    /// <summary>
    /// Checks whether the meta data are valid for the assumed number of
    /// <paramref name="frames" /> in the file and number of element
    /// <paramref name="groups" /> in the format.
    /// </summary>
    /// <param name="frames">The total number of frames in the file.</param>
    /// <param name="groups">The number of element groups in the particle
    /// format description.</param>
    /// <returns>The number of blocks per frame, which must be computed to
    /// perform this check and might be useful for the caller.</returns>
    inline std::size_t check(_In_ const std::size_t frames,
            _In_ const std::size_t groups) const {
        this->check(frames);
        const auto retval = this->blocks(frames);
        if ((retval > 1) && ((retval % groups) != 0)) {
            throw std::invalid_argument("If a frame comprising multiple "
                "element groups is compressed using GDeflate, each of the "
                "groups must be compressed individually in order to access "
                "the data without decompressing the whole frame.");
        }
        return retval;
    }

    /// <inheritdoc />
    compression method(void) const noexcept override {
        return compression::gdeflate;
    }

    /// <summary>
    /// Answer the compressed size of the specified block in bytes.
    /// </summary>
    /// <param name="block">The zero-based index of the block to retrieve the
    /// size of..</param>
    /// <returns>The size of the specified block. Invalid blocks are reported as
    /// having a size of zero.</returns>
    inline std::uint32_t operator [](const std::size_t block) const noexcept {
        return (block < this->_blocks.size()) ? this->_blocks[block] : 0;
    }

    /// <summary>
    /// Exposes the vector of block sizes.
    /// </summary>
    /// <returns>The size of each block in bytes.</returns>
    inline operator const std::vector<std::uint32_t>&(void) const noexcept {
        return this->_blocks;
    }

private:

    std::vector<std::uint32_t> _blocks;

    template<class> friend class FPD_NAMESPACE::meta_data;
};


FPD_NAMESPACE_END

#endif /* !defined(_FPD_COMPRESSION_META_DATA_H) */
