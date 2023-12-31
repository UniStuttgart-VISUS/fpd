// <copyright file="format_variant.inl" company="Visualisierungsinstitut der Universität Stuttgart">
// Copyright © 2023 Visualisierungsinstitut der Universität Stuttgart.
// Licensed under the MIT licence. See LICENCE file for details.
// </copyright>
// <author>Christoph Müller</author>



/*
 * ...::basic_format_variant
 */
template<FPD_NAMESPACE::format ...Formats>
template<class TValue>
FPD_NAMESPACE::basic_format_variant<FPD_NAMESPACE::format_list<Formats...>>
::basic_format_variant(const TValue& value)
        : _format(format_reverse_traits<TValue>::format) {
}
