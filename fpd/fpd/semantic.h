// <copyright file="semantic.h" company="Visualisierungsinstitut der Universität Stuttgart">
// Copyright © 2023 Visualisierungsinstitut der Universität Stuttgart.
// Licensed under the MIT licence. See LICENCE file for details.
// </copyright>
// <author>Christoph Müller</author>

#pragma once

#include <cinttypes>

#include "fpd/abi.h"


FPD_NAMESPACE_BEGIN

/// <summary>
/// Defines possible semantics of a vector or scalar.
/// </summary>
enum class semantic : std::uint64_t {

    /// <summary>
    /// Identifies the position of a particle in 3D space.
    /// </summary>
    position = 0x00000001,

    /// <summary>
    /// Identifies the radius of a particle.
    /// </summary>
    radius = 0x00000002,

    /// <summary>
    /// Identifies a vector holding position and radius at the same time. The
    /// position is assumed to be the first three components whereas the radius
    /// is the last.
    /// </summary>
    position_radius = static_cast<std::uint64_t>(position)
        | static_cast<std::uint64_t>(radius),

    /// <summary>
    /// Identifies a user-defined scalar value that eventually should be mapped
    /// to the colour of the particle.
    /// </summary>
    intensity = 0x00000004,

    /// <summary>
    /// Identifies the colour of the particle.
    /// </summary>
    colour = 0x0000000C,

    /// <summary>
    /// Idenfifies the velocity of the particle in 3D space.
    /// </summary>
    velocity = 0x00000010,

    /// <summary>
    /// The first user-defined semantic, which cannot be processed by the
    /// library itself, but only by the application defining it.
    /// </summary>
    /// <remarks>
    /// Applications are free to define user-defined semantics as they want, but
    /// they need to provide a textual description in the header of the file for
    /// it. User-defined semantics must use consecutive indices starting at the
    /// value defined by <see cref="user_defined" />.
    /// </remarks>
    user_defined = 0x100000000
};

FPD_NAMESPACE_END
