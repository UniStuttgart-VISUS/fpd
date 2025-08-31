// <copyright file="pkd_tree_builder.inl" company="Visualisierungsinstitut der Universität Stuttgart">
// Copyright © 2024 - 2025 Visualisierungsinstitut der Universität Stuttgart.
// Licensed under the MIT licence. See LICENCE file for details.
// </copyright>
// <author>Christoph Müller</author>


FPD_DETAIL_NAMESPACE_BEGIN

/// <summary>
/// Answer whether <paramref name="node" /> is not a valid node in a tree of
/// <paramref name="cnt" /> particles.
/// </summary>
inline constexpr bool ospray_pkd_invalid_node(
        _In_ const std::size_t node,
        _In_ const std::size_t cnt) noexcept {
    return (node >= cnt);
}

/// <summary>
/// Gets the index of the left child of <paramref name="node" /> in an
/// OSPRay-style balanced kd-tree.
/// </summary>
inline constexpr std::size_t ospray_pkd_left_child(
        _In_ const std::size_t node) noexcept {
    return 2 * node + 1;
}

/// <summary>
/// Gets the index of the parent of <paramref name="node" /> in an
/// OSPRay-style balanced kd-tree.
/// </summary>
inline constexpr std::size_t ospray_pkd_parent(
        _In_ const std::size_t node) noexcept {
    return (node - 1) >> 1;
}

/// <summary>
/// Gets the index of the right child of <paramref name="node" /> in an
/// OSPRay-style balanced kd-tree.
/// </summary>
inline constexpr std::size_t ospray_pkd_right_child(
        _In_ const std::size_t node) noexcept {
    return 2 * node + 2;
}

/// <summary>
/// Answer whether <paramref name="node" /> is a valid node in a tree of
/// <paramref name="cnt" /> particles.
/// </summary>
inline constexpr bool ospray_pkd_valid_node(
        _In_ const std::size_t node,
        _In_ const std::size_t cnt) noexcept {
    return (node < cnt);
}

/// <summary>
/// Iterates over the nodes of a specific subtree in level-order (breadth
/// first).
/// </summary>
/// <remarks>
/// <para>This iterator assumes a tree layout as used in OSPRay, i.e. for
/// each node, the left child is located at (2 * node + 1) and the right
/// child at (2 * node + 2). In this case, the iterator will iterate all
/// node indices of a node and its children in level-order.</para>
/// <para>This code was derived from
/// https://github.com/ingowald/ospray-module-pkd/blob/master/apps/PartiKD.cpp
/// </para>
/// </remarks>
class ospray_pkd_iterator final {

public:

    /// <summary>
    /// Initialises a new instance.
    /// </summary>
    /// <param name="root">The zero-based index of the root node.</param>
    ospray_pkd_iterator(_In_ const std::size_t root)
        : _pos(root), _end(1), _cur(0) { }

    /// <summary>
    /// Increments the iterator.
    /// </summary>
    /// <returns><c>*this</c>.</returns>
    inline ospray_pkd_iterator& operator ++(void) noexcept {
        ++this->_pos;

        if (++this->_cur == this->_end) {
            this->_cur = 0;
            this->_pos = detail::ospray_pkd_left_child(this->_pos - this->_end);
            this->_end += this->_end;
        }

        return *this;
    }

    /// <summary>
    /// Postfix-increment the iterator.
    /// </summary>
    /// <returns><c>*this</c> before the increment.</returns>
    inline ospray_pkd_iterator& operator ++(int) noexcept {
        auto retval = *this;
        ++(*this);
        return retval;
    }

    /// <summary>
    /// Answer the current position of the iterator.
    /// </summary>
    /// <returns>The current position of the iterator.</returns>
    inline operator std::size_t(void) const noexcept {
        return this->_pos;
    }

    /// <summary>
    /// Dereferences the iterator.
    /// </summary>
    /// <returns>The current position of the iterator.</returns>
    inline std::size_t operator *(void) const noexcept {
        return this->_pos;
    }

    /// <summary>
    /// Test for equality.
    /// </summary>
    /// <param name="rhs">The object to be compared.</param>
    /// <returns><c>true</c> if this iterator and <paramref name="rhs" />
    /// point to the same node, <c>false</c> otherwise.</returns>
    inline bool operator ==(
        _In_ const ospray_pkd_iterator& rhs) const noexcept {
        // Note: if the positions are the same, the rest must match, too.
        return (this->_pos == rhs._pos);
    }

