// <copyright file="test_meta_data.cpp" company="Visualisierungsinstitut der Universität Stuttgart">
// Copyright © 2023 - 2025 Visualisierungsinstitut der Universität Stuttgart.
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
                .using_code_page(encodings::ansi)
                .with_version(FPD_FILE_MAJOR, FPD_FILE_MINOR)
                .with_bounding_box(0.0f, 1.0f, 2.0f, 10.0f, 11.0f, 12.0f)
                .with_uniform_frames(42, 1)
                .add_format_element(semantic::position, nullptr, format::float32xyz);

            {
                const std::uint64_t expected
                    = 3                         // magic cookie
                    + 1 + 1                     // file version
                    + sizeof(endian)            // BOM
                    + sizeof(std::uint32_t)     // header size

                    // Frame table.
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
                    + (::wcslen(L"description") + 1) * sizeof(char);

                Assert::AreEqual(expected, builder.size(), L"Overall header size", LINE_INFO());
            }

            Assert::IsTrue(builder, L"Builder valid", LINE_INFO());
            auto data = builder.build();
            Assert::IsFalse(builder, L"Builder reset", LINE_INFO());
        }

        TEST_METHOD(builder_char_user_defined) {
            meta_data_builder<char> builder;

            Assert::IsFalse(builder, L"Initial state is invalid", LINE_INFO());

            builder.using_code_page(encodings::ansi)
                .with_frames({ 42 })
                .add_format_element(semantic::user_defined, "float3", format::float32xyz);

            const std::uint64_t expected_header_size
                = 3                         // magic cookie
                + 1 + 1                     // file version
                + sizeof(endian)            // BOM
                + sizeof(std::uint32_t)     // header size

                // Frame table.
                + sizeof(header_segment)
                + 1 * sizeof(std::uint64_t)

                // One format element
                + sizeof(header_segment)
                + element<wchar_t>::size_on_disk

                // User-defined format table.
                + sizeof(header_segment)
                + (::wcslen(L"float3") + 1) * sizeof(char);

            Assert::AreEqual(expected_header_size, builder.size(), L"Overall header size", LINE_INFO());

            Assert::IsTrue(builder, L"Builder valid", LINE_INFO());
            auto data = builder.build();
            Assert::IsFalse(builder, L"Builder reset", LINE_INFO());

            Assert::AreEqual(int(FPD_FILE_MAJOR), int(data.version().first), L"major version", LINE_INFO());
            Assert::AreEqual(int(FPD_FILE_MINOR), int(data.version().second), L"minor version", LINE_INFO());
            Assert::AreEqual(int(endian::system), int(data.endian()), L"endian", LINE_INFO());
            Assert::AreEqual(std::size_t(1), data.frames(), L"# of frames", LINE_INFO());
            Assert::AreEqual(expected_header_size, data.frame_offset(0), L"First frame after header", LINE_INFO());
            Assert::AreEqual(std::uint64_t(42), data.particles(0), L"particles in frame", LINE_INFO());
            Assert::AreEqual(std::uint64_t(42 * 3 * sizeof(float)), data.frame_size(0), L"size of frame", LINE_INFO());
            Assert::IsTrue(data.global_particle_properties().empty(), L"No global properties", LINE_INFO());
            Assert::AreEqual(std::size_t(1), data.format_description().count_elements(), L"# of format elements", LINE_INFO());
            Assert::AreEqual(std::size_t(1), data.format_description().count_groups(), L"# of format groups", LINE_INFO());
        }

        TEST_METHOD(frame_table_from_initialiser) {
            meta_data_builder<wchar_t> builder;
            builder.add_format_element(semantic::position, nullptr, format::float32xyz);
            builder.add_format_element(semantic::colour, nullptr, format::uint32xyzw);
            builder.with_frames({ 42, 17, 12 });

            {
                constexpr auto expected
                    = 3                         // magic cookie
                    + 1 + 1                     // file version
                    + sizeof(endian)            // BOM
                    + sizeof(std::uint32_t)     // header size

                    // Frame table.
                    + sizeof(header_segment)
                    + 3 * sizeof(std::uint64_t)

                    // Two format elements
                    + sizeof(header_segment)
                    + 2 * element<wchar_t>::size_on_disk;
                Assert::AreEqual(expected, builder.size(), L"Overall header size", LINE_INFO());
            }

            auto start_of_data = builder.size();

            auto data = builder.build();
            Assert::AreEqual(std::size_t(3), data.frames(), L"# of frames", LINE_INFO());

            Assert::AreEqual(std::uint64_t(42), data.particles(0), L"particles in frame 0", LINE_INFO());
            Assert::AreEqual(start_of_data, data.frame_offset(0), L"start of frame 0", LINE_INFO());
            Assert::AreEqual(std::uint64_t(42 * (3 * sizeof(float) + 4 * sizeof(std::uint32_t))), data.frame_size(0), L"size of frame 0", LINE_INFO());

            start_of_data += data.frame_size(0);

            Assert::AreEqual(std::uint64_t(17), data.particles(1), L"particles in frame 1", LINE_INFO());
            Assert::AreEqual(start_of_data, data.frame_offset(1), L"start of frame 1", LINE_INFO());
            Assert::AreEqual(std::uint64_t(17 * (3 * sizeof(float) + 4 * sizeof(std::uint32_t))), data.frame_size(1), L"size of frame 1", LINE_INFO());

            start_of_data += data.frame_size(1);

            Assert::AreEqual(std::uint64_t(12), data.particles(2), L"particles in frame 2", LINE_INFO());
            Assert::AreEqual(start_of_data, data.frame_offset(2), L"start of frame 2", LINE_INFO());
            Assert::AreEqual(std::uint64_t(12 * (3 * sizeof(float) + 4 * sizeof(std::uint32_t))), data.frame_size(2), L"size of frame 2", LINE_INFO());
        }

        TEST_METHOD(measure_semantic) {
            meta_data_builder<wchar_t> builder;
            builder.add_format_element(user_defined_semantic(0), L"PÖSÜTÜN", format::float32xyz);
            builder.add_format_element(user_defined_semantic(1), L"色", format::uint32xyzw);
            builder.with_uniform_frames(1, 1);

            {
                const auto expected
                    = 3                         // magic cookie
                    + 1 + 1                     // file version
                    + sizeof(endian)            // BOM
                    + sizeof(std::uint32_t)     // header size

                    // Frame table.
                    + sizeof(header_segment)
                    + 1 * sizeof(std::uint64_t)

                    // Two format elements
                    + sizeof(header_segment)
                    + 2 * element<wchar_t>::size_on_disk

                    // User-defined format table.
                    + sizeof(header_segment)
                    + 11
                    + 4;
                Assert::AreEqual(expected, builder.size(), L"Overall header size", LINE_INFO());
            }


        }
    };
}
