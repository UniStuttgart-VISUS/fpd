// <copyright file="compression_meta_data.h" company="Visualisierungsinstitut der Universität Stuttgart">
// Copyright © 2025 Visualisierungsinstitut der Universität Stuttgart.
// Licensed under the MIT licence. See LICENCE file for details.
// </copyright>
// <author>Christoph Müller</author>

#if !defined(_FPD_COMPRESSION_META_DATA_H_)
#define _FPD_COMPRESSION_META_DATA_H_
#pragma once

#include <vector>

#include "fpd/compression.h"


FPD_NAMESPACE_BEGIN

/// <summary>
/// The interface for compression meta data.
/// </summary>
class compression_meta_data {

public:

    /// <summary>
    /// Finalises the instance.
    /// </summary>
    virtual ~compression_meta_data(void) = default;

    /// <summary>
    /// Answer the compression method that has been used.
    /// </summary>
    /// <returns>The compression method.</returns>
    virtual compression method(void) const noexcept = 0;

protected:

    /// <summary>
    /// Initialises a new instance.
    /// </summary>
    compression_meta_data(void) = default;
};

FPD_NAMESPACE_END


FPD_DETAIL_NAMESPACE_BEGIN

/// <summary>
/// Implements the <see cref="compression_meta_data" /> interface. This default
/// implementation only carries the information about the compression method. If
/// additional data are required to decompress the data, a specialised
/// implementation of the meta data class is required.
/// </summary>
/// <typeparam name="Compression">The compression type the data are for.
/// </typeparam>
template<compression Compression>
class compression_meta_data_impl final : public compression_meta_data {

public:

    /// <inheritdoc />
    compression method(void) const noexcept override {
        return Compression;
    }
};


/// <summary>
/// Specialisation for GDeflate.
/// </summary>
template<> class compression_meta_data_impl<compression::gdeflate> final
        : public compression_meta_data {

public:

    /// <summary>
    /// Answer the size of the individually compressed blocks in bytes.
    /// </summary>
    /// <returns>The sizes of the compressed blocks.</returns>
    inline const std::vector<std::uint32_t>& blocks(void) const noexcept {
        return this->_blocks;
    }

    /// <inheritdoc />
    compression method(void) const noexcept override {
        return compression::gdeflate;
    }

private:

    std::vector<std::uint32_t> _blocks;

    template<class> friend class FPD_NAMESPACE::meta_data;
};


FPD_DETAIL_NAMESPACE_END

#endif /* !defined(_FPD_COMPRESSION_META_DATA_H_) */
