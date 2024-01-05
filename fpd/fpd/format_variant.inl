// <copyright file="format_variant.inl" company="Visualisierungsinstitut der Universität Stuttgart">
// Copyright © 2023 - 2024 Visualisierungsinstitut der Universität Stuttgart.
// Licensed under the MIT licence. See LICENCE file for details.
// </copyright>
// <author>Christoph Müller</author>


FPD_NAMESPACE_BEGIN

namespace detail {

    template<class TValue> struct format_value_ctor final {

        template<class TArg>
        typename std::enable_if<std::is_convertible<TValue, TArg>::value>::type
        operator ()(_In_ TValue *target, _In_ TArg&& value) {
            assert(target != nullptr);
            new (target) TValue(static_cast<TValue>(value));
        }

        template<class TArg>
        typename std::enable_if<!std::is_convertible<TValue, TArg>::value>::type
        operator ()(_In_ TValue *target, _In_ TArg&& value) {
            assert(target != nullptr);
            assert(false);  // This should never be called by invoke.
            new (target) TValue();
        }
    };

    template<class TValue> struct format_value_runtime_ctor final {
        inline void operator ()(_In_ TValue *target,
                _In_reads_(cnt) const void *value,
                _In_ std::size_t cnt) {
            assert(target != nullptr);
            assert(cnt >= sizeof(TValue));
            new (target) TValue(*reinterpret_cast<const TValue *>(value));
        }
    };

    template<class TValue> struct format_value_dtor final {
        inline void operator ()(TValue *target) noexcept {
            assert(target != nullptr);
            target->~TValue();
        }
    };

} /* namespace detail */

FPD_NAMESPACE_END


#define _FPD_VAR_FMTS FPD_NAMESPACE::format_list<Formats...>

/*
 * ...::basic_format_variant<_FPD_VAR_FMTS>::basic_format_variant
 */
template<FPD_NAMESPACE::format... Formats>
FPD_NAMESPACE::basic_format_variant<_FPD_VAR_FMTS>::basic_format_variant(
        _In_ const format_type format,
        _In_reads_(cnt) const void *value,
        _In_ const std::size_t cnt)
        : _format(format) {
    if (!this->supports(this->_format)) {
        throw std::invalid_argument("The variant does not support the "
            "specified kind of value.");
    }
    if (value == nullptr) {
        throw std::invalid_argument("The provided data are invalid.");
    }
    if (cnt < to_size(format)) {
        throw std::invalid_argument("The provided data are too small to "
            "represent the specified format.");
    }

    this->invoke<detail::format_value_runtime_ctor>(value, cnt);
}


/*
 * ...::basic_format_variant<_FPD_VAR_FMTS>::basic_format_variant
 */
template<FPD_NAMESPACE::format... Formats>
FPD_NAMESPACE::basic_format_variant<_FPD_VAR_FMTS>::basic_format_variant(
        _In_ const basic_format_variant& rhs)
        : _format(rhs._format) {
    this->invoke<detail::format_value_runtime_ctor>(rhs._data.data(),
        rhs._data.size());
}


/*
 * ...::basic_format_variant<_FPD_VAR_FMTS>::basic_format_variant
 */
template<FPD_NAMESPACE::format... Formats>
template<FPD_NAMESPACE::format... F>
FPD_NAMESPACE::basic_format_variant<_FPD_VAR_FMTS>::basic_format_variant(
        _In_ const basic_format_variant<format_list<F...>>& rhs)
        : _format(rhs._format) {
    if ((this->_format != format_type::unknown)
            && !this->supports(this->_format)) {
        throw std::invalid_argument("A copy of a variant cannot be created "
            "from a variant holding an unsupported kind of value.");
    }

    this->invoke<detail::format_value_runtime_ctor>(rhs._data.data(),
        rhs._data.size());
}


/*
 * ...::basic_format_variant<_FPD_VAR_FMTS>::basic_format_variant
 */
template<FPD_NAMESPACE::format... Formats>
FPD_NAMESPACE::basic_format_variant<_FPD_VAR_FMTS>::basic_format_variant(
        _Inout_ basic_format_variant<_FPD_VAR_FMTS>&& rhs)
        : _data(std::move(rhs._data)), _format(rhs._format) {
    assert(this->supports(this->_format));
    rhs._format = format::unknown;
}


/*
 * FPD_NAMESPACE::basic_format_variant<_FPD_VAR_FMTS>::~basic_format_variant
 */
template<FPD_NAMESPACE::format... Formats>
FPD_NAMESPACE::basic_format_variant<_FPD_VAR_FMTS>::~basic_format_variant(
        void) noexcept {
    this->invoke<detail::format_value_dtor>();
}


