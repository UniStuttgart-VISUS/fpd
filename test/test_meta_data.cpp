// <copyright file="test_meta_data.cpp" company="Visualisierungsinstitut der Universität Stuttgart">
// Copyright © 2023 Visualisierungsinstitut der Universität Stuttgart.
// Licensed under the MIT licence. See LICENCE file for details.
// </copyright>
// <author>Christoph Müller</author>

#include "pch.h"


namespace fpd {

    TEST_CLASS(test_meta_data) {

    public:

        TEST_METHOD(default_ctor) {
            meta_data<wchar_t> md;
            Assert::AreEqual(int(endian::undefined), int(md.endian()), L"endian", LINE_INFO());
            Assert::AreEqual(std::uint64_t(0), md.file_size(), L"file_size", LINE_INFO());
            Assert::AreEqual(std::size_t(0), md.format_description().count_elements(), L"format elements", LINE_INFO());
            Assert::AreEqual(std::size_t(0), md.format_description().count_groups(), L"format groups", LINE_INFO());
            Assert::AreEqual(std::size_t(0), md.frames(), L"frames", LINE_INFO());
            Assert::IsTrue(md.frame_table().empty(), L"frame_table", LINE_INFO());
            Assert::AreEqual(std::uint8_t(0), md.version().first, L"version", LINE_INFO());
            Assert::AreEqual(std::uint8_t(0), md.version().second, L"version", LINE_INFO());
        }

        TEST_METHOD(check_header_size) {
            Assert::ExpectException<std::invalid_argument>([](void) {
                meta_data<char> dummy;
                meta_data<char>::check_header_size(dummy, nullptr, meta_data<char>::fixed_header_size);
            }, L"invalid data pointer", LINE_INFO());

            Assert::ExpectException<std::invalid_argument>([](void) {
                std::array<std::uint8_t, 1> data;
                meta_data<char> dummy;
                meta_data<char>::check_header_size(dummy, data.data(), data.size());
            }, L"Too few data", LINE_INFO());

            Assert::ExpectException<std::invalid_argument>([](void) {
                std::array<std::uint8_t, meta_data<char>::fixed_header_size> data;
                meta_data<char> dummy;
                ::memset(data.data(), 0, data.size());
                meta_data<char>::check_header_size(dummy, data.data(), data.size());
            }, L"Invalid header", LINE_INFO());

            {
                std::array<std::uint8_t, meta_data<char>::fixed_header_size> data;
                meta_data<char> md;
                data[0] = 'F';
                data[1] = 'P';
                data[2] = 'D';
                data[3] = FPD_FILE_MAJOR;
                data[4] = FPD_FILE_MINOR;
                *reinterpret_cast<endian *>(data.data() + 5) = endian::system;
                *reinterpret_cast<std::uint32_t *>(data.data() + 5 + sizeof(endian)) = 42;
                Assert::AreEqual(std::size_t(42), meta_data<char>::check_header_size(md, data.data(), data.size()), L"Correct segments size", LINE_INFO());
                Assert::AreEqual(std::uint8_t(FPD_FILE_MAJOR), md.version().first, L"major version", LINE_INFO());
                Assert::AreEqual(std::uint8_t(FPD_FILE_MINOR), md.version().second, L"minor version", LINE_INFO());
            }
        }

        TEST_METHOD(builder_wchar_builtin) {
            meta_data_builder<wchar_t> builder;

            builder.with_description(L"description")
                .using_code_page(code_page::ansi)
                .with_endian(endian::system)
                .with_version(FPD_FILE_MAJOR, FPD_FILE_MINOR)
                .with_bounding_box(0.0f, 1.0f, 2.0f, 10.0f, 11.0f, 12.0f)
                .with_frames(42)
                .add_format_element(semantic::position, nullptr, format::float32xyz);

            {
                const std::size_t expected
                    = 3                         // magic cookie
                    + 1 + 1                     // file version
                    + sizeof(endian)            // BOM
                    + sizeof(std::uint32_t)     // header size

                    // Frame offset table.
                    + sizeof(header_segment)
                    + 42 * sizeof(std::uint64_t)

                    // One format element
                    + sizeof(header_segment)
                    + element<wchar_t>::size_on_disk

                    // Single bounding box.
                    + sizeof(header_segment)
                    + 6 * sizeof(float)

                    // file description
                    + sizeof(header_segment)
                    + (::wcslen(L"description") + 1) * sizeof(char16_t);

                Assert::AreEqual(expected, builder.size(), L"Overall header size", LINE_INFO());
            }

            Assert::IsTrue(builder, L"Builder valid", LINE_INFO());
            auto data = builder.build();
            Assert::IsFalse(builder, L"Builder reset", LINE_INFO());
        }

        TEST_METHOD(builder_char_user_defined) {
            meta_data_builder<char> builder;

            builder.using_code_page(code_page::ansi)
                .with_endian(endian::system)
                .with_frames(1)
                .add_format_element(semantic::user_defined, "float3", format::float32xyz);

            {
                const std::size_t expected
                    = 3                         // magic cookie
                    + 1 + 1                     // file version
                    + sizeof(endian)            // BOM
                    + sizeof(std::uint32_t)     // header size

                    // Frame offset table.
                    + sizeof(header_segment)
                    + 1 * sizeof(std::uint64_t)

                    // One format element
                    + sizeof(header_segment)
                    + element<wchar_t>::size_on_disk

                    // User-defined format table.
                    + sizeof(header_segment)
                    + (::wcslen(L"float3") + 1) * sizeof(char16_t);

                Assert::AreEqual(expected, builder.size(), L"Overall header size", LINE_INFO());
            }

            Assert::IsTrue(builder, L"Builder valid", LINE_INFO());
            auto data = builder.build();
            Assert::IsFalse(builder, L"Builder reset", LINE_INFO());
        }
    };
}
