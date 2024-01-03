// <copyright file="test_format_variant.cpp" company="Visualisierungsinstitut der Universität Stuttgart">
// Copyright © 2023 Visualisierungsinstitut der Universität Stuttgart.
// Licensed under the MIT licence. See LICENCE file for details.
// </copyright>
// <author>Christoph Müller</author>

#include "pch.h"


namespace fpd {

    TEST_CLASS(test_format_variant) {

    public:

        TEST_METHOD(ctor) {
            {
                format_variant v;
                Assert::IsFalse(bool(v), L"initialised invalid", LINE_INFO());
                Assert::AreEqual(int(v.format()), int(format::unknown), L"unknown", LINE_INFO());
            }
            {
                format_variant v(std::int32_t(12));
                Assert::IsTrue(bool(v), L"initialised valid", LINE_INFO());
                Assert::AreEqual(int(v.format()), int(format::int32), L"int32", LINE_INFO());
            }

        }

        TEST_METHOD(size) {
            {
                basic_format_variant<format_list<>> v;
                Assert::AreEqual(std::size_t(0), v.size(), L"empty", LINE_INFO());
            }

            {
                basic_format_variant<format_list<format::int32>> v;
                Assert::AreEqual(sizeof(std::int32_t), v.size(), L"int", LINE_INFO());
            }

            {
                basic_format_variant<format_list<format::int32, format::float32>> v;
                Assert::AreEqual(sizeof(std::int32_t), v.size(), L"int, float", LINE_INFO());
            }

            {
                basic_format_variant<format_list<format::int32, format::float64>> v;
                Assert::AreEqual(sizeof(double), v.size(), L"int, float, double", LINE_INFO());
            }

            {
                // Note: this might fail in regression tests if a new format was added. Adjust accordingly.
                format_variant v;
                Assert::AreEqual(4 * sizeof(double), v.size(), L"format_variant", LINE_INFO());
            }
        }

        TEST_METHOD(supports) {
            {
                basic_format_variant<format_list<>> v;
                Assert::IsFalse(v.supports(format::float32), L"float32", LINE_INFO());
                Assert::IsFalse(v.supports<format::float32>(), L"float32 comile time", LINE_INFO());
                Assert::IsFalse(v.supports<float>(), L"float", LINE_INFO());
            }

            {
                basic_format_variant<format_list<format::int32, format::float64>> v;

                Assert::IsFalse(v.supports(format::float32), L"float32", LINE_INFO());
                Assert::IsFalse(v.supports<format::float32>(), L"float32 comile time", LINE_INFO());
                Assert::IsFalse(v.supports<float>(), L"float", LINE_INFO());

                Assert::IsTrue(v.supports(format::int32), L"int32", LINE_INFO());
                Assert::IsTrue(v.supports<format::int32>(), L"int32 comile time", LINE_INFO());
                Assert::IsTrue(v.supports<int>(), L"int", LINE_INFO());
            }
        }

    };
}
