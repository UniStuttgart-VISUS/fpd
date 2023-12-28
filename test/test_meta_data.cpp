// <copyright file="test_meta_data.cpp" company="Visualisierungsinstitut der Universität Stuttgart">
// Copyright © 2023 Visualisierungsinstitut der Universität Stuttgart.
// Licensed under the MIT licence. See LICENCE file for details.
// </copyright>
// <author>Christoph Müller</author>

#include "pch.h"
#include "CppUnitTest.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;


namespace fpd {

    TEST_CLASS(test_meta_data) {

    public:

        TEST_METHOD(default_ctor) {
            meta_data<wchar_t> md;
            Assert::AreEqual(int(endian::undefined), int(md.endian()), L"endian", LINE_INFO());
            Assert::AreEqual(std::uint64_t(0), md.file_size(), L"file_size", LINE_INFO());
            Assert::AreEqual(std::size_t(0), md.format_description().count(), L"format_description", LINE_INFO());
            Assert::AreEqual(std::size_t(0), md.frames(), L"frames", LINE_INFO());
            Assert::IsTrue(md.frame_table().empty(), L"frame_table", LINE_INFO());
            Assert::AreEqual(std::uint8_t(0), md.version().first, L"version", LINE_INFO());
            Assert::AreEqual(std::uint8_t(0), md.version().second, L"version", LINE_INFO());
        }
    };
}
