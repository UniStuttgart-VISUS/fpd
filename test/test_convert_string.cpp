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
            const auto cp = encodings::oem;
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
            const auto cp = encodings::oem;
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
            const auto cp = encodings::utf8;
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
                Assert::AreEqual(std::size_t(13), expected.size(), L"expected size", LINE_INFO());
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
            const auto cp = encodings::oem;
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
            const auto cp = encodings::oem;
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
            const auto cp = encodings::utf8;
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
            Assert::AreEqual(std::size_t(0), convert_string(static_cast<utf16_type *>(nullptr), 0, static_cast<char *>(nullptr), 0, encodings::utf8), L"UTF-8 to UTF-16 nullptr, 0", LINE_INFO());
            Assert::AreEqual(std::size_t(0), convert_string(static_cast<utf16_type *>(nullptr), 0, static_cast<char *>(nullptr), -1, encodings::utf8), L"UTF-8 to UTF-16 nullptr, -1", LINE_INFO());
            Assert::AreEqual(std::size_t(0), convert_string(static_cast<utf16_type *>(nullptr), 0, static_cast<char *>(nullptr), 42, encodings::utf8), L"UTF-8 to UTF-16 nullptr, 42", LINE_INFO());

            Assert::AreEqual(std::size_t(0), convert_string(static_cast<char *>(nullptr), 0, static_cast<utf16_type *>(nullptr), 0, encodings::utf8), L"UTF-16 to UTF-8 nullptr, 0", LINE_INFO());
            Assert::AreEqual(std::size_t(0), convert_string(static_cast<char *>(nullptr), 0, static_cast<utf16_type *>(nullptr), -1, encodings::utf8), L"UTF-16 to UTF-8 nullptr, -1", LINE_INFO());
            Assert::AreEqual(std::size_t(0), convert_string(static_cast<char *>(nullptr), 0, static_cast<utf16_type *>(nullptr), 42, encodings::utf8), L"UTF-16 to UTF-8, nullptr, 42", LINE_INFO());

            {
                std::vector<utf16_type> buffer(convert_string(static_cast<utf16_type *>(nullptr), 0, "input", -1, encodings::utf8));
                Assert::AreEqual(std::size_t(6), buffer.size(), L"UTF-8 to UTF-16 \"input\", -1", LINE_INFO());
                Assert::AreEqual(std::size_t(6), convert_string(buffer.data(), buffer.size(), "input", -1, encodings::utf8), L"Converted input", LINE_INFO());
            }

            {
                std::vector<utf16_type> buffer(convert_string(static_cast<utf16_type *>(nullptr), 0, "input", 0, encodings::utf8));
                Assert::AreEqual(std::size_t(0), buffer.size(), L"UTF-8 to UTF-16 \"input\", 0", LINE_INFO());
                Assert::AreEqual(std::size_t(0), convert_string(buffer.data(), buffer.size(), "input", 0, encodings::utf8), L"Converted input", LINE_INFO());
            }

            {
                std::vector<utf16_type> buffer(convert_string(static_cast<utf16_type *>(nullptr), 0, "input", 5, encodings::utf8));
                Assert::AreEqual(std::size_t(5), buffer.size(), L"UTF-8 to UTF-16 \"input\", 5", LINE_INFO());
                Assert::AreEqual(std::size_t(5), convert_string(buffer.data(), buffer.size(), "input", 5, encodings::utf8), L"Converted input", LINE_INFO());
            }

            {
                std::vector<char> buffer(convert_string(static_cast<char *>(nullptr), 0, L"input", -1, encodings::utf8));
                Assert::AreEqual(std::size_t(6), buffer.size(), L"UTF-16 to UTF-8 \"input\", -1", LINE_INFO());
                Assert::AreEqual(std::size_t(6), convert_string(buffer.data(), buffer.size(), L"input", -1, encodings::utf8), L"Converted input", LINE_INFO());
            }

            {
                std::vector<char> buffer(convert_string(static_cast<char *>(nullptr), 0, L"input", 5, encodings::utf8));
                Assert::AreEqual(std::size_t(5), buffer.size(), L"UTF-16 to UTF-8 \"input\", 5", LINE_INFO());
                Assert::AreEqual(std::size_t(5), convert_string(buffer.data(), buffer.size(), L"input", 5, encodings::utf8), L"Converted input", LINE_INFO());
            }

            {
                std::vector<char> buffer(convert_string(static_cast<char *>(nullptr), 0, encodings::ascii, "input", -1, encodings::utf8));
                Assert::AreEqual(std::size_t(6), buffer.size(), L"UTF-8 to ASCII \"input\", -1", LINE_INFO());
                Assert::AreEqual(std::size_t(6), convert_string(buffer.data(), buffer.size(), encodings::ascii, "input", -1, encodings::utf8), L"Converted input", LINE_INFO());
            }

            {
                std::vector<char> buffer(convert_string(static_cast<char *>(nullptr), 0, encodings::ascii, "input", 5, encodings::utf8));
                Assert::AreEqual(std::size_t(5), buffer.size(), L"UTF-8 to ASCII \"input\", 5", LINE_INFO());
                Assert::AreEqual(std::size_t(5), convert_string(buffer.data(), buffer.size(), encodings::ascii, "input", 5, encodings::utf8), L"Converted input", LINE_INFO());
            }

            {
                std::vector<char> buffer(convert_string(static_cast<char *>(nullptr), 0, encodings::utf8, "Müller", -1, encodings::oem));
                Assert::AreEqual(std::size_t(8), buffer.size(), L"OEM to UTF-8 \"Müller\", -1", LINE_INFO());
                Assert::AreEqual(std::size_t(8), convert_string(buffer.data(), buffer.size(), encodings::utf8, "Müller", -1, encodings::oem), L"Converted input", LINE_INFO());
            }

            {
                std::vector<char> buffer(convert_string(static_cast<char *>(nullptr), 0, encodings::utf8, "Müller", 6, encodings::oem));
                Assert::AreEqual(std::size_t(7), buffer.size(), L"OEM to UTF-8 \"Müller\", 6", LINE_INFO());
                Assert::AreEqual(std::size_t(7), convert_string(buffer.data(), buffer.size(), encodings::utf8, "Müller", 6, encodings::oem), L"Converted input", LINE_INFO());
            }

            {
                std::vector<char> buffer(convert_string(static_cast<char *>(nullptr), 0, L"привет", -1, encodings::utf8));
                Assert::AreEqual(std::size_t(13), buffer.size(), L"UTF-16 to UTF-8 \"привет\", -1", LINE_INFO());
                Assert::AreEqual(std::size_t(13), convert_string(buffer.data(), buffer.size(), L"привет", -1, encodings::utf8), L"Converted привет", LINE_INFO());
            }

            {
                std::vector<char> buffer(convert_string(static_cast<char *>(nullptr), 0, L"привет", 6, encodings::utf8));
                Assert::AreEqual(std::size_t(12), buffer.size(), L"UTF-16 to UTF-8 \"привет\", 6", LINE_INFO());
                Assert::AreEqual(std::size_t(12), convert_string(buffer.data(), buffer.size(), L"привет", 6, encodings::utf8), L"Converted привет", LINE_INFO());
            }

            {
                std::vector<utf16_type> buffer(convert_string(static_cast<utf16_type *>(nullptr), 0, encodings::utf16, L"привет", -1, encodings::utf16));
                Assert::AreEqual(std::size_t(7), buffer.size(), L"UTF-16 to UTF-16 \"привет\", -1", LINE_INFO());
                Assert::AreEqual(std::size_t(7), convert_string(buffer.data(), buffer.size(), encodings::utf16, L"привет", -1, encodings::utf16), L"Converted привет", LINE_INFO());
            }

            {
                std::vector<utf16_type> buffer(convert_string(static_cast<utf16_type *>(nullptr), 0, encodings::utf16, L"привет", 6, encodings::utf16));
                Assert::AreEqual(std::size_t(6), buffer.size(), L"UTF-16 to UTF-16 \"привет\", 6", LINE_INFO());
                Assert::AreEqual(std::size_t(6), convert_string(buffer.data(), buffer.size(), encodings::utf16, L"привет", 6, encodings::utf16), L"Converted привет", LINE_INFO());
            }

            {
                std::vector<utf16_type> buffer(convert_string(static_cast<utf16_type *>(nullptr), 0, encodings::utf16, u8"привет", -1, encodings::utf8));
                Assert::AreEqual(std::size_t(7), buffer.size(), L"UTF-8 to UTF-16 \"привет\", -1", LINE_INFO());
                Assert::AreEqual(std::size_t(7), convert_string(buffer.data(), buffer.size(), encodings::utf16, u8"привет", -1, encodings::utf8), L"Converted привет", LINE_INFO());
            }

            {
                std::vector<utf16_type> buffer(convert_string(static_cast<utf16_type *>(nullptr), 0, encodings::utf16, u8"привет", int(::strlen(u8"привет")), encodings::utf8));
                Assert::AreEqual(std::size_t(6), buffer.size(), L"UTF-8 to UTF-16 \"привет\", strlen(\"привет\")", LINE_INFO());
                Assert::AreEqual(std::size_t(6), convert_string(buffer.data(), buffer.size(), encodings::utf16, u8"привет", int(::strlen(u8"привет")), encodings::utf8), L"Converted привет", LINE_INFO());
            }
        }
    };
}