    /// <summary>
    /// Test for inequality.
    /// </summary>
    /// <param name="rhs">The object to be compared.</param>
    /// <returns><c>true</c> if this iterator and <paramref name="rhs" />
    /// do not point to the same node, <c>false</c> otherwise.</returns>
    inline bool operator !=(
        _In_ const ospray_pkd_iterator& rhs) const noexcept {
        return !(*this == rhs);
    }

private:

    std::size_t _cur;
    std::size_t _end;
    std::size_t _pos;
};

FPD_DETAIL_NAMESPACE_END


/*
 * FPD_NAMESPACE::pkd_tree_builder<TChar>::pkd_tree_builder
 */
    template<class TChar>
FPD_NAMESPACE::pkd_tree_builder<TChar>::pkd_tree_builder(void) noexcept
    : _axis_component((std::numeric_limits<std::size_t>::max)()),
    _components(0),
    _dimensions(0),
    _format(format::unknown),
    _group(0),
    _offset(0),
    _parallel_threshold((std::numeric_limits<std::size_t>::max)()),
    _particles(0),
    _scalar(0) { }


/*
 * FPD_NAMESPACE::pkd_tree_builder<TChar>::build
 */
template<class TChar>
FPD_NAMESPACE::pkd_tree_builder<TChar>&
FPD_NAMESPACE::pkd_tree_builder<TChar>::build(
        _In_ void *particles,
        _In_ const std::size_t cnt) {
    if ((cnt > 0) && (particles == nullptr)) {
        throw std::invalid_argument("A valid pointer to the particles must be "
            "provided.");
    }

    if (this->_format == format::unknown) {
        throw std::logic_error("The tree builder must be configured before it "
            "can be used to build trees.");
    }

    if ((this->_axis_dimension < this->_dimensions) && !this->_bbox) {
        throw std::logic_error("A valid bounding box must be provided if the "
            "split axes are to be chosen dynamically based on the bounding "
            "box.");
    }

    // Cache the begin of each stream such that we subsequently have direct
    // access to it.
    for (auto& s : this->_streams) {
        s.start = static_cast<std::uint8_t *>(particles);
        s.start = this->operator ()(s.start, cnt);
    }

    throw "TODO";
}


/*
 * FPD_NAMESPACE::pkd_tree_builder<TChar>::for_format
 */
template<class TChar>
FPD_NAMESPACE::pkd_tree_builder<TChar>&
FPD_NAMESPACE::pkd_tree_builder<TChar>::for_format(
        _In_ const format_description_type& desc) {
    // Search all flexible elements for one with position semantics. If we have
    // that, we know the group, the offset and the format of the position, which
    // also encodes the dimensionality of the space.
    this->_dimensions = 0;
    this->_format = format::unknown;
    this->_group = 0;
    this->_offset = 0;

    // The '_streams' hold the offsets of all groups and '_group' is the index 
    // of the group holding the position in '_streams'.
    std::size_t offset = 0;
    this->_streams.clear();
    this->_streams.reserve(desc.count_groups());

    for (auto& g : desc) {
        this->_streams.emplace_back(offset);
        offset += this->_streams.back().stride = g.size();

        auto e = g.find(semantic::position);
        if (e != g.end()) {
            if (this->_format == format::unknown) {
                throw std::invalid_argument("The specified format description "
                    "multiple positions, wherefore it is not clear which on "
                    "the pkd tree should be based.");
            }
            assert(this->_format == format::unknown);
            this->_format = e->format();
            this->_components = to_components(this->_format);
            this->_dimensions = this->_components;
            this->_offset = g.offset(*e);
            this->_scalar = to_scalar_size(this->_format);

            // If this is a combined position + radius semantic, we need to
            // subtract one dimension that is used by the radius.
            if (e->has_semantic(semantic::radius)) {
                assert(this->_dimensions > 1);
                --this->_dimensions;
            }
            break;
        }

        ++this->_group;
    }

    if (this->_format == format::unknown) {
        throw std::invalid_argument("The specified format description does not "
            "hold a usable position.");
    }

    assert(this->_format != format::unknown);
    assert(this->_components > 0);
    assert(this->_dimensions > 0);
    assert(this->_dimensions <= this->_components);
    assert(this->_group >= 0);
    assert(this->_group < desc.count_groups());
    assert(this->_scalar > 0);
    assert(this->_streams.size() == desc.count_groups());
    assert(this->_offset < this->_streams[this->_group].stride);
    return *this;
}


