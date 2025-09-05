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
    try {
        std::size_t batch_size = 0;
        std::vector<std::uint8_t> buffer;
        std::basic_string<_TCHAR> description;
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

        {
            auto end = argv + argc;
            auto it = ::find_argument(argv, end, _T("--description"));
            if (it != end) {
                description = *it;
            } else {
                description = std::basic_string<_TCHAR>(_T("Converted from "));
                description += input;
                description += _T(".");
            }
        }

        {
            auto end = argv + argc;
            auto it = ::find_argument(argv, end, _T("--batch"));
            if (it != end) {
                output = std::stoi(*it);
            }
        }

        // Open the MMPLD file and scan its contents to build the particle
        // format.
        mmpld::file_header file_header;
        std::vector<std::uint64_t> frame_sizes;
        std::vector<mmpld::list_header> list_headers;
        mmpld::seek_table seek_table;

        std::ifstream mmpld;
        mmpld.exceptions(mmpld.exceptions() | std::ios::failbit);
        mmpld.open(input, std::ios::binary);

        mmpld::read_file_header(mmpld, file_header, seek_table);

        for (decltype(file_header.frames) f = 0; f < file_header.frames; ++f) {
            mmpld::frame_header fh;

            mmpld.seekg(seek_table[f]);
            mmpld::read_frame_header(mmpld, file_header.version, fh);

            frame_sizes.push_back(0);

            for (decltype(fh.lists) l = 0; l < fh.lists; ++l) {
                list_headers.emplace_back();
                auto& lh = list_headers.back();
                mmpld::read_list_header(mmpld, file_header.version, lh);
                frame_sizes.back() += lh.particles;
            }
        }

        // Determine the particle format in the FPD file. From this format,
        // which might be a superset of all lists in the MMPLD file, we derive
        // the common MMPLD format that we will use to convert the data while
        // reading.
        auto fpd_format = fpd::mmpld_format<_TCHAR>(list_headers.begin(),
            list_headers.end());
        auto mmpld_format = fpd::mmpld_list(fpd_format);

        fpd::meta_data_builder<_TCHAR> builder;
        builder.with_format(fpd_format)
            .with_description(description)
            .with_frames(frame_sizes.begin(), frame_sizes.end());
        auto meta_data = builder.build();

        // Write the meta data to the FPD file.
#if defined(_WIN32)
        fpd::detail::io_traits<HANDLE> fpd(output.c_str(),
            GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ, CREATE_ALWAYS);
#else /* defined(_WIN32) */
        fpd::detail::io_traits<int> fpd(output.c_str(),
            O_RDWR | O_BINARY | O_TRUNC);
#endif /* defined(_WIN32) */
        meta_data.save(fpd);

        // Append the frames.
        for (decltype(file_header.frames) f = 0; f < file_header.frames; ++f) {
            mmpld::frame_header fh;

            mmpld.seekg(seek_table[f]);
            mmpld::read_frame_header(mmpld, file_header.version, fh);

            for (decltype(fh.lists) l = 0; l < fh.lists; ++l) {
                mmpld::list_header lh;
                mmpld::read_list_header(mmpld, file_header.version, lh);

                mmpld_format.particles = lh.particles;

                const auto cnt = mmpld::get_size<std::size_t>(mmpld_format);
                if (buffer.size() < cnt) {
                    buffer.resize(cnt);
                }

                if (batch_size > 0) {
                    ::mmpld::read_as(mmpld, lh, buffer.data(), mmpld_format,
                        batch_size);
                } else {
                    mmpld::read_as(mmpld, lh, buffer.data(), mmpld_format);
                }

                if (file_header.version == mmpld::make_version(1, 1)) {
                    mmpld::skip_cluster_info(mmpld);
                }

                ::fpd::detail::write_all(fpd, buffer.data(), cnt);
            } /* for (decltype(fh.lists) l = 0; l < fh.lists; ++l) */
        } /* for (decltype(file_header.frames) f = 0; ... */

    } catch (std::exception& ex) {
        std::cerr << ex.what() << std::endl;
    }
}
