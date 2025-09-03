// <copyright file="test_samples.cpp" company="Visualisierungsinstitut der Universität Stuttgart">
// Copyright © 2025 Visualisierungsinstitut der Universität Stuttgart.
// Licensed under the MIT licence. See LICENCE file for details.
// </copyright>
// <author>Christoph Müller</author>

#include "pch.h"


namespace fpd {

    /// <summary>
    /// Tests whether the sample code in the documentation actually works ...
    /// </summary>
    TEST_CLASS(test_samples) {

    public:

        TEST_METHOD(read_meta) {
            const auto path = make_meta();

            typedef fpd::meta_data<char> meta_data;

            // Open the input file for binary I/O.
            int fh = ::_open(path.c_str(), _O_BINARY | _O_RDONLY);
            Assert::AreNotEqual(-1, fh, L"file opened", LINE_INFO());

            // Read the fixed-size header, which has always the same size.
            std::vector<std::uint8_t> data(meta_data::fixed_header_size);
            auto rem = static_cast<int>(data.size());
            while (rem > 0) {
                auto read = ::_read(fh, data.data() + data.size() - rem, rem);
                Assert::IsTrue(read > 0, L"read something", LINE_INFO());
                rem -= read;
            }

            // Parse what we have so far in order to determine what must be read
            // to get the complete header.
            meta_data md(fpd::encodings::utf8);
            rem = meta_data::check_header_size(md, data.data(), data.size());

            // Read the variable-size header segments.
            data.resize(rem);
            while (rem > 0) {
                auto read = ::_read(fh, data.data() + data.size() - rem, rem);
                Assert::IsTrue(read > 0, L"read something", LINE_INFO());
                rem -= read;
            }

            // Parse the variable-size header segments, which completes parsing of
            // the meta data.
            meta_data::parse(md, data.data(), data.size());

            // Seek to the start of the first frame.
            ::_lseek(fh, md.frame_offset(0), SEEK_SET);
            data.resize(md.frame_size(0));

            // Read the data and do something useful with them ...

            ::_close(fh);
        }



    private:

        static std::string make_meta(void) {
            char retval[MAX_PATH];

            const auto cnt = ::GetTempPathA(MAX_PATH, retval);
            if ((cnt > MAX_PATH) || (cnt == 0)) {
                Assert::Fail(L"Failed to get temporary path", LINE_INFO());
            }

            Assert::IsTrue(::GetTempFileNameA(retval, "fpd", 0, retval), L"GetTempFileName", LINE_INFO());

            meta_data_builder<wchar_t> builder;
            auto data = builder.add_format_element(semantic::position, format::float32xyz, 0)
                .add_format_element(semantic::radius, format::float32, 1)
                .add_format_element(semantic::colour, format::uint64xyzw)
                .with_frames({ 41, 42, 42 })
                .with_description(L"オマイゴッド")
                .build();
            win32_io io(retval, GENERIC_READ | GENERIC_WRITE, 0, CREATE_ALWAYS);
            data.save(io);

            return retval;
        }
    };
}