/*
 * FPD_NAMESPACE::basic_format_variant<_FPD_VAR_FMTS>::clear
 */
template<FPD_NAMESPACE::format... Formats>
void FPD_NAMESPACE::basic_format_variant<_FPD_VAR_FMTS>::clear(
        void) noexcept {
    this->invoke<detail::format_value_dtor>();
    this->_format = format_type::unknown;
}


/*
 * FPD_NAMESPACE::basic_format_variant<_FPD_VAR_FMTS>::get
 */
template<FPD_NAMESPACE::format... Formats>
template<class TValue>
const TValue& FPD_NAMESPACE::basic_format_variant<_FPD_VAR_FMTS>::get(
        void) const {
    auto retval = this->try_get<TValue>();
    if (retval == nullptr) {
        throw std::logic_error("The variant does not hold the expected kind of "
            "data.");
    } else {
        return *retval;
    }
}


/*
 * FPD_NAMESPACE::basic_format_variant<_FPD_VAR_FMTS>::get
 */
template<FPD_NAMESPACE::format... Formats>
template<class TValue>
TValue& FPD_NAMESPACE::basic_format_variant<_FPD_VAR_FMTS>::get(void) {
    auto retval = this->try_get<TValue>();
    if (retval == nullptr) {
        throw std::logic_error("The variant does not hold the expected kind of "
            "data.");
    } else {
        return *retval;
    }
}


/*
 * FPD_NAMESPACE::basic_format_variant<_FPD_VAR_FMTS>::operator =
 */
template<FPD_NAMESPACE::format... Formats>
FPD_NAMESPACE::basic_format_variant<_FPD_VAR_FMTS> &
FPD_NAMESPACE::basic_format_variant<_FPD_VAR_FMTS>::operator =(
        _In_ const basic_format_variant &rhs) {
    if (this != std::addressof(rhs)) {
        this->invoke<detail::format_value_dtor>();
        this->_format = rhs._format;
        this->invoke<detail::format_value_runtime_ctor>(rhs._data.data(),
            rhs._data.size());
    }

    return *this;
}


/*
 * FPD_NAMESPACE::basic_format_variant<_FPD_VAR_FMTS>::operator =
 */
template<FPD_NAMESPACE::format... Formats>
template<FPD_NAMESPACE::format... F>
FPD_NAMESPACE::basic_format_variant<_FPD_VAR_FMTS>&
FPD_NAMESPACE::basic_format_variant<_FPD_VAR_FMTS>::operator =(
        _In_ const basic_format_variant<format_list<F...>>& rhs) {
    if ((rhs._format != format_type::unknown) && !this->supports(rhs._format)) {
        throw std::invalid_argument("The right-hand side operand holds "
            "a value which is not supported by the left-hand side "
            "operand.");
    }

    this->invoke<detail::format_value_dtor>();
    this->_format = rhs._format;
    this->invoke<detail::format_value_runtime_ctor>(rhs._data.data(),
        rhs._data.size());

    return *this;
}


/*
 * FPD_NAMESPACE::basic_format_variant<_FPD_VAR_FMTS>::operator =
 */
template<FPD_NAMESPACE::format... Formats>
FPD_NAMESPACE::basic_format_variant<_FPD_VAR_FMTS>&
FPD_NAMESPACE::basic_format_variant<_FPD_VAR_FMTS>::operator =(
        _In_ basic_format_variant&& rhs) noexcept {
    if (this != std::addressof(rhs)) {
        this->invoke<detail::format_value_dtor>();
        this->_data = std::move(rhs._data);
        this->_format = rhs._format;
        rhs._format = format::unknown;
    }

    return *this;
}


/*
 * FPD_NAMESPACE::basic_format_variant<_FPD_VAR_FMTS>::invoke0
 */
template<FPD_NAMESPACE::format... Formats>
template<template<class> class TFunc,
    FPD_NAMESPACE::format Fmt,
    FPD_NAMESPACE::format... Fmts,
    class... TArgs>
void FPD_NAMESPACE::basic_format_variant<_FPD_VAR_FMTS>::invoke0(
        _In_ format_list<Fmt, Fmts...>,
        _In_ TArgs&&... args) {
    typedef typename format_forward_traits<Fmt>::type value_type;
    if (Fmt == this->_format) {
        FPD_NAMESPACE::invoke(TFunc<value_type>(),
            this->try_get<value_type>(),
            std::forward<TArgs>(args)...);
    } else {
        this->invoke0<TFunc>(format_list<Fmts...>(),
            std::forward<TArgs>(args)...);
    }
}

#undef _FPD_VAR_FMTS
