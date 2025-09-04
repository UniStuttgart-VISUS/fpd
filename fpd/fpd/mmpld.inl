// <copyright file="mmpld.inl" company="Visualisierungsinstitut der Universität Stuttgart">
// Copyright © 2025 Visualisierungsinstitut der Universität Stuttgart.
// Licensed under the MIT licence. See LICENCE file for details.
// </copyright>
// <author>Christoph Müller</author>


/*
 * FPD_NAMESPACE::mmpld_format
 */
FPD_NAMESPACE::format FPD_NAMESPACE::mmpld_format(
        _In_ const mmpld::colour_type colour) noexcept {
    switch (colour) {
        case mmpld::colour_type::rgb8: return format::uint8xyz;
        case mmpld::colour_type::rgba8: return format::uint8xyzw;
        case mmpld::colour_type::intensity: return format::float32;
        case mmpld::colour_type::rgb32: return format::float32xyz;
        case mmpld::colour_type::rgba32: return format::float32xyzw;
        case mmpld::colour_type::rgba16: return format::uint16xyzw;
        case mmpld::colour_type::intensity64: return format::float64;
        default: return format::unknown;
    }
}


/*
 * FPD_NAMESPACE::mmpld_format
 */
FPD_NAMESPACE::format FPD_NAMESPACE::mmpld_format(
        _In_ const mmpld::vertex_type vertex) noexcept {
    switch (vertex) {
        case mmpld::vertex_type::float_xyz: return format::float32xyz;
        case mmpld::vertex_type::float_xyzr: return format::float32xyzw;
        case mmpld::vertex_type::short_xyz: return format::int16xyz;
        case mmpld::vertex_type::double_xyz: return format::float64xyz;
        default: return format::unknown;
    }
}


/*
 * FPD_NAMESPACE::mmpld_format
 */
template<class TChar, class TIterator>
FPD_NAMESPACE::element_group<TChar> FPD_NAMESPACE::mmpld_format(
        _In_ const TIterator begin,
        _In_ const TIterator end) {
    typedef element_group<TChar> group_type;
    typedef typename group_type::element_type element_type;
    std::vector<group_type> candidates;

    // Determine all formats present in the input lists.
    if (mmpld_formats(std::back_inserter(candidates), begin, end) < 1) {
        throw std::invalid_argument("The given range of particle lists does "
            "not contain any valid data.");
    }

    // Sort the candidates such that the ones with more elements are preferred.
    auto it = std::max_element(candidates.begin(), candidates.end(),
        [](const group_type& lhs, const group_type& rhs) {
            return (lhs.count() < rhs.count());
        });

    // As the candidates are immutable, we now copy the elements of our first
    // best candidate to 'elements' in which we will accumulate the largest
    // type we must be able to support. The rationale for removing all other
    // data from the elements is that we do not need them as MMLPD never has
    // user-defined semantics and we must re-evaluate all offsets anyway after
    // changing data types.
    std::vector<std::pair<semantic, format>> formats(it->count());
    std::transform(it->begin(),
        it->end(),
        formats.begin(),
        [](const element_type& e) {
            return std::make_pair(e.semantic(), e.format());
        });

    for (std::size_t c = 1; c < candidates.size(); ++c) {
        const auto& candidate = candidates[c];

        for (std::size_t i = 0; (i < formats.size()) && (i < candidate.count());
                ++i) {
            const auto cs = static_cast<std::underlying_type_t<semantic>>(
                candidate[i].semantic());
            const auto fs = static_cast<std::underlying_type_t<semantic>>(
                formats[i].first);

            if (cs & fs == 0) {
                // The semantics do not match, so we cannot merge.
                continue;
            }

            // If the candidate adds more semantics, we add it. This is the case
            // if a radius component is added or if the intensity is "upgraded"
            // to a colour.
            formats[i].first = static_cast<semantic>(cs | fs);

            const auto cf = candidate[i].format();
            const auto ff = formats[i].second;

            // If any of the format uses float scalars, we pipe the resulting
            // format though to_floating_point.
            const auto flt = is_floating_point(cf) || is_floating_point(ff);

            // We always prefer the largest vector size.
            const auto cmps = (std::max)(to_components(cf), to_components(ff));

            if (to_scalar_size(cf) > to_scalar_size(ff)) {
                // Candidate scalar is larger than current reference, so we will
                // use the candidate format.
                formats[i].second = flt
                    ? to_floating_point(cf, cmps)
                    : make_format(cf, cmps);
            } else {
                // The reference uses larger scalars, but it could still be that
                // we need to do a float conversion or add more components.
                formats[i].second = flt
                    ? to_floating_point(ff, cmps)
                    : make_format(ff, cmps);
            }
        } /* for (std::size_t i = 0; (i < formats.size()) ... */
    } /* for (std::size_t c = 1; c < candidates.size(); ++c) */

    // Rebuild the group from the merged formats.
    group_type retval;
    for (auto& f : formats) {
        retval.add(f.first, f.second);
    }

    return retval;
}