/*
 * FPD_NAMESPACE::pkd_tree_builder<TChar>::build_ospray
 */
template<class TChar>
template<class TScalar>
void FPD_NAMESPACE::pkd_tree_builder<TChar>::build_ospray(
        _In_ const std::size_t node,
        _In_ const bounding_box bounds,
        _In_ const std::size_t depth) const {
    assert(this->_format != format::unknown);
    assert(std::all_of(this->_streams.begin(), this->_streams.end(),
        [](const stream& s) { return (s.start != nullptr); }));

    auto left = detail::ospray_pkd_left_child(node);
    if (detail::ospray_pkd_invalid_node(left, this->_particles)) {
        // If there is no left child, there is no child at all, so the root node
        // is the whole subtree.
        return;
    }

    const auto position = this->position<TScalar>(node);
    const auto split = depth % this->_dimensions;
    const auto node_pos = position[split];
    // TODO: select dimension based on bounding box
    // const size_t dim = maxDim(bounds.size());

    auto right = detail::ospray_pkd_right_child(node);
    if (detail::ospray_pkd_invalid_node(left, this->_particles)) {
        // We have exactly one child, so if the order between the node and its
        // child is wrong, we swap it. As we lack another child, we are done
        // after this.
        if (this->position<TScalar>(left, split) > node_pos) {
            this->swap_particles(left, node);
            this->embed_axis(position, split);
        }
        return;
    }

    detail::ospray_pkd_iterator l(left);
    detail::ospray_pkd_iterator r(right);

    while (true) {
        // Search the first mismatch in the left and right subtree.
        while (detail::ospray_pkd_valid_node(l, this->_particles)
                && (this->position<TScalar>(l, split) <= node_pos)) {
            ++l;
        }
        const auto left_mismatch = detail::ospray_pkd_valid_node(l,
            this->_particles);

        while (detail::ospray_pkd_valid_node(r, this->_particles)
                && (this->position<TScalar>(r, split) >= node_pos)) {
            ++r;
        }
        const auto right_mismatch = detail::ospray_pkd_valid_node(r,
            this->_particles);

        if (left_mismatch && right_mismatch) {
            // Both subtrees have a mismatch.
            this->swap_particles(l++, r++);
            continue;

        } else if (left_mismatch) {
            // Only the left subtree has a mismatch.
            assert(!right_mismatch);
            auto ll = l++;

            while (detail::ospray_pkd_valid_node(l, this->_particles)) {
                if (this->position<TScalar>(l, split) <= node_pos) {
                    this->swap_particles(ll++, l);
                }
                ++l;
            }

            this->swap_particles(node, ll++);

            l = ll;
            continue;

        } else if (right_mismatch) {
            // Only the right subtree has a mismatch.
            assert(!left_mismatch);
            auto rr = r++;

            while (detail::ospray_pkd_valid_node(r, this->_particles)) {
                if (this->position<TScalar>(r, split) >= node_pos) {
                    this->swap_particles(rr++, r);
                }
                ++r;
            }

            this->swap_particles(node, rr++);

            r = rr;
            continue;
        } /* if (left_mismatch && right_mismatch) */

#if defined(FPD_CHECK_PDK)
        {
            detail::ospray_pkd_iterator l(detail::ospray_pkd_left_child(node));
            detail::ospray_pkd_iterator r(detail::ospray_pkd_right_child(node));

            while (detail::ospray_pkd_valid_node(l, this->_particles)) {
                if (!(this->position<TScalar>(l, split) <= node_pos)) {
                    throw std::runtime_error("The left subtree is not a valid "
                        "kd-tree.");
                }
                ++l;
            }

            while (detail::ospray_pkd_valid_node(r, this->_particles)) {
                if (!(this->position<TScalar>(r, split) >= node_pos)) {
                    throw std::runtime_error("The right subtree is not a valid "
                        "kd-tree.");
                }
                ++r;
            }
        }
#endif /* defined(FPD_CHECK_PDK) */
    } /* while (true) */

//    box3f lBounds = bounds;
//    box3f rBounds = bounds;

    this->embed_axis(static_cast<TScalar *>(position), split);

    auto left_bounds = bounds;
    auto right_bounds = bounds;
//    lBounds.upper[dim] = rBounds.lower[dim] = pos(nodeID, dim);

    /* numLevels - depth */
    if (0 > this->_parallel_threshold) {
        std::thread t(&build_ospray<TScalar>,
            this,
            detail::ospray_pkd_left_child(node),
            left_bounds,
            depth + 1);
        this->build_ospray<TScalar>(
            detail::ospray_pkd_right_child(node),
            right_bounds,
            depth + 1);
        assert(t.joinable());
        t.join();

    } else {
        this->build_ospray<TScalar>(
            detail::ospray_pkd_left_child(node),
            left_bounds,
            depth + 1);
        this->build_ospray<TScalar>(
            detail::ospray_pkd_right_child(node),
            right_bounds,
            depth + 1);
    }
}


