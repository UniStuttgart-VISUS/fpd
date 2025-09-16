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

FPD_NAMESPACE_END


FPD_DETAIL_NAMESPACE_BEGIN

/// <summary>
/// Base implementation of compression methods that split each frame into the
/// same number of blocks.
/// </summary>
/// <typeparam name="Compression">The compression type the data are for.
/// </typeparam>
/// <typeparam name="TSize">The type used to represent the size of each block.
/// </typeparam>
template<compression Compression, class TSize = std::uint32_t>
class uniform_block_compression_meta_data : public compression_meta_data_base {

public:

    /// <summary>
    /// The type used to represent the size of each block.
    /// </summary>
    typedef TSize value_type;

    /// <summary>
    /// Initialises a new instance.
    /// </summary>
    inline uniform_block_compression_meta_data(void) noexcept = default;

    /// <summary>
    /// Initialises a new instance.
    /// </summary>
    /// <param name="blocks">The sizes of the compressed blocks.</param>
    explicit inline uniform_block_compression_meta_data(
            _In_ const std::vector<value_type>& blocks)
        : _blocks(blocks) { }

    /// <summary>
    /// Initialises a new instance.
    /// </summary>
    /// <param name="blocks">The sizes of the compressed blocks.</param>
    explicit inline uniform_block_compression_meta_data(
            _In_ std::vector<value_type>&& blocks) noexcept
        : _blocks(std::move(blocks)) { }

    /// <summary>
    /// Initialises a new instance.
    /// </summary>
    /// <typeparam name="TIterator">An iterator over unsigned integer that
    /// specify the size of each compressed block.</typeparam>
    /// <param name="begin">The begin of the range of block sizes.</param>
    /// <param name="end">The end of the range of block sizes.</param>
    template<class TIterator>
    inline uniform_block_compression_meta_data(
            _In_ const TIterator begin,
            _In_ const TIterator end)
        : _blocks(begin, end) { }

    /// <summary>
    /// Initialises a new instance.
    /// </summary>
    /// <remarks>
    /// This constructor can be used to reserve a header block that is to be
    /// patched later with the actual sizes once the compressed blocks have been
    /// written.
    /// </remarks>
    /// <param name="blocks">The total number of compressed blocks, which are
    /// all initialised to a size of zero.</param>
    /// <param name="size">The size to initialise each block to. This parameter
    /// defaults to zero.</param>
    explicit inline uniform_block_compression_meta_data(
            _In_ const std::size_t blocks,
            _In_ const value_type size = 0)
        : _blocks(blocks, size) { }

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
            throw std::invalid_argument("The compression method requires that "
                "all frames are split into an equal number of blocks. The "
                "compression meta data or the assumed number of frames are "
                "invalid as this is not the case for the given data.");
        }
    }

    /// <summary>
    /// Computes the locations where each frame begins assuming the specified
    /// number of <paramref name="frames" /> and the specified size of the file
    /// header.
    /// </summary>
    /// <typeparam name="TIterator"></typeparam>
    /// <param name="oit"></param>
    /// <param name="frames"></param>
    /// <param name="header_size"></param>
    template<class TIterator> void calc_frame_offsets(_In_ TIterator oit,
        _In_ const std::size_t frames,
        _In_ const std::size_t header_size) const;

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
                "element groups is compressed, each of the groups must be "
                "compressed individually in order to access the data without "
                "decompressing the whole frame.");
        }
        return retval;
    }

    /// <inheritdoc />
    compression method(void) const noexcept override {
        return Compression;
    }

    /// <summary>
    /// Answer the compressed size of the specified block in bytes.
    /// </summary>
    /// <param name="block">The zero-based index of the block to retrieve the
    /// size of..</param>
    /// <returns>The size of the specified block. Invalid blocks are reported as
    /// having a size of zero.</returns>
    inline value_type operator [](const std::size_t block) const noexcept {
        return (block < this->_blocks.size()) ? this->_blocks[block] : 0;
    }

    /// <summary>
    /// Exposes the vector of block sizes.
    /// </summary>
    /// <returns>The size of each block in bytes.</returns>
    inline operator const std::vector<value_type>& (void) const noexcept {
        return this->_blocks;
    }

private:

    std::vector<value_type> _blocks;

    template<class> friend class FPD_NAMESPACE::meta_data;
};

FPD_DETAIL_NAMESPACE_END


FPD_NAMESPACE_BEGIN

/// <summary>
/// Specialisation for GDeflate.
/// </summary>
template<>
class compression_meta_data<compression::gdeflate> final
        : public detail::uniform_block_compression_meta_data<
        compression::gdeflate> {

    typedef detail::uniform_block_compression_meta_data<compression::gdeflate>
        base;

public:

    /// <summary>
    /// Initialises a new instance.
    /// </summary>
    /// <typeparam name="TArgs">The types of the arguments to be passed to the
    /// base class constructor.</typeparam>
    /// <param name="args">The arguments to be passed to the base class
    /// constructor.</param>
    template<class... TArgs> inline compression_meta_data(TArgs&&... args)
        : base(std::forward<TArgs>(args)...) { }

};

FPD_NAMESPACE_END

#include "fpd/compression_meta_data.inl"

#endif /* !defined(_FPD_COMPRESSION_META_DATA_H) */
