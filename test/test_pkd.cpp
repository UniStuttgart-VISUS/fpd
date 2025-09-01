// <copyright file="test_pkd.cpp" company="Visualisierungsinstitut der Universität Stuttgart">
// Copyright © 2024 Visualisierungsinstitut der Universität Stuttgart.
// Licensed under the MIT licence. See LICENCE file for details.
// </copyright>
// <author>Christoph Müller</author>

#include "pch.h"


namespace fpd {

    TEST_CLASS(test_pkd) {

    public:

        TEST_METHOD(ospray_pkd_children) {
            Assert::AreEqual(std::size_t(1), detail::ospray_pkd_left_child(0), L"left child of 0", LINE_INFO());
            Assert::AreEqual(std::size_t(2), detail::ospray_pkd_right_child(0), L"right child of 0", LINE_INFO());
            Assert::AreEqual(std::size_t(3), detail::ospray_pkd_left_child(1), L"left child of 1", LINE_INFO());
            Assert::AreEqual(std::size_t(4), detail::ospray_pkd_right_child(1), L"right child of 1", LINE_INFO());
            Assert::AreEqual(std::size_t(5), detail::ospray_pkd_left_child(2), L"left child of 2", LINE_INFO());
            Assert::AreEqual(std::size_t(6), detail::ospray_pkd_right_child(2), L"right child of 2", LINE_INFO());
            Assert::AreEqual(std::size_t(9), detail::ospray_pkd_left_child(4), L"left child of 4", LINE_INFO());
            Assert::AreEqual(std::size_t(10), detail::ospray_pkd_right_child(4), L"right child of 4", LINE_INFO());
            Assert::AreEqual(std::size_t(11), detail::ospray_pkd_left_child(5), L"left child of 5", LINE_INFO());
            Assert::AreEqual(std::size_t(12), detail::ospray_pkd_right_child(5), L"right child of 5", LINE_INFO());
        }

        TEST_METHOD(ospray_pkd_iterator) {
            {
                detail::ospray_pkd_iterator it(0);
                Assert::AreEqual(std::size_t(0), std::size_t(it), L"iterate 0 from 0", LINE_INFO());
                Assert::AreEqual(std::size_t(1), std::size_t(++it), L"iterate 1 from 0", LINE_INFO());
                Assert::AreEqual(std::size_t(2), std::size_t(++it), L"iterate 2 from 0", LINE_INFO());
                Assert::AreEqual(std::size_t(3), std::size_t(++it), L"iterate 3 from 0", LINE_INFO());
                Assert::AreEqual(std::size_t(4), std::size_t(++it), L"iterate 4 from 0", LINE_INFO());
                Assert::AreEqual(std::size_t(5), std::size_t(++it), L"iterate 5 from 0", LINE_INFO());
                Assert::AreEqual(std::size_t(6), std::size_t(++it), L"iterate 6 from 0", LINE_INFO());
            }

            {
                detail::ospray_pkd_iterator it(1);
                Assert::AreEqual(std::size_t(1), std::size_t(it), L"iterate 0 from 1", LINE_INFO());
                Assert::AreEqual(std::size_t(3), std::size_t(++it), L"iterate 1 from 1", LINE_INFO());
                Assert::AreEqual(std::size_t(4), std::size_t(++it), L"iterate 2 from 1", LINE_INFO());
                Assert::AreEqual(std::size_t(7), std::size_t(++it), L"iterate 3 from 1", LINE_INFO());
                Assert::AreEqual(std::size_t(8), std::size_t(++it), L"iterate 4 from 1", LINE_INFO());
                Assert::AreEqual(std::size_t(9), std::size_t(++it), L"iterate 5 from 1", LINE_INFO());
                Assert::AreEqual(std::size_t(10), std::size_t(++it), L"iterate 6 from 1", LINE_INFO());
            }

            {
                detail::ospray_pkd_iterator it(2);
                Assert::AreEqual(std::size_t(2), std::size_t(it), L"iterate 0 from 2", LINE_INFO());
                Assert::AreEqual(std::size_t(5), std::size_t(++it), L"iterate 1 from 2", LINE_INFO());
                Assert::AreEqual(std::size_t(6), std::size_t(++it), L"iterate 2 from 2", LINE_INFO());
                Assert::AreEqual(std::size_t(11), std::size_t(++it), L"iterate 3 from 2", LINE_INFO());
                Assert::AreEqual(std::size_t(12), std::size_t(++it), L"iterate 4 from 2", LINE_INFO());
                Assert::AreEqual(std::size_t(13), std::size_t(++it), L"iterate 5 from 2", LINE_INFO());
                Assert::AreEqual(std::size_t(14), std::size_t(++it), L"iterate 6 from 2", LINE_INFO());
            }

            {
                detail::ospray_pkd_iterator it(4);
                Assert::AreEqual(std::size_t(4), std::size_t(it), L"iterate 0 from 4", LINE_INFO());
                Assert::AreEqual(std::size_t(9), std::size_t(++it), L"iterate 1 from 4", LINE_INFO());
                Assert::AreEqual(std::size_t(10), std::size_t(++it), L"iterate 2 from 4", LINE_INFO());
                Assert::AreEqual(std::size_t(19), std::size_t(++it), L"iterate 3 from 4", LINE_INFO());
                Assert::AreEqual(std::size_t(20), std::size_t(++it), L"iterate 4 from 4", LINE_INFO());
                Assert::AreEqual(std::size_t(21), std::size_t(++it), L"iterate 5 from 4", LINE_INFO());
                Assert::AreEqual(std::size_t(22), std::size_t(++it), L"iterate 6 from 4", LINE_INFO());
            }

            {
                detail::ospray_pkd_iterator it(5);
                Assert::AreEqual(std::size_t(5), std::size_t(it), L"iterate 0 from 5", LINE_INFO());
                Assert::AreEqual(std::size_t(11), std::size_t(++it), L"iterate 1 from 5", LINE_INFO());
                Assert::AreEqual(std::size_t(12), std::size_t(++it), L"iterate 2 from 5", LINE_INFO());
                Assert::AreEqual(std::size_t(23), std::size_t(++it), L"iterate 3 from 5", LINE_INFO());
                Assert::AreEqual(std::size_t(24), std::size_t(++it), L"iterate 4 from 5", LINE_INFO());
                Assert::AreEqual(std::size_t(25), std::size_t(++it), L"iterate 5 from 5", LINE_INFO());
                Assert::AreEqual(std::size_t(26), std::size_t(++it), L"iterate 6 from 5", LINE_INFO());
            }
        }

        TEST_METHOD(build) {
            pkd_tree_builder<wchar_t> builder;

            Assert::ExpectException<std::invalid_argument>([&builder](void) {
                builder.for_format(format_description<wchar_t>());
            }, L"invalid format", LINE_INFO());

            Assert::ExpectException<std::invalid_argument>([&builder](void) {
                builder.for_format(format_description<wchar_t>({ { semantic::radius, format::float32x, 0, 0 } }));
            }, L"invalid format", LINE_INFO());

            builder.for_format(format_description<wchar_t>({ { semantic::position_radius, format::float32xyzw, 0, 0 } }));

            std::vector<float> particles = {
                2.0f, 2.0f, 2.0f, 0.1f,
                1.0f, 1.0f, 1.0f, 0.2f,
                4.0f, 4.0f, 4.0f, 0.3f,
                3.0f, 3.0f, 3.0f, 0.4f,
            };

            builder.build(particles.data(), 4);
        }

    };
}
