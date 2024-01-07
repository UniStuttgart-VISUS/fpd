// <copyright file="semantic.h" company="Visualisierungsinstitut der Universität Stuttgart">
// Copyright © 2023 Visualisierungsinstitut der Universität Stuttgart.
// Licensed under the MIT licence. See LICENCE file for details.
// </copyright>
// <author>Christoph Müller</author>

#pragma once

#include <cinttypes>
#include <string>
#include <type_traits>

#include "fpd/annotations.h"
#include "fpd/literal.h"


FPD_NAMESPACE_BEGIN

/// <summary>
/// Defines possible semantics of a vector or scalar.
/// </summary>
/// <remarks>
/// <para>Do not forget to update <see cref="semantics" /> below when adding
/// new elements to this enumeration.</para>
/// <para>Do not forget to update <see cref="to_string" /> below when adding
/// new elements to this enumeration.</para>
/// </remarks>
enum class semantic : std::uint64_t {

    /// <summary>
    /// The data has no semantic, which means that it can be discarded.
    /// </summary>
    unknown = 0x00000000,

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


/// <summary>
/// A compile-time list of semantics.
/// </summary>
/// <typeparam name="Semantics">The semantics in the list.</typeparam>
template<semantic... Semantics> struct semantic_list final { };


/// <summary>
/// A compile-time list of all built-in semantics.
/// </summary>
typedef semantic_list<
    semantic::unknown,
    semantic::position,
    semantic::position_radius,
    semantic::intensity,
    semantic::colour,
    semantic::velocity> semantics;


/// <summary>
/// Answer a human-readable version of the given <paramref name="semantic" />.
/// </summary>
/// <typeparam name="TChar">The character type of the string to create.
/// </typeparam>
/// <param name="semantic">The semantic to convert to a string.</param>
/// <returns>The string representation of the given semantic.</returns>
template<class TChar>
const TChar *const to_string(_In_ const semantic semantic);


/// <summary>
/// Creates the <paramref name="index" />th user-defined semantic.
/// </summary>
/// <param name="index">The zero-based index of a user-defined semantic.</param>
/// <returns>The requested user-defined semantic.</returns>
constexpr inline semantic user_defined_semantic(
        _In_ const std::uint32_t index) {
    typedef std::underlying_type<semantic>::type int_type;
    return static_cast<semantic>(
        static_cast<int_type>(semantic::user_defined)
        + static_cast<int_type>(index));
}

FPD_NAMESPACE_END

#include "fpd/semantic.inl"
