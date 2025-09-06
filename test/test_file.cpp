// <copyright file="test_file.cpp" company="Visualisierungsinstitut der Universität Stuttgart">
// Copyright © 2025 Visualisierungsinstitut der Universität Stuttgart.
// Licensed under the MIT licence. See LICENCE file for details.
// </copyright>
// <author>Christoph Müller</author>

#include "pch.h"


namespace fpd {

    TEST_CLASS(test_file) {

    public:

        TEST_METHOD(ctor_char) {
            auto path = make_meta();
            file<char, HANDLE> file(encodings::utf8, path.c_str());
            Assert::AreEqual(int(endian::system), int(file.meta_data().endian()), L"endian", LINE_INFO());
            Assert::AreEqual(std::size_t(3), file.meta_data().format_description().count_elements(), L"format elements", LINE_INFO());
            Assert::AreEqual(std::size_t(3), file.meta_data().format_description().count_groups(), L"format groups", LINE_INFO());
            Assert::AreEqual(int(semantic::position), int(file.meta_data().format_description(0, 0).semantic()), L"semantic 0, 0", LINE_INFO());
            Assert::AreEqual(int(format::float32xyz), int(file.meta_data().format_description(0, 0).format()), L"format 0, 0", LINE_INFO());
            Assert::AreEqual(int(semantic::radius), int(file.meta_data().format_description(1, 0).semantic()), L"semantic 1, 0", LINE_INFO());
            Assert::AreEqual(int(format::float32), int(file.meta_data().format_description(1, 0).format()), L"format 1, 0", LINE_INFO());
            Assert::AreEqual(int(semantic::colour), int(file.meta_data().format_description(2, 0).semantic()), L"semantic 2, 0", LINE_INFO());
            Assert::AreEqual(int(format::uint64xyzw), int(file.meta_data().format_description(2, 0).format()), L"format 2, 0", LINE_INFO());
            Assert::AreEqual(std::size_t(3), file.meta_data().frames(), L"frames", LINE_INFO());
            Assert::AreEqual(std::size_t(41), file.meta_data().particles(0), L"particles frame 0", LINE_INFO());
            Assert::AreEqual(std::size_t(42), file.meta_data().particles(1), L"particles frame 1", LINE_INFO());
            Assert::AreEqual(std::size_t(43), file.meta_data().particles(2), L"particles frame 2", LINE_INFO());
            Assert::AreEqual(u8"オマイゴッド", file.meta_data().description().c_str(), L"description", LINE_INFO());
            Assert::AreEqual(int(format::float32), int(file.meta_data().bounding_box().format()), L"bbox format", LINE_INFO());
            Assert::AreEqual(0.0f, file.meta_data().bounding_box().get<float>(bounding_box::value::min_x), L"bbox min_x", LINE_INFO());
            Assert::AreEqual(1.0f, file.meta_data().bounding_box().get<float>(bounding_box::value::min_y), L"bbox min_y", LINE_INFO());
            Assert::AreEqual(2.0f, file.meta_data().bounding_box().get<float>(bounding_box::value::min_z), L"bbox min_z", LINE_INFO());
            Assert::AreEqual(42.0f, file.meta_data().bounding_box().get<float>(bounding_box::value::max_x), L"bbox max_x", LINE_INFO());
            Assert::AreEqual(43.0f, file.meta_data().bounding_box().get<float>(bounding_box::value::max_y), L"bbox max_y", LINE_INFO());
            Assert::AreEqual(44.0f, file.meta_data().bounding_box().get<float>(bounding_box::value::max_z), L"bbox max_z", LINE_INFO());
        }

        TEST_METHOD(ctor_wchar) {
            auto path = make_meta();
            file<wchar_t, HANDLE> file(encodings::oem, path.c_str());
            Assert::AreEqual(int(endian::system), int(file.meta_data().endian()), L"endian", LINE_INFO());
            Assert::AreEqual(std::size_t(3), file.meta_data().format_description().count_elements(), L"format elements", LINE_INFO());
            Assert::AreEqual(std::size_t(3), file.meta_data().format_description().count_groups(), L"format groups", LINE_INFO());
            Assert::AreEqual(int(semantic::position), int(file.meta_data().format_description(0, 0).semantic()), L"semantic 0, 0", LINE_INFO());
            Assert::AreEqual(int(format::float32xyz), int(file.meta_data().format_description(0, 0).format()), L"format 0, 0", LINE_INFO());
            Assert::AreEqual(int(semantic::radius), int(file.meta_data().format_description(1, 0).semantic()), L"semantic 1, 0", LINE_INFO());
            Assert::AreEqual(int(format::float32), int(file.meta_data().format_description(1, 0).format()), L"format 1, 0", LINE_INFO());
            Assert::AreEqual(int(semantic::colour), int(file.meta_data().format_description(2, 0).semantic()), L"semantic 2, 0", LINE_INFO());
            Assert::AreEqual(int(format::uint64xyzw), int(file.meta_data().format_description(2, 0).format()), L"format 2, 0", LINE_INFO());
            Assert::AreEqual(std::size_t(3), file.meta_data().frames(), L"frames", LINE_INFO());
            Assert::AreEqual(std::size_t(41), file.meta_data().particles(0), L"particles frame 0", LINE_INFO());
            Assert::AreEqual(std::size_t(42), file.meta_data().particles(1), L"particles frame 1", LINE_INFO());
            Assert::AreEqual(std::size_t(43), file.meta_data().particles(2), L"particles frame 2", LINE_INFO());
            Assert::AreEqual(L"オマイゴッド", file.meta_data().description().c_str(), L"description", LINE_INFO());
            Assert::AreEqual(int(format::float32), int(file.meta_data().bounding_box().format()), L"bbox format", LINE_INFO());
            Assert::AreEqual(0.0f, file.meta_data().bounding_box().get<float>(bounding_box::value::min_x), L"bbox min_x", LINE_INFO());
            Assert::AreEqual(1.0f, file.meta_data().bounding_box().get<float>(bounding_box::value::min_y), L"bbox min_y", LINE_INFO());
            Assert::AreEqual(2.0f, file.meta_data().bounding_box().get<float>(bounding_box::value::min_z), L"bbox min_z", LINE_INFO());
            Assert::AreEqual(42.0f, file.meta_data().bounding_box().get<float>(bounding_box::value::max_x), L"bbox max_x", LINE_INFO());
            Assert::AreEqual(43.0f, file.meta_data().bounding_box().get<float>(bounding_box::value::max_y), L"bbox max_y", LINE_INFO());
            Assert::AreEqual(44.0f, file.meta_data().bounding_box().get<float>(bounding_box::value::max_z), L"bbox max_z", LINE_INFO());
        }


    private:

        static std::string get_temp_file(void) {
            char retval[MAX_PATH];

            const auto cnt = ::GetTempPathA(MAX_PATH, retval);
            if ((cnt > MAX_PATH) || (cnt == 0)) {
                Assert::Fail(L"Failed to get temporary path", LINE_INFO());
            }

            Assert::IsTrue(::GetTempFileNameA(retval, "fpd", 0, retval), L"GetTempFileName", LINE_INFO());

            return retval;
        }

        static std::string make_meta(void) {
            auto retval = get_temp_file();

            meta_data_builder<wchar_t> builder;
            auto data = builder.add_format_element(semantic::position, format::float32xyz, 0)
                .add_format_element(semantic::radius, format::float32, 1)
                .add_format_element(semantic::colour, format::uint64xyzw, 2)
                .with_frames({ 41, 42, 43 })
                .with_description(L"オマイゴッド")
                .with_bounding_box(0.0f, 1.0f, 2.0f, 42.0f, 43.0f, 44.0f)
                .build();
            detail::io_traits<HANDLE> io(retval.c_str(), GENERIC_READ | GENERIC_WRITE, 0, CREATE_ALWAYS);
            data.save(io);

            return retval;
        }

    };
}
