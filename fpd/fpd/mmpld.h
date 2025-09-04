// <copyright file="mmpld.h" company="Visualisierungsinstitut der Universität Stuttgart">
// Copyright © 2025 Visualisierungsinstitut der Universität Stuttgart.
// Licensed under the MIT licence. See LICENCE file for details.
// </copyright>
// <author>Christoph Müller</author>

#if !defined(_FPD_MMPLD_H)
#define _FPD_MMPLD_H
#pragma once

#if defined(FPD_WITH_MMPLD)

#include <algorithm>
#include <cstdlib>
#include <limits>
#include <set>
#include <stdexcept>
#include <vector>

#include <mmpld.h>

#include "fpd/format_description.h"


FPD_NAMESPACE_BEGIN

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
/// Determines the FPD format that is able to hold all information in any of
/// the MMPLD list headers in the given range.
/// </summary>
/// <remarks>
/// <para>The format computed here prefers per-particle data whenever it is
/// available in any of the lists. For example, if one list has per-vertex
/// radius, but the others do not, the resulting format will have a per-vertex
/// radius. The format computed here always uses the widest type found for any
/// of the data and it will prefer floating-point formats over integral ones.
/// That is, an integral format will only be emitted if none of the input lists
/// uses a floating-point format for the same data.</para>
/// </remarks>
/// <typeparam name="TChar">The character type used by the elememt group being
/// generated.</typeparam>
/// <typeparam name="TIterator">An input iterator type over MMPLD list headers.
/// </typeparam>
/// <param name="begin">The begin of the range of MMPLD list headers.</param>
/// <param name="end">The end of the range of MMPLD list headers.</param>
/// <returns>An description of the FPD format that is best suited to represent
/// all data in the given range of MMPLD lists.</returns>
template<class TChar, class TIterator>
format_description<TChar> mmpld_format(_In_ const TIterator begin,
    _In_ const TIterator end);


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
/// Create an MMPLD list header for the given FPD format. In case of success,
/// the MMPLD list header will represent the same in-memory layout as the input
/// format. The function will fail if the input format cannot be represented
/// by means of the MMPLD format specification.
/// </summary>
/// <typeparam name="TChar">The character type used in the format description.
/// </typeparam>
/// <param name="format">The format description.</param>
/// <returns>A list header for the given format.</returns>
/// <exception cref="std::invalid_argument">If the input format cannot be
/// converted to MMPLD.</exception>
template<class TChar>
mmpld::list_header mmpld_list(_In_ const format_description<TChar>& format);


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