/*
 * FPD_NAMESPACE::mmpld_formats
 */
template<class TOut, class TIn>
std::size_t FPD_NAMESPACE::mmpld_formats(
        _In_ TOut oit,
        _In_ const TIn begin,
        _In_ const TIn end) {
    typedef typename TOut::container_type container_type;
    typedef typename container_type::value_type group_type;
    static_assert(sizeof(mmpld::vertex_type) == 1, "The vertex type is assumed "
        "to be one byte for the algorithm here to work.");
    static_assert(sizeof(mmpld::colour_type) == 1, "The colour type is assumed "
        "to be one byte for the algorithm here to work.");
    std::set<std::uint16_t> formats;

    for (auto it = begin; it != end; ++it) {
        const auto ci = static_cast<std::uint8_t>(it->colour_type);
        const auto vi = static_cast<std::uint8_t>(it->vertex_type);
        const auto id = (vi << std::numeric_limits<std::uint8_t>::digits) | ci;

        if (formats.find(id) == formats.end()) {
            // We have not seen this format yet, so we add it to the list of
            // known formats and emit an element description for it.
            formats.insert(id);

            group_type group;

            // In MMPLD, vertex data always come first, so we can hard code the
            // order here.
            {
                auto semantic = mmpld_semantic(it->vertex_type);
                auto format = mmpld_format(it->vertex_type);
                if (semantic != semantic::unknown) {
                    group.add(semantic, format);
                }
            }

            {
                auto semantic = mmpld_semantic(it->colour_type);
                auto format = mmpld_format(it->colour_type);
                if (semantic != semantic::unknown) {
                    group.add(semantic, format);
                }
            }

            *oit++ = std::move(group);
        }
    }

    return formats.size();
}


/*
 * FPD_NAMESPACE::mmpld_semantic
 */
FPD_NAMESPACE::semantic FPD_NAMESPACE::mmpld_semantic(
        _In_ const mmpld::colour_type colour) noexcept {
    switch (colour) {
        case mmpld::colour_type::rgb8:
            __fallthrough;
        case mmpld::colour_type::rgba8:
            __fallthrough;
        case mmpld::colour_type::rgb32:
            __fallthrough;
        case mmpld::colour_type::rgba32:
            __fallthrough;
        case mmpld::colour_type::rgba16:
            return semantic::colour;

        case mmpld::colour_type::intensity:
            __fallthrough;
        case mmpld::colour_type::intensity64:
            return semantic::intensity;

        default:
            return semantic::unknown;
    }
}


/*
 * FPD_NAMESPACE::mmpld_semantic
 */
FPD_NAMESPACE::semantic FPD_NAMESPACE::mmpld_semantic(
        _In_ const mmpld::vertex_type vertex) noexcept {
    switch (vertex) {
        case mmpld::vertex_type::float_xyz:
            __fallthrough;
        case mmpld::vertex_type::short_xyz:
            __fallthrough;
        case mmpld::vertex_type::double_xyz:
            return semantic::position;

        case mmpld::vertex_type::float_xyzr:
            return semantic::position_radius;

        default:
            return semantic::unknown;
    }
}
