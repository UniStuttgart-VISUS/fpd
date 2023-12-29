// <copyright file="test_type_size.cpp" company="Visualisierungsinstitut der Universität Stuttgart">
// Copyright © 2023 Visualisierungsinstitut der Universität Stuttgart.
// Licensed under the MIT licence. See LICENCE file for details.
// </copyright>
// <author>Christoph Müller</author>

#include "pch.h"


namespace fpd {

    TEST_CLASS(test_type_size) {

    public:

        TEST_METHOD(normal) {
            Assert::AreEqual(sizeof(int), type_size<int>::value, L"int", LINE_INFO());
            Assert::AreEqual(sizeof(std::uint8_t), type_size<std::uint8_t>::value, L"std::uint8_t", LINE_INFO());
            Assert::AreEqual(sizeof(float), type_size<int>::value, L"float", LINE_INFO());
        }

        TEST_METHOD(specialisations) {
            Assert::AreEqual(std::size_t(0), type_size<void>::value, L"void", LINE_INFO());
        }
    };
}
