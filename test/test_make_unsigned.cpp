// <copyright file="test_make_unsigned.cpp" company="Visualisierungsinstitut der Universität Stuttgart">
// Copyright © 2025 Visualisierungsinstitut der Universität Stuttgart.
// Licensed under the MIT licence. See LICENCE file for details.
// </copyright>
// <author>Christoph Müller</author>

#include "pch.h"


namespace fpd {

    TEST_CLASS(test_make_unsigned) {

    public:

        TEST_METHOD(int32) {
            Assert::IsTrue(std::is_same_v<std::uint32_t, detail::make_unsigned_t<int32_t>>, L"int32_t is converted to uint32_t", LINE_INFO());
        }

        TEST_METHOD(float32) {
            Assert::IsTrue(std::is_same_v<std::uint32_t, detail::make_unsigned_t<float>>, L"float is converted to uint32_t", LINE_INFO());
        }

        TEST_METHOD(float64) {
            Assert::IsTrue(std::is_same_v<std::uint64_t, detail::make_unsigned_t<double>>, L"double is converted to uint64_t", LINE_INFO());
        }
    };
}
