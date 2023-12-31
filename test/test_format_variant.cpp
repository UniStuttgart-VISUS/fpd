// <copyright file="test_format_variant.cpp" company="Visualisierungsinstitut der Universität Stuttgart">
// Copyright © 2023 Visualisierungsinstitut der Universität Stuttgart.
// Licensed under the MIT licence. See LICENCE file for details.
// </copyright>
// <author>Christoph Müller</author>

#include "pch.h"


namespace fpd {

    TEST_CLASS(test_format_variant) {

    public:

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

    };
}
