// <copyright file="pkd_tree_builder.inl" company="Visualisierungsinstitut der Universität Stuttgart">
// Copyright © 2024 Visualisierungsinstitut der Universität Stuttgart.
// Licensed under the MIT licence. See LICENCE file for details.
// </copyright>
// <author>Christoph Müller</author>


FPD_NAMESPACE_BEGIN

namespace detail {

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
            _In_ const ospray_pkd_iterator &rhs) const noexcept {
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
            _In_ const ospray_pkd_iterator &rhs) const noexcept {
            return !(*this == rhs);
        }

    private:

        std::size_t _cur;
        std::size_t _end;
        std::size_t _pos;
    };

} /* namespace detail */

FPD_NAMESPACE_END


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
    _scalar(0),
    _stride(0, 0) { }


/*
 * FPD_NAMESPACE::pkd_tree_builder<TChar>::build
 */
template<class TChar>
FPD_NAMESPACE::pkd_tree_builder<TChar> &
FPD_NAMESPACE::pkd_tree_builder<TChar>::build(_In_ void *particles,
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

    auto positions = positions(particles, cnt);

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
    this->_stride.first = 0;

    for (auto& g : desc) {
        auto e = g.find(semantic::position);
        if (e != g.end()) {
            this->_format = e->format();
            this->_components = to_components(this->_format);
            this->_dimensions = this->_components;
            this->_offset += g.offset(*e);
            this->_scalar = to_scalar_size(this->_format);
            this->_stride.second = g.size();

            // If this is a combined position + radius semantic, we need to
            // subtract one dimension that is used by the radius.
            if (e->has_semantic(semantic::radius)) {
                assert(this->_dimensions > 1);
                --this->_dimensions;
            }
            break;
        }

        ++this->_group;
        this->_stride.first += g.size();
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
    assert(this->_stride.second > 0);
    return *this;
}


/*
 * FPD_NAMESPACE::pkd_tree_builder<TChar>::build_ospray
 */
template<class TChar>
template<class TScalar>
void FPD_NAMESPACE::pkd_tree_builder<TChar>::build_ospray(
        _In_ void *particles,
        _In_ std::vector<std::size_t>& indices,
        _In_ const std::size_t node,
        _In_ const bounding_box bounds,
        _In_ const std::size_t depth) const {
    const auto nodes = indices.size();

    auto left = detail::ospray_pkd_left_child(node);
    if (detail::ospray_pkd_invalid_node(left, nodes)) {
        // If there is no left child, there is no child at all, so we are node
        // for the subtree.
        return;
    }

    const auto position = this->position(particles, node, indices);
    auto node_pos = this->component<TScalar>(position, split);
    const auto split = depth % this->_dimensions;
    // const size_t dim = maxDim(bounds.size());

    auto right = detail::ospray_pkd_right_child(node);
    if (detail::ospray_pkd_invalid_node(left, nodes)) {
        // We have exactly one child, so if the order between the node and its
        // child is wrong, we swap it. As we lack the right child, we are done
        // after this.
        if (this->component<TScalar>(particles, left, indices, split)
                > node_pos) {
            std::swap(indices[left], indices[node]);
            this->embed_axis(position, split);
        }

        return;
    }

    detail::ospray_pkd_iterator l0(left);
    detail::ospray_pkd_iterator r0(right);
    detail::ospray_pkd_iterator l = l0;
    detail::ospray_pkd_iterator r = r0;

    while (true) {
        // Search the first mismatch in the left and right subtree.
        while (detail::ospray_pkd_valid_node(l, nodes)
                && (this->component<TScalar>(particles, l, indices, split)
                <= node_pos)) {
            ++l;
        }
        const auto left_mismatch = detail::ospray_pkd_valid_node(l, nodes);

        while (detail::ospray_pkd_valid_node(r, nodes)
                && (this->component<TScalar>(particles, r, indices, split)
                >= node_pos)) {
            ++r;
        }
        const auto right_mismatch = detail::ospray_pkd_valid_node(r, nodes);

        if (left_mismatch && right_mismatch) {
            // Both subtrees have a mismatch.
            std::swap(indices[l++], indices[r++]);
            continue;

        } else if (left_mismatch) {
            // Only the left subtree has a mismatch.
            assert(!right_mismatch);
            l0 = l++;

            while (detail::ospray_pkd_valid_node(l, nodes)) {
                if (this->component<TScalar>(particles, l, indices, split)
                        <= node_pos) {
                    std::swap(indices[l0++], indices[l]);
                }
                ++l;
            }

            std::swap(indices[node], indices[l0++]);

            l = l0;
            r = r0;

            continue;

        } else if (right_mismatch) {
            // Only the right subtree has a mismatch.
            assert(!left_mismatch);
            r0 = r++;

            while (detail::ospray_pkd_valid_node(r, nodes)) {
                if (this->component<TScalar>(particles, r, indices, split)
                        >= node_pos) {
                    std::swap(indices[l0++], indices[l]);
                }

                ++r;
            }

            std::swap(indices[node], indices[r0++]);

            l = l0;
            r = r0;

            continue;
        }

//            if (isValidNode(l, N)) {
//                if (isValidNode(r, N)) {
//                    // both mis-mathces valid, just swap them and go on
//                    swap(l, r); ++l; ++r;
//                    continue;
//                } else {
//                    // mis-match on left side, but nothing on right side to swap with: swap with root
//                    // --> can't go on on right side any more, but can still compact matches on left
//                    l0 = l;
//                    ++l;
//                    while (isValidNode(l, N)) {
//                        if (POS(l, dim) <= rootPos) { swap(l0, l); ++l0; }
//                        ++l;
//                    }
//                    swap(nodeID, l0); ++l0;
//                    rootPos = POS(nodeID, dim);
//
//                    l = l0;
//                    r = r0;
//                    continue;
//                }
//            } else {
//                if (isValidNode(r, N)) {
//                    // mis-match on left side, but nothing on right side to swap with: swap with root
//                    // --> can't go on on right side any more, but can still compact matches on left
//                    r0 = r;
//                    ++r;
//                    while (isValidNode(r, N)) {
//                        if (POS(r, dim) >= rootPos) { swap(r0, r); ++r0; }
//                        ++r;
//                    }
//                    swap(nodeID, r0); ++r0;
//                    rootPos = POS(nodeID, dim);
//
//                    l = l0;
//                    r = r0;
//                    continue;
//                } else {
//                    // no mis-match on either side ... done.
//                    break;
//                }
//            }
//        }

//#if CHECK
//        // check tree:
//        {
//            SubtreeIterator l(leftChildOf(nodeID));
//            SubtreeIterator r(rightChildOf(nodeID));
//            while (isValidNode(l)) {
//                if (!(pos(l, dim) <= pos(nodeID, dim)))
//                    throw std::runtime_error("error in building. not a valid kd-tree...");
//                ++l;
//            }
//            while (isValidNode(r)) {
//                if (!(pos(r, dim) >= pos(nodeID, dim)))
//                    throw std::runtime_error("error in building. not a valid kd-tree...");
//                ++r;
//            }
//        }
//#endif
    }
//
//    box3f lBounds = bounds;
//    box3f rBounds = bounds;

    this->embed_axis(static_cast<TScalar *>(position), split);
//    setDim(nodeID, dim);
//
//    lBounds.upper[dim] = rBounds.lower[dim] = pos(nodeID, dim);
//
//#if 1
//    if ((numLevels - depth) > 20) {
//        std::thread lThread([&]() {
//            pkdBuildThread(new PKDBuildJob(this, leftChildOf(nodeID), lBounds, depth + 1));
//        });
//        buildRec(rightChildOf(nodeID), rBounds, depth + 1);
//        lThread.join();
//    } else
//#endif
//    {
//        buildRec(leftChildOf(nodeID), lBounds, depth + 1);
//        buildRec(rightChildOf(nodeID), rBounds, depth + 1);
//    }
//}
}


/*
 * FPD_NAMESPACE::pkd_tree_builder<TChar>::embed_axis
 */
template<class TChar>
template<class TScalar>
void FPD_NAMESPACE::pkd_tree_builder<TChar>::embed_axis(_In_ TScalar *position,
        _In_ const std::size_t axis) const noexcept {
    if (this->_axis_component < this->_components) {
        // Determine equivalent UINT
        throw "TODO";
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
