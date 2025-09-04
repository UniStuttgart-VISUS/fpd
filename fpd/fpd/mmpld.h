// <copyright file="mmpld.h" company="Visualisierungsinstitut der Universität Stuttgart">
// Copyright © 2025 Visualisierungsinstitut der Universität Stuttgart.
// Licensed under the MIT licence. See LICENCE file for details.
// </copyright>
// <author>Christoph Müller</author>

#if !defined(_FPD_MMPLD_H)
#define _FPD_MMPLD_H
#pragma once

#if defined(FPD_WITH_MMPLD)

#include <cstdlib>
#include <limits>
#include <set>

#include <mmpld.h>

#include "fpd/element.h"


FPD_NAMESPACE_BEGIN

/// <summary>
/// Produces the (distinct) FPD element groups corresponding to the particle
/// list headers in the given range.
/// </summary>
/// <typeparam name="TOut">An output iterator type over
/// <see cref="element_group" />s.</typeparam>
/// <typeparam name="TIn">An input iterator type over MMPLD list headers.
/// </typeparam>
/// <param name="oit">The output iterator which will receive an element group
/// for each distinct list format.</param>
/// <param name="begin">The begin of the range of MMPLD list headers.</param>
/// <param name="end">The end of the range of MMPLD list headers.</param>
/// <returns>The number of element groups that have been emitted to
/// <paramref name="oit" />.</returns>
template<class TOut, class TIn>
std::size_t mmpld_formats(_In_ TOut oit, _In_ const TIn begin,
    _In_ const TIn end);

/// <summary>
/// Converts an MMPLD colour type to the corresponding FPD format.
/// </summary>
/// <param name="colour">The MMPLD colour type to convert.</param>
/// <returns>The corresponding FPD format, or <see cref="format::unknown" />
/// if no corresponding format could be found.</returns>
inline format mmpld_format(_In_ const mmpld::colour_type colour) noexcept;


/// <summary>
/// Converts an MMPLD vertex type to the corresponding FPD format.
/// </summary>
/// <param name="vertex">The MMPLD vertex type to convert.</param>
/// <returns>The corresponding FPD format, or <see cref="format::unknown" />
/// if no corresponding format could be found.</returns>
inline format mmpld_format(_In_ const mmpld::vertex_type vertex) noexcept;


/// <summary>
/// Determines the semantic of a MMPLD colour specification.
/// </summary>
/// <param name="colour">The MMPLD colour type to convert.</param>
/// <returns>The FPD semantic of the given colour type.</returns>
inline semantic mmpld_semantic(_In_ const mmpld::colour_type colour) noexcept;


/// <summary>
/// Determines the semantic of a MMPLD vertex type.
/// </summary>
/// <param name="vertex">The MMPLD vertex type to convert.</param>
/// <returns>The FPD semantic of the given vertex type.</returns>
inline semantic mmpld_semantic(_In_ const mmpld::vertex_type vertex) noexcept;

FPD_NAMESPACE_END

#include "fpd/mmpld.inl"

#endif /* defined(FPD_WITH_MMPLD) */
#endif /* !defined(_FPD_MMPLD_H) */
