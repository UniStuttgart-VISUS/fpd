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

        TEST_METHOD(mmpld_format_global_radius) {
            meta_data_builder<wchar_t> builder;

            mmpld::list_header header;
            header.vertex_type = mmpld::vertex_type::float_xyz;
            header.colour_type = mmpld::colour_type::rgb8;
            header.radius = 0.5f;

            builder.with_mmpld_format(header);
            builder.with_uniform_frames(1, 1);

            auto data = builder.build();

            Assert::AreEqual(std::size_t(2), data.format_description().count_elements(), L"# of format elements", LINE_INFO());
            Assert::AreEqual(int(semantic::position), int(data.format_description()(0, 0).semantic()), L"semantic of element 0", LINE_INFO());
            Assert::AreEqual(int(format::float32xyz), int(data.format_description()(0, 0).format()), L"format of element 0", LINE_INFO());
            Assert::AreEqual(int(semantic::colour), int(data.format_description()(0, 1).semantic()), L"semantic of element 1", LINE_INFO());
            Assert::AreEqual(int(format::uint8xyz), int(data.format_description()(0, 1).format()), L"format of element 1", LINE_INFO());
            Assert::AreEqual(std::size_t(1), data.global_particle_properties().size(), L"# of global properties", LINE_INFO());
            Assert::IsTrue(data.global_particle_properties().find(semantic::radius) != data.global_particle_properties().end(), L"global radius present", LINE_INFO());
            // TODO
//            Assert::IsTrue(format_variant(header.radius) == data.global_particle_properties().at(semantic::radius), L"global radius", LINE_INFO());
        }

        TEST_METHOD(mmpld_format_particle_radius) {
            meta_data_builder<wchar_t> builder;

            mmpld::list_header header;
            header.vertex_type = mmpld::vertex_type::float_xyzr;
            header.colour_type = mmpld::colour_type::rgba32;
            header.radius = 0.5f;

            builder.with_mmpld_format(header);
            builder.with_uniform_frames(1, 1);

            auto data = builder.build();

            Assert::AreEqual(std::size_t(2), data.format_description().count_elements(), L"# of format elements", LINE_INFO());
            Assert::AreEqual(int(semantic::position_radius), int(data.format_description()(0, 0).semantic()), L"semantic of element 0", LINE_INFO());
            Assert::AreEqual(int(format::float32xyzw), int(data.format_description()(0, 0).format()), L"format of element 0", LINE_INFO());
            Assert::AreEqual(int(semantic::colour), int(data.format_description()(0, 1).semantic()), L"semantic of element 1", LINE_INFO());
            Assert::AreEqual(int(format::float32xyzw), int(data.format_description()(0, 1).format()), L"format of element 1", LINE_INFO());
            Assert::AreEqual(std::size_t(0), data.global_particle_properties().size(), L"no global property", LINE_INFO());
        }

        TEST_METHOD(save_roundtrip) {
            meta_data_builder<wchar_t> builder;

            const auto description = L"Test description.";
            builder.add_format_element(semantic::position_radius, nullptr, format::float32xyzw)
                .add_format_element(semantic::intensity, format::float32, 1)
                .with_uniform_frames(42, 10)
                .with_description(description);

            const auto size_on_disk = decltype(builder)::meta_data_type::fixed_header_size
                // Two format elements
                + sizeof(header_segment)
                + 2 * element<wchar_t>::size_on_disk
                // Frame table.
                + sizeof(header_segment)
                + 42 * sizeof(std::uint64_t)
                // Description
                + ::wcslen(description);
            Assert::AreEqual(size_on_disk, builder.size(), L"Overall header size", LINE_INFO());

            const auto expected = builder.build();
            const auto path = get_temp_file();
            win32_io io(path.c_str(), GENERIC_READ | GENERIC_WRITE, 0, CREATE_ALWAYS);

            Assert::AreEqual(size_on_disk, expected.save(io), L"file size", LINE_INFO());
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
    };
}
