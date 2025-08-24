// <copyright file="test_convert_string.cpp" company="Visualisierungsinstitut der Universität Stuttgart">
// Copyright © 2024 - 2025 Visualisierungsinstitut der Universität Stuttgart.
// Licensed under the MIT licence. See LICENCE file for details.
// </copyright>
// <author>Christoph Müller</author>

#include "pch.h"


namespace fpd {

    TEST_CLASS(test_convert_string) {

    public:

        TEST_METHOD(oem2wchar) {
            const auto cp = code_pages::oem;
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
                Assert::IsTrue(convert_string<dst_type>(input, -1, cp).empty(), L"nullptr conversion", LINE_INFO());
            }
        }

        TEST_METHOD(wchar2oem) {
            const auto cp = code_pages::oem;
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
                Assert::IsTrue(convert_string<dst_type>(input, -1, cp).empty(), L"nullptr conversion", LINE_INFO());
            }
        }

        TEST_METHOD(wchar2utf8) {
            const auto cp = code_pages::utf8;
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
                Assert::IsTrue(convert_string<dst_type>(input, -1, cp).empty(), L"nullptr conversion", LINE_INFO());
            }
        }

        TEST_METHOD(char162wchar) {
            const auto cp = code_pages::oem;
            typedef char16_t src_type;
            typedef wchar_t dst_type;

            {
                const std::basic_string<dst_type> expected = L"input";
                const auto input = u"input";
                Assert::AreEqual(expected, convert_string<dst_type>(input, -1, cp), L"standard input", LINE_INFO());
            }

            {
                const std::basic_string<dst_type> expected = L"";
                const auto input = u"";
                Assert::AreEqual(expected, convert_string<dst_type>(input, -1, cp), L"empty conversion", LINE_INFO());
            }

            {
                const auto input = static_cast<src_type *>(nullptr);
                Assert::IsTrue(convert_string<dst_type>(input, -1, cp).empty(), L"nullptr conversion", LINE_INFO());
            }
        }

        TEST_METHOD(wchar2char16) {
            const auto cp = code_pages::oem;
            typedef wchar_t src_type;
            typedef char16_t dst_type;

            {
                const auto input = L"input";
                auto actual = convert_string<dst_type>(input, -1, cp);
                Assert::AreEqual(input, reinterpret_cast<const wchar_t *>(actual.c_str()), L"standard input", LINE_INFO());
            }

            {
                const auto input = L"";
                auto actual = convert_string<dst_type>(input, -1, cp);
                Assert::AreEqual(input, reinterpret_cast<const wchar_t *>(actual.c_str()), L"empty conversion", LINE_INFO());
            }

            {
                const auto input = static_cast<src_type *>(nullptr);
                Assert::IsTrue(convert_string<dst_type>(input, -1, cp).empty(), L"nullptr conversion", LINE_INFO());
            }
        }


        TEST_METHOD(utf32to8) {
            const auto cp = code_pages::utf8;
            typedef wchar_t src_type;
            typedef char dst_type;

            {
                const std::basic_string<dst_type> expected = u8"input";
                const auto input = L"input";
                const auto actual = convert_string<dst_type>(input, -1, cp);
                Assert::AreEqual(expected, actual, L"standard input", LINE_INFO());
            }

            {
                const std::basic_string<dst_type> expected = u8"";
                const auto input = L"";
                const auto actual = convert_string<dst_type>(input, -1, cp);
                Assert::AreEqual(expected, actual, L"empty conversion", LINE_INFO());
            }

            {
                const std::basic_string<dst_type> expected = u8"привет";
                const auto input = L"привет";
                const auto actual = convert_string<dst_type>(input, -1, cp);
                Assert::AreEqual(expected, actual, L"non-ASCII conversion", LINE_INFO());
            }

            {
                const auto input = static_cast<src_type *>(nullptr);
                Assert::IsTrue(convert_string<dst_type>(input, -1, cp).empty(), L"nullptr conversion", LINE_INFO());
            }
        }

        TEST_METHOD(measure) {
            {
                Assert::AreEqual(std::size_t(0), measure_string<wchar_t>(nullptr, code_pages::utf8), L"nullptr", LINE_INFO());
                Assert::AreEqual(std::size_t(0), measure_string<wchar_t>(nullptr, -1, code_pages::utf8), L"nullptr", LINE_INFO());
                Assert::AreEqual(std::size_t(0), measure_string<wchar_t>(nullptr, 42, code_pages::utf8), L"nullptr", LINE_INFO());
            }

            {
                typedef char dst_type;
                const auto input = L"input";
                Assert::AreEqual(std::size_t(6), measure_string(input, code_pages::utf8), L"wchar_t as utf8", LINE_INFO());
            }

            {
                typedef char dst_type;
                const auto input = L"input";
                Assert::AreEqual(std::size_t(3), measure_string(input, 3, code_pages::utf8), L"limited wchar_t as utf8", LINE_INFO());
            }

            {
                typedef char dst_type;
                const auto input = L"привет";
                Assert::AreEqual(std::size_t(13), measure_string(code_pages::utf8, input, code_pages::utf16le), L"привет as utf8", LINE_INFO());
            }
        }

        TEST_METHOD(measure_bugs) {

            //{
            //    typedef char dst_type;
            //    const auto input = "input";
            //    Assert::AreEqual(std::size_t(6), measure_string(code_pages::ansi, input, -1, code_pages::utf8), L"utf8 as ANSI", LINE_INFO());
            //}

            {
                typedef char dst_type;
                const auto input = L"input";
                Assert::AreEqual(std::size_t(6), measure_string(code_pages::ansi, input, -1, code_pages::utf16le), L"utf16 as ANSI", LINE_INFO());
            }
        }
    };
}
