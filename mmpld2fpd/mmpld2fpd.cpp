// <copyright file="mmpld2fpd.h" company="Visualisierungsinstitut der Universität Stuttgart">
// Copyright © 2023 Visualisierungsinstitut der Universität Stuttgart.
// Licensed under the MIT licence. See LICENCE file for details.
// </copyright>
// <author>Christoph Müller</author>

#include <iostream>
#include <stdexcept>

#include <mmpld.h>
#include <tchar.h>


/// <summary>
/// Entry point of the application.
/// </summary>
/// <param name="argc"></param>
/// <param name="argv"></param>
/// <returns></returns>
int _tmain(const int argc, const TCHAR **argv) {
    try {
        if (argc < 2) {
            throw std::invalid_argument("At least the path to the MMPLD input "
                "file must be provided as command line argument.");
        }

        mmpld::file<HANDLE, TCHAR> input(argv[1]);


    } catch (std::exception& ex) {
        std::cerr << ex.what() << std::endl;
    }
}
