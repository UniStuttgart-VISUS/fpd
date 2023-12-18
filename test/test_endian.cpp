// <copyright file="test_endian.cpp" company="Visualisierungsinstitut der Universität Stuttgart">
// Copyright © 2023 Visualisierungsinstitut der Universität Stuttgart.
// Licensed under the MIT licence. See LICENCE file for details.
// </copyright>
// <author>Christoph Müller</author>

#include "pch.h"
#include "CppUnitTest.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;


namespace fpd {

    TEST_CLASS(test_endian) {

    public:

        TEST_METHOD(definition) {
            Assert::AreNotEqual(int(endian::undefined), int(endian::big), L"big != undefined", LINE_INFO());
            Assert::AreNotEqual(int(endian::undefined), int(endian::little), L"little != undefined", LINE_INFO());
            Assert::AreNotEqual(int(endian::undefined), int(endian::system), L"system != undefined", LINE_INFO());
        }
    };
}