/*
 * FPD_NAMESPACE::pkd_tree_builder<TChar>::embed_axis
 */
template<class TChar>
template<class TScalar>
void FPD_NAMESPACE::pkd_tree_builder<TChar>::embed_axis(_In_ TScalar *position,
        _In_ const std::size_t axis) const noexcept {
    typedef detail::make_unsigned<TScalar> mask_type;
    constexpr auto three = static_cast<mask_type>(3);

    if (this->_axis_component < this->_components) {
        auto a = reinterpret_cast<mask_type *>(position);
        a += this->_axis_component;
        // Erase the three least significant bits and and replace them with the
        // split axis.
        assert(axis < 3);
        *a = (*a & ~three) | static_cast<mask_type>(axis);
    }
}


//inline void PartiKD::swap(const size_t a, const size_t b) const
//{
//    std::swap(model->position[a], model->position[b]);
//    for (size_t i = 0; i < model->attribute.size(); i++)
//        std::swap(model->attribute[i]->value[a], model->attribute[i]->value[b]);
//    if (!model->type.empty())
//        std::swap(model->type[a], model->type[b]);
//}


///*
// * FPD_NAMESPACE::pkd_tree_builder<TChar>::resolve_compare_as
// */
//template<class TChar>
//template<FPD_NAMESPACE::format Format, FPD_NAMESPACE::format... Formats>
//void FPD_NAMESPACE::pkd_tree_builder<TChar>::resolve_compare_as(
//        _In_ format_list<Format, Formats...>, _In_ const format format) {
//    if (format == Format) {
//        typedef typename format_forward_traits<Format>::scalar value_type;
//        this->_eq = compare_as< value_type>::eq;
//        this->_ge = compare_as< value_type>::ge;
//        this->_gt = compare_as< value_type>::gt;
//        this->_le = compare_as< value_type>::le;
//        this->_lt = compare_as< value_type>::lt;
//        this->_ne = compare_as< value_type>::ne;
//
//    } else {
//        this->resolve_compare_as(format_list<Formats...>(), format);
//    }
//}
//
//
///*
// * FPD_NAMESPACE::pkd_tree_builder<TChar>::resolve_compare_as
// */
//template<class TChar>
//void FPD_NAMESPACE::pkd_tree_builder<TChar>::resolve_compare_as(
//        _In_ format_list<>, _In_ const format format) {
//    throw std::logic_error("The specified format of particle coordinates "
//        "cannot be compared.");
//}


/*
 * FPD_NAMESPACE::pkd_tree_builder<TChar>::swap_particles
 */
template<class TChar>
void FPD_NAMESPACE::pkd_tree_builder<TChar>::swap_particles(
        _In_ const std::size_t lhs, _In_ const std::size_t rhs) noexcept {
    assert(lhs < this->_particles);
    assert(rhs < this->_particles);
    std::vector<std::uint8_t> buffer(0);

    for (auto& s : this->_streams) {
        if (buffer.size() < s.stride) {
            buffer.resize(s.stride);
        }

        ::memcpy(buffer.data(), s(lhs), s.stride);
        ::memcpy(s(lhs), s(rhs), s.stride);
        ::memcpy(s(rhs), buffer.data(), s.stride);
    }
}
