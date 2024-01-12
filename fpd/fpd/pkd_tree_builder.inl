// <copyright file="pkd_tree_builder.inl" company="Visualisierungsinstitut der Universität Stuttgart">
// Copyright © 2024 Visualisierungsinstitut der Universität Stuttgart.
// Licensed under the MIT licence. See LICENCE file for details.
// </copyright>
// <author>Christoph Müller</author>


FPD_NAMESPACE_BEGIN

namespace detail {

    /// <summary>
    /// Gets the index of the left child of <paramref name="node" /> in an
    /// OSPRay-style balanced kd-tree.
    /// </summary>
    static inline constexpr std::size_t ospray_pkd_left_child(
            _In_ const std::size_t node) noexcept {
        return 2 * node + 1;
    }

    /// <summary>
    /// Gets the index of the parent of <paramref name="node" /> in an
    /// OSPRay-style balanced kd-tree.
    /// </summary>
    static inline constexpr std::size_t ospray_pkd_parent(
            _In_ const std::size_t node) noexcept {
        return (node - 1) >> 1;
    }

    /// <summary>
    /// Gets the index of the right child of <paramref name="node" /> in an
    /// OSPRay-style balanced kd-tree.
    /// </summary>
    static inline constexpr std::size_t ospray_pkd_right_child(
            _In_ const std::size_t node) noexcept {
        return 2 * node + 2;
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
                this->_pos = ospray_pkd_left_child(this->_pos - this->_end);
                this->_end += this->_end;
            }

            return *this;
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
    : _dimensions(0),
    _eq(nullptr),
    _format(format::unknown),
    _ge(nullptr),
    _group(0),
    _gt(nullptr),
    _le(nullptr),
    _lt(nullptr),
    _ne(nullptr),
    _offset(0),
    _scalar(0),
    _stride(0, 0) { }


/*
 * FPD_NAMESPACE::pkd_tree_builder<TChar>::build
 */
template<class TChar>
FPD_NAMESPACE::pkd_tree_builder<TChar>&
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
            this->_dimensions = to_components(this->_format);
            this->_offset += g.offset(*e);
            this->_scalar = to_scalar_size(this->_format);
            this->_stride.second = g.size();
            this->resolve_compare_as(formats(), this->_format);

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

    assert(this->_eq != nullptr);
    assert(this->_format != format::unknown);
    assert(this->_dimensions > 0);
    assert(this->_ge != nullptr);
    assert(this->_group >= 0);
    assert(this->_group < desc.count_groups());
    assert(this->_gt != nullptr);
    assert(this->_le != nullptr);
    assert(this->_lt != nullptr);
    assert(this->_ne != nullptr);
    assert(this->_scalar > 0);
    assert(this->_stride.second > 0);
    return *this;
}


/*
 * FPD_NAMESPACE::pkd_tree_builder<TChar>::resolve_compare_as
 */
template<class TChar>
template<FPD_NAMESPACE::format Format, FPD_NAMESPACE::format... Formats>
void FPD_NAMESPACE::pkd_tree_builder<TChar>::resolve_compare_as(
        _In_ format_list<Format, Formats...>, _In_ const format format) {
    if (format == Format) {
        typedef typename format_forward_traits<Format>::scalar value_type;
        this->_eq = compare_as< value_type>::eq;
        this->_ge = compare_as< value_type>::ge;
        this->_gt = compare_as< value_type>::gt;
        this->_le = compare_as< value_type>::le;
        this->_lt = compare_as< value_type>::lt;
        this->_ne = compare_as< value_type>::ne;

    } else {
        this->resolve_compare_as(format_list<Formats...>(), format);
    }
}


/*
 * FPD_NAMESPACE::pkd_tree_builder<TChar>::resolve_compare_as
 */
template<class TChar>
void FPD_NAMESPACE::pkd_tree_builder<TChar>::resolve_compare_as(
        _In_ format_list<>, _In_ const format format) {
    throw std::logic_error("The specified format of particle coordinates "
        "cannot be compared.");
}
