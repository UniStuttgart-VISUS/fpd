// <copyright file="semantic.inl" company="Visualisierungsinstitut der Universität Stuttgart">
// Copyright © 2023 Visualisierungsinstitut der Universität Stuttgart.
// Licensed under the MIT licence. See LICENCE file for details.
// </copyright>
// <author>Christoph Müller</author>


/*
 * FPD_NAMESPACE::to_string
 */
template<class TChar> std::basic_string<TChar> FPD_NAMESPACE::to_string(
        _In_ const semantic semantic) {
#define _FPD_SEM_TO_STR(s) case FPD_NAMESPACE::semantic::s:\
    return ESS_TPL_LITERAL(TChar, #s)

    switch (semantic) {
        _FPD_SEM_TO_STR(position);
        _FPD_SEM_TO_STR(radius);
        _FPD_SEM_TO_STR(position_radius);
        _FPD_SEM_TO_STR(intensity);
        _FPD_SEM_TO_STR(colour);
        _FPD_SEM_TO_STR(velocity);

        default:
            return ESS_TPL_LITERAL(TChar, "");
    }

#undef _FPD_SEM_TO_STR
}
