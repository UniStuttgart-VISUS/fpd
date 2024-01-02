// <copyright file="test_convert_string.cpp" company="Visualisierungsinstitut der Universität Stuttgart">
// Copyright © 2024 Visualisierungsinstitut der Universität Stuttgart.
// Licensed under the MIT licence. See LICENCE file for details.
// </copyright>
// <author>Christoph Müller</author>

#include "pch.h"


namespace fpd {

    TEST_CLASS(test_convert_string) {

    public:

        TEST_METHOD(oem2wchar) {
            const auto cp = code_page::oem;
            typedef char src_type;
            typedef wchar_t dst_type;


            {
                const std::basic_string<dst_type> expected = L"input";
                const auto input = "input";
                Assert::AreEqual(expected, convert_string<dst_type>(input, -1, cp), L"standard input", LINE_INFO());
            }

            {
                const std::basic_string<dst_type> expected = L"";
                const auto input = "";
                Assert::AreEqual(expected, convert_string<dst_type>(input, -1, cp), L"empty narrow conversion", LINE_INFO());
            }

            {
                const auto input = static_cast<src_type *>(nullptr);
                Assert::ExpectException<std::invalid_argument>([&input, cp](void) {
                    convert_string<dst_type>(input, -1, cp), L"nullptr conversion", LINE_INFO();
                });
            }
        }

        TEST_METHOD(wchar2oem) {
            const auto cp = code_page::oem;
            typedef wchar_t src_type;
            typedef char dst_type;

            {
                const std::basic_string<dst_type> expected = "input";
                const auto input = L"input";
                Assert::AreEqual(expected, convert_string<dst_type>(input, -1, cp), L"standard input", LINE_INFO());
            }

            {
                const std::basic_string<dst_type> expected = "";
                const auto input = L"";
                Assert::AreEqual(expected, convert_string<dst_type>(input, -1, cp), L"empty conversion", LINE_INFO());
            }

            {
                const auto input = static_cast<src_type *>(nullptr);
                Assert::ExpectException<std::invalid_argument>([&input, cp](void) {
                    convert_string<dst_type>(input, -1, cp), L"nullptr conversion", LINE_INFO();
                });
            }
        }

        TEST_METHOD(wchar2utf8) {
            const auto cp = code_page::utf8;
            typedef wchar_t src_type;
            typedef char dst_type;

            {
                const std::basic_string<dst_type> expected = "input";
                const auto input = L"input";
                const auto actual = convert_string<dst_type>(input, -1, cp);
                Assert::AreEqual(expected, actual, L"standard input", LINE_INFO());
            }

            {
                const std::basic_string<dst_type> expected = "";
                const auto input = L"";
                const auto actual = convert_string<dst_type>(input, -1, cp);
                Assert::AreEqual(expected, actual, L"empty conversion", LINE_INFO());
            }

            {
                // This is incredibly hard to test without messing the constants ...
                // Reference from https://onlineutf8tools.com/convert-utf8-to-bytes.
                const std::vector<std::uint8_t> expected { 0xd0, 0xbf, 0xd1, 0x80, 0xd0, 0xb8, 0xd0, 0xb2, 0xd0, 0xb5, 0xd1, 0x82, 0 };
                const auto input = L"привет";
                const auto actual = convert_string<dst_type>(input, -1, cp);
                Assert::AreEqual(0, ::memcmp(expected.data(), actual.c_str(), expected.size()), L"non-ASCII conversion", LINE_INFO());
            }

            {
                const auto input = static_cast<src_type *>(nullptr);
                Assert::ExpectException<std::invalid_argument>([&input, cp](void) {
                    convert_string<dst_type>(input, -1, cp), L"nullptr conversion", LINE_INFO();
                });
            }
        }
    };
}
