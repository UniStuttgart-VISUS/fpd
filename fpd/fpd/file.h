// <copyright file="file.h" company="Visualisierungsinstitut der Universität Stuttgart">
// Copyright © 2024 Visualisierungsinstitut der Universität Stuttgart.
// Licensed under the MIT licence. See LICENCE file for details.
// </copyright>
// <author>Christoph Müller</author>

#pragma once

#include <cinttypes>
#include <utility>

#include "fpd/meta_data.h"


FPD_NAMESPACE_BEGIN

/// <summary>
/// This class represents an FDP file.
/// </summary>
/// <typeparam name="TChar">The type of a character used for strings in the
/// meta data.</typeparam>
/// <typeparam name="TIoTraits">The I/O traits used to retrieve data from disk.
/// </typeparam>
template<class TChar, class TIoTraits> class file final {

public:

    /// <summary>
    /// The type of a character used for strings in the meta data.
    /// </summary>
    typedef TChar char_type;

    /// <summary>
    /// The type of the meta data read from the file.
    /// </summary>
    typedef meta_data<char_type> meta_data_type;

    /// <summary>
    /// The type of the I/O API.
    /// </summary>
    typedef TIoTraits io_type;

    /// <summary>
    /// Initialises a new instance.
    /// </summary>
    /// <typeparam name="TArgs">The type of the arguments passed to the I/O
    /// constructor.</typeparam>
    /// <param name="args">The arguments passed to the I/O constructors. This
    /// depends on <see cref="io_type" /> and its constructor.</param>
    template<class... TArgs> explicit inline file(_In_ TArgs&&... args)
            : _io(std::forward<TArgs>(args)...) { }

private:

    io_type _io;
    meta_data_type _meta_data;
};

FPD_NAMESPACE_END
