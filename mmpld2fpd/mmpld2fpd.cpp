// <copyright file="mmpld2fpd.h" company="Visualisierungsinstitut der Universität Stuttgart">
// Copyright © 2023 Visualisierungsinstitut der Universität Stuttgart.
// Licensed under the MIT licence. See LICENCE file for details.
// </copyright>
// <author>Christoph Müller</author>

#include <iostream>
#include <stdexcept>
#include <string>

#if defined(_WIN32)
#include <Windows.h>
#include <tchar.h>
#else /* defined(_WIN32) */
#define _TCHAR char
#endif /* defined(_WIN32) */

#include <mmpld.h>

#define FPD_WITH_MMPLD
#include <fpd/encoding.h>
#include <fpd/meta_data_builder.h>
#include <fpd/posix_io.h>
#include <fpd/win32_io.h>


/// <summary>
/// Find (case-insensitively) the given switch in the given range.
/// </summary>
template<class TIterator, class TChar>
TIterator find_switch(_In_ const TIterator begin,
        _In_ const TIterator end,
        _In_z_ const TChar *name) {
    typedef std::basic_string<TChar> string_type;
    static const auto eq = [](const TChar l, const TChar r) {
        return (std::tolower(l) == std::tolower(r));
    };

    return std::find_if(begin,
        end,
        [name](const string_type& s) {
            return std::equal(s.begin(), s.end(), name, eq);
        });
}


/// <summary>
/// Find (case-insensitively) the given switch and return its argument.
/// </summary>
template<class TIterator, class TChar>
TIterator find_argument(_In_ const TIterator begin,
        _In_ const TIterator end,
        _In_z_ const TChar *name) {
    auto retval = ::find_switch(begin, end, name);
    return (retval != end) ? ++retval : retval;
}


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
        std::basic_string<_TCHAR> input;
        std::basic_string<_TCHAR> output;

        {
            auto end = argv + argc;
            auto it = ::find_argument(argv, end, _T("--in"));
            if (it == end) {
                throw std::invalid_argument("At least the path to the MMPLD "
                    "input file must be provided as command line argument.");
            }
            input = *it;
        }

        {
            auto end = argv + argc;
            auto it = ::find_argument(argv, end, _T("--out"));
            if (it != end) {
                output = *it;
            } else {
                output = input + _T(".fpd");
            }
        }

        // Open the MMPLD file and go to the first list of the first frame in
        // order to construct the particle format.
        mmpld::file<handle_type, _TCHAR> mmpld(input.c_str());
        mmpld::list_header reference_list { };

        for (std::size_t f = 0; f < mmpld.frames(); ++f) {
            mmpld.open_frame(f);

            if (mmpld.frame_header().lists < 1) {
                continue;
            }

            mmpld.read_particles(true, reference_list, nullptr, 0);
            break;
        }

        if (reference_list.particles == 0) {
            throw std::invalid_argument("The MMPLD file contains no usable "
                "data.");
        }

        // Start building the FPD meta data from the MMPLD format.
        fpd::meta_data_builder<_TCHAR> builder;

        {
            auto desc = std::basic_string<_TCHAR>(_T("Converted from "));
            desc += input;
            builder.with_description(std::move(desc));
        }

        builder.with_mmpld_format(reference_list);

        // Add the frames.
        for (std::size_t f = 0; f < mmpld.frames(); ++f) {

        }



    } catch (std::exception& ex) {
        std::cerr << ex.what() << std::endl;
    }
}
