// <copyright file="test_mmpld.cpp" company="Visualisierungsinstitut der Universität Stuttgart">
// Copyright © 2025 Visualisierungsinstitut der Universität Stuttgart.
// Licensed under the MIT licence. See LICENCE file for details.
// </copyright>
// <author>Christoph Müller</author>

#include "pch.h"


namespace fpd {

    TEST_CLASS(test_mmpld) {

    public:

        TEST_METHOD(test_mmpld_colour_none) {
            constexpr auto mmpld = mmpld::colour_type::none;
            const auto fpd = mmpld_format(mmpld);
            Assert::AreEqual(int(format::unknown), int(fpd), L"FPD format", LINE_INFO());

            const auto sem = mmpld_semantic(mmpld);
            Assert::AreEqual(int(semantic::unknown), int(sem), L"FPD semantic", LINE_INFO());
        }

        TEST_METHOD(test_mmpld_rgb8) {
            constexpr auto mmpld = mmpld::colour_type::rgb8;
            mmpld::colour_properties props;
            Assert::IsTrue(mmpld::get_properties(mmpld, props), L"MMPLD properties", LINE_INFO());

            const auto fpd = mmpld_format(mmpld);
            Assert::AreEqual(int(format::uint8xyz), int(fpd), L"FPD format", LINE_INFO());
            Assert::AreEqual(to_size(fpd), props.size, L"Same size", LINE_INFO());

            const auto sem = mmpld_semantic(mmpld);
            Assert::AreEqual(int(semantic::colour), int(sem), L"FPD semantic", LINE_INFO());
        }

        TEST_METHOD(test_mmpld_rgba8) {
            constexpr auto mmpld = mmpld::colour_type::rgba8;
            mmpld::colour_properties props;
            Assert::IsTrue(mmpld::get_properties(mmpld, props), L"MMPLD properties", LINE_INFO());

            const auto fpd = mmpld_format(mmpld);
            Assert::AreEqual(int(format::uint8xyzw), int(fpd), L"FPD format", LINE_INFO());
            Assert::AreEqual(to_size(fpd), props.size, L"Same size", LINE_INFO());

            const auto sem = mmpld_semantic(mmpld);
            Assert::AreEqual(int(semantic::colour), int(sem), L"FPD semantic", LINE_INFO());
        }

        TEST_METHOD(test_mmpld_intensity) {
            constexpr auto mmpld = mmpld::colour_type::intensity;
            mmpld::colour_properties props;
            Assert::IsTrue(mmpld::get_properties(mmpld, props), L"MMPLD properties", LINE_INFO());

            const auto fpd = mmpld_format(mmpld);
            Assert::AreEqual(int(format::float32), int(fpd), L"FPD format", LINE_INFO());
            Assert::AreEqual(to_size(fpd), props.size, L"Same size", LINE_INFO());

            const auto sem = mmpld_semantic(mmpld);
            Assert::AreEqual(int(semantic::intensity), int(sem), L"FPD semantic", LINE_INFO());
        }

        TEST_METHOD(test_mmpld_rgb32) {
            constexpr auto mmpld = mmpld::colour_type::rgb32;
            mmpld::colour_properties props;
            Assert::IsTrue(mmpld::get_properties(mmpld, props), L"MMPLD properties", LINE_INFO());

            const auto fpd = mmpld_format(mmpld);
            Assert::AreEqual(int(format::float32xyz), int(fpd), L"FPD format", LINE_INFO());
            Assert::AreEqual(to_size(fpd), props.size, L"Same size", LINE_INFO());

            const auto sem = mmpld_semantic(mmpld);
            Assert::AreEqual(int(semantic::colour), int(sem), L"FPD semantic", LINE_INFO());
        }

        TEST_METHOD(test_mmpld_rgba32) {
            constexpr auto mmpld = mmpld::colour_type::rgba32;
            mmpld::colour_properties props;
            Assert::IsTrue(mmpld::get_properties(mmpld, props), L"MMPLD properties", LINE_INFO());

            const auto fpd = mmpld_format(mmpld);
            Assert::AreEqual(int(format::float32xyzw), int(fpd), L"FPD format", LINE_INFO());
            Assert::AreEqual(to_size(fpd), props.size, L"Same size", LINE_INFO());

            const auto sem = mmpld_semantic(mmpld);
            Assert::AreEqual(int(semantic::colour), int(sem), L"FPD semantic", LINE_INFO());
        }

        TEST_METHOD(test_mmpld_rgba16) {
            constexpr auto mmpld = mmpld::colour_type::rgba16;
            mmpld::colour_properties props;
            Assert::IsTrue(mmpld::get_properties(mmpld, props), L"MMPLD properties", LINE_INFO());

            const auto fpd = mmpld_format(mmpld);
            Assert::AreEqual(int(format::uint16xyzw), int(fpd), L"FPD format", LINE_INFO());
            Assert::AreEqual(to_size(fpd), props.size, L"Same size", LINE_INFO());

            const auto sem = mmpld_semantic(mmpld);
            Assert::AreEqual(int(semantic::colour), int(sem), L"FPD semantic", LINE_INFO());
        }

        TEST_METHOD(test_mmpld_intensity64) {
            constexpr auto mmpld = mmpld::colour_type::intensity64;
            mmpld::colour_properties props;
            Assert::IsTrue(mmpld::get_properties(mmpld, props), L"MMPLD properties", LINE_INFO());

            const auto fpd = mmpld_format(mmpld);
            Assert::AreEqual(int(format::float64), int(fpd), L"FPD format", LINE_INFO());
            Assert::AreEqual(to_size(fpd), props.size, L"Same size", LINE_INFO());

            const auto sem = mmpld_semantic(mmpld);
            Assert::AreEqual(int(semantic::intensity), int(sem), L"FPD semantic", LINE_INFO());
        }

        TEST_METHOD(test_mmpld_vertex_none) {
            constexpr auto mmpld = mmpld::vertex_type::none;
            const auto fpd = mmpld_format(mmpld);
            Assert::AreEqual(int(format::unknown), int(fpd), L"FPD format", LINE_INFO());

            const auto sem = mmpld_semantic(mmpld);
            Assert::AreEqual(int(semantic::unknown), int(sem), L"FPD semantic", LINE_INFO());
        }

        TEST_METHOD(test_mmpld_float_xyz) {
            constexpr auto mmpld = mmpld::vertex_type::float_xyz;
            mmpld::vertex_properties props;
            Assert::IsTrue(mmpld::get_properties(mmpld, props), L"MMPLD properties", LINE_INFO());

            const auto fpd = mmpld_format(mmpld);
            Assert::AreEqual(int(format::float32xyz), int(fpd), L"FPD format", LINE_INFO());
            Assert::AreEqual(to_size(fpd), props.size, L"Same size", LINE_INFO());

            const auto sem = mmpld_semantic(mmpld);
            Assert::AreEqual(int(semantic::position), int(sem), L"FPD semantic", LINE_INFO());
        }

        TEST_METHOD(test_mmpld_float_xyzr) {
            constexpr auto mmpld = mmpld::vertex_type::float_xyzr;
            mmpld::vertex_properties props;
            Assert::IsTrue(mmpld::get_properties(mmpld, props), L"MMPLD properties", LINE_INFO());

            const auto fpd = mmpld_format(mmpld);
            Assert::AreEqual(int(format::float32xyzw), int(fpd), L"FPD format", LINE_INFO());
            Assert::AreEqual(to_size(fpd), props.size, L"Same size", LINE_INFO());

            const auto sem = mmpld_semantic(mmpld);
            Assert::AreEqual(int(semantic::position_radius), int(sem), L"FPD semantic", LINE_INFO());
        }

        TEST_METHOD(test_mmpld_short_xyz) {
            constexpr auto mmpld = mmpld::vertex_type::short_xyz;
            mmpld::vertex_properties props;
            Assert::IsTrue(mmpld::get_properties(mmpld, props), L"MMPLD properties", LINE_INFO());

            const auto fpd = mmpld_format(mmpld);
            Assert::AreEqual(int(format::int16xyz), int(fpd), L"FPD format", LINE_INFO());
            Assert::AreEqual(to_size(fpd), props.size, L"Same size", LINE_INFO());

            const auto sem = mmpld_semantic(mmpld);
            Assert::AreEqual(int(semantic::position), int(sem), L"FPD semantic", LINE_INFO());
        }

        TEST_METHOD(test_mmpld_double_xyz) {
            constexpr auto mmpld = mmpld::vertex_type::double_xyz;
            mmpld::vertex_properties props;
            Assert::IsTrue(mmpld::get_properties(mmpld, props), L"MMPLD properties", LINE_INFO());

            const auto fpd = mmpld_format(mmpld);
            Assert::AreEqual(int(format::float64xyz), int(fpd), L"FPD format", LINE_INFO());
            Assert::AreEqual(to_size(fpd), props.size, L"Same size", LINE_INFO());

            const auto sem = mmpld_semantic(mmpld);
            Assert::AreEqual(int(semantic::position), int(sem), L"FPD semantic", LINE_INFO());
        }

        TEST_METHOD(test_mmpld_formats) {
            std::vector<element_group<char>> formats;
            std::vector<mmpld::list_header> lists;

            lists.emplace_back();
            lists.back().vertex_type = mmpld::vertex_type::float_xyz;
            lists.back().colour_type = mmpld::colour_type::rgb8;
            lists.back().radius = 42.0f;

            lists.emplace_back();
            lists.back().vertex_type = mmpld::vertex_type::float_xyzr;
            lists.back().colour_type = mmpld::colour_type::intensity32;

            lists.emplace_back();
            lists.back().vertex_type = mmpld::vertex_type::float_xyz;
            lists.back().colour_type = mmpld::colour_type::rgb8;
            lists.back().radius = 42.0f;

            lists.emplace_back();
            lists.back().vertex_type = mmpld::vertex_type::float_xyz;
            lists.back().colour_type = mmpld::colour_type::rgb8;
            lists.back().radius = 42.0f;

            lists.emplace_back();
            lists.back().vertex_type = mmpld::vertex_type::float_xyzr;
            lists.back().colour_type = mmpld::colour_type::intensity32;

            Assert::AreEqual(std::size_t(2), mmpld_formats(std::back_inserter(formats), lists.begin(), lists.end()), L"# distinct elements", LINE_INFO());
        }

        TEST_METHOD(test_mmpld_format) {
            std::vector<mmpld::list_header> lists;
            Assert::ExpectException<std::invalid_argument>([lists](void) {
                mmpld_format<char>(lists.begin(), lists.end());
            }, L"check for invalid input", LINE_INFO());

            lists.emplace_back();
            lists.back().vertex_type = mmpld::vertex_type::float_xyz;
            lists.back().colour_type = mmpld::colour_type::none;
            lists.back().radius = 42.0f;

            {
                auto format = mmpld_format<char>(lists.begin(), lists.end());
                Assert::AreEqual(std::size_t(1), format.count(), L"one element", LINE_INFO());
                Assert::AreEqual(std::uint16_t(3 * sizeof(float)), format.size(), L"size of format", LINE_INFO());
                Assert::AreEqual(int(semantic::position), int(format[0].semantic()), L"vertex semantic", LINE_INFO());
                Assert::AreEqual(int(format::float32xyz), int(format[0].format()), L"vertex format", LINE_INFO());
            }

            lists.emplace_back();
            lists.back().vertex_type = mmpld::vertex_type::float_xyzr;
            lists.back().colour_type = mmpld::colour_type::none;

            {
                auto format = mmpld_format<char>(lists.begin(), lists.end());
                Assert::AreEqual(std::size_t(1), format.count(), L"one element", LINE_INFO());
                Assert::AreEqual(std::uint16_t(4 * sizeof(float)), format.size(), L"size of format", LINE_INFO());
                Assert::AreEqual(int(semantic::position_radius), int(format[0].semantic()), L"vertex semantic", LINE_INFO());
                Assert::AreEqual(int(format::float32xyzw), int(format[0].format()), L"vertex format", LINE_INFO());
            }

            lists.emplace_back();
            lists.back().vertex_type = mmpld::vertex_type::float_xyz;
            lists.back().colour_type = mmpld::colour_type::intensity;

            {
                auto format = mmpld_format<char>(lists.begin(), lists.end());
                Assert::AreEqual(std::size_t(2), format.count(), L"two element2", LINE_INFO());
                Assert::AreEqual(std::uint16_t(4 * sizeof(float) + sizeof(float)), format.size(), L"size of format", LINE_INFO());
                Assert::AreEqual(int(semantic::position_radius), int(format[0].semantic()), L"vertex semantic", LINE_INFO());
                Assert::AreEqual(int(format::float32xyzw), int(format[0].format()), L"vertex format", LINE_INFO());
                Assert::AreEqual(int(semantic::intensity), int(format[1].semantic()), L"colour semantic", LINE_INFO());
                Assert::AreEqual(int(format::float32), int(format[1].format()), L"colour format", LINE_INFO());
            }

            lists.emplace_back();
            lists.back().vertex_type = mmpld::vertex_type::float_xyz;
            lists.back().colour_type = mmpld::colour_type::rgb8;

            {
                auto format = mmpld_format<char>(lists.begin(), lists.end());
                Assert::AreEqual(std::size_t(2), format.count(), L"two element2", LINE_INFO());
                Assert::AreEqual(std::uint16_t(4 * sizeof(float) + 3 * sizeof(float)), format.size(), L"size of format", LINE_INFO());
                Assert::AreEqual(int(semantic::position_radius), int(format[0].semantic()), L"vertex semantic", LINE_INFO());
                Assert::AreEqual(int(format::float32xyzw), int(format[0].format()), L"vertex format", LINE_INFO());
                Assert::AreEqual(int(semantic::colour), int(format[1].semantic()), L"colour semantic", LINE_INFO());
                Assert::AreEqual(int(format::float32xyz), int(format[1].format()), L"colour format", LINE_INFO());
            }
        }

        TEST_METHOD(test_mmpld_list) {
            {
                const format_description<char> format {
                    { semantic::position, format::float32xyz }
                };
                const auto list = mmpld_list(format);
                Assert::AreEqual(int(mmpld::vertex_type::float_xyz), int(list.vertex_type), L"vertex type", LINE_INFO());
                Assert::AreEqual(int(mmpld::colour_type::none), int(list.colour_type), L"colour type", LINE_INFO());
            }

            {
                const format_description<char> format{
                    { semantic::position_radius, format::float32xyzw }
                };
                const auto list = mmpld_list(format);
                Assert::AreEqual(int(mmpld::vertex_type::float_xyzr), int(list.vertex_type), L"vertex type", LINE_INFO());
                Assert::AreEqual(int(mmpld::colour_type::none), int(list.colour_type), L"colour type", LINE_INFO());
            }

            {
                const format_description<char> format{
                    { semantic::position, format::float32xyz },
                    { semantic::intensity, format::float32 }
                };
                const auto list = mmpld_list(format);
                Assert::AreEqual(int(mmpld::vertex_type::float_xyz), int(list.vertex_type), L"vertex type", LINE_INFO());
                Assert::AreEqual(int(mmpld::colour_type::intensity), int(list.colour_type), L"colour type", LINE_INFO());
            }

            {
                const format_description<char> format{
                    { semantic::position, format::float32xyz },
                    { semantic::colour, format::uint8xyz }
                };
                const auto list = mmpld_list(format);
                Assert::AreEqual(int(mmpld::vertex_type::float_xyz), int(list.vertex_type), L"vertex type", LINE_INFO());
                Assert::AreEqual(int(mmpld::colour_type::rgb8), int(list.colour_type), L"colour type", LINE_INFO());
            }

            {
                const format_description<char> format{
                    { semantic::position, format::float32xyz },
                    { semantic::colour, format::uint8xyzw }
                };
                const auto list = mmpld_list(format);
                Assert::AreEqual(int(mmpld::vertex_type::float_xyz), int(list.vertex_type), L"vertex type", LINE_INFO());
                Assert::AreEqual(int(mmpld::colour_type::rgba8), int(list.colour_type), L"colour type", LINE_INFO());
            }

            {
                const format_description<char> format{
                    { semantic::position, format::float32xyz },
                    { semantic::radius, format::float32 },
                    { semantic::colour, format::uint8xyz }
                };
                const auto list = mmpld_list(format);
                Assert::AreEqual(int(mmpld::vertex_type::float_xyzr), int(list.vertex_type), L"vertex type", LINE_INFO());
                Assert::AreEqual(int(mmpld::colour_type::rgb8), int(list.colour_type), L"colour type", LINE_INFO());
            }

            {
                const format_description<char> format{
                    { semantic::position, format::int16xyz },
                    { semantic::colour, format::float32xyzw }
                };
                const auto list = mmpld_list(format);
                Assert::AreEqual(int(mmpld::vertex_type::short_xyz), int(list.vertex_type), L"vertex type", LINE_INFO());
                Assert::AreEqual(int(mmpld::colour_type::rgba32), int(list.colour_type), L"colour type", LINE_INFO());
            }

            {
                const format_description<char> format{
                    { semantic::position, format::float64xyz },
                    { semantic::intensity, format::float64 }
                };
                const auto list = mmpld_list(format);
                Assert::AreEqual(int(mmpld::vertex_type::double_xyz), int(list.vertex_type), L"vertex type", LINE_INFO());
                Assert::AreEqual(int(mmpld::colour_type::intensity64), int(list.colour_type), L"colour type", LINE_INFO());
            }

            {
                const format_description<char> format{
                    { semantic::position, format::float32xyz },
                    { semantic::colour, format::uint8xyz },
                    { semantic::intensity, format::float32 }
                };
                Assert::ExpectException<std::invalid_argument>([format](void) {
                    mmpld_list(format);
                }, L"colour and intensity are not allowed at the same time", LINE_INFO());
            }

            {
                const format_description<char> format{
                    { semantic::position, format::float32xyz },
                    { semantic::position, format::float32xyz }
                };
                Assert::ExpectException<std::invalid_argument>([format](void) {
                    mmpld_list(format);
                }, L"multiple positions are not allowed", LINE_INFO());
            }

            {
                const format_description<char> format{
                    { semantic::intensity, format::float32 },
                    { semantic::position, format::float32xyz }
                };
                Assert::ExpectException<std::invalid_argument>([format](void) {
                    mmpld_list(format);
                }, L"intensity cannot be before positon", LINE_INFO());
            }

            {
                const format_description<char> format{
                    { semantic::colour, format::float32xyzw },
                    { semantic::position, format::float32xyz }
                };
                Assert::ExpectException<std::invalid_argument>([format](void) {
                    mmpld_list(format);
                }, L"colour cannot be before positon", LINE_INFO());
            }

            {
                const format_description<char> format{
                    { semantic::position_radius, format::float32xyzw },
                    { semantic::radius, format::float32 }
                };
                Assert::ExpectException<std::invalid_argument>([format](void) {
                    mmpld_list(format);
                }, L"radius cannot be specified twice", LINE_INFO());
            }

            {
                const format_description<char> format{
                    { semantic::position, format::float32xyz },
                    { semantic::colour, format::uint8xyzw },
                    { semantic::radius, format::float32 }
                };
                Assert::ExpectException<std::invalid_argument>([format](void) {
                    mmpld_list(format);
                }, L"radius must follow position", LINE_INFO());
            }

            {
                const format_description<char> format{
                    { semantic::position, format::int16xyz },
                    { semantic::radius, format::float32 }
                };
                Assert::ExpectException<std::invalid_argument>([format](void) {
                    mmpld_list(format);
                }, L"separate radius is not supported for int16", LINE_INFO());
            }

        }
    };
}
