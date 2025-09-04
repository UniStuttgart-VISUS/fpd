// <copyright file="mmpld.inl" company="Visualisierungsinstitut der Universität Stuttgart">
// Copyright © 2025 Visualisierungsinstitut der Universität Stuttgart.
// Licensed under the MIT licence. See LICENCE file for details.
// </copyright>
// <author>Christoph Müller</author>


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
