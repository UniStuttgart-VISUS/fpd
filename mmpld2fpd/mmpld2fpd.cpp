// <copyright file="mmpld2fpd.h" company="Visualisierungsinstitut der Universität Stuttgart">
// Copyright © 2023 Visualisierungsinstitut der Universität Stuttgart.
// Licensed under the MIT licence. See LICENCE file for details.
// </copyright>
// <author>Christoph Müller</author>

#include <iostream>
#include <stdexcept>

#if defined(_WIN32)
#include <Windows.h>
#include <tchar.h>
#else /* defined(_WIN32) */
#define _TCHAR char
#endif /* defined(_WIN32) */

#include <mmpld.h>

#include <fpd/file.h>
#include <fpd/posix_io.h>
#include <fpd/win32_io.h>


/// <summary>
/// Entry point of the application.
/// </summary>
/// <param name="argc"></param>
/// <param name="argv"></param>
/// <returns></returns>
int _tmain(const int argc, const _TCHAR **argv) {
#if defined(_WIN32)
    typedef HANDLE handle_type;
    typedef fpd::win32_io io_type;
#else /* defined(_WIN32) */
    typedef int handle_type;
    typedef fpd::posix_io io_type;
#endif /* defined(_WIN32) */

    try {
        if (argc < 2) {
            throw std::invalid_argument("At least the path to the MMPLD input "
                "file must be provided as command line argument.");
        }

        mmpld::file<handle_type, _TCHAR> input(argv[1]);
        fpd::file<_TCHAR, io_type> output(argv[2]);


    } catch (std::exception& ex) {
        std::cerr << ex.what() << std::endl;
    }
}
