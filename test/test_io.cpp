// <copyright file="test_io.cpp" company="Visualisierungsinstitut der Universität Stuttgart">
// Copyright © 2024 Visualisierungsinstitut der Universität Stuttgart.
// Licensed under the MIT licence. See LICENCE file for details.
// </copyright>
// <author>Christoph Müller</author>

#include "pch.h"


namespace fpd {

    TEST_CLASS(test_io) {

    public:

        TEST_METHOD(win32) {
            const auto expected = "Horst";
            char actual[] = "\0\0\0\0\0\0";

            {
                wchar_t path[MAX_PATH];

                {
                    auto r = ::GetTempPathW(MAX_PATH, path);
                    if ((r > MAX_PATH) || (r == 0)) {
                        Assert::Fail(L"Failed to get temporary path", LINE_INFO());
                    }
                }

                Assert::IsTrue(::GetTempFileNameW(path, L"fpd", 0, path), L"GetTempFileName", LINE_INFO());

                win32_io io(path, GENERIC_READ | GENERIC_WRITE, 0, CREATE_ALWAYS);
                Assert::IsTrue(::strlen(expected) <= io.write(0, expected, ::strlen(expected)), L"write", LINE_INFO());
                ::ZeroMemory(actual, sizeof(actual));
                Assert::IsTrue(::strlen(expected) <= io.read(actual, ::strlen(expected), 0), L"read", LINE_INFO());
                Assert::AreEqual(actual, expected, L"roundtrip", LINE_INFO());
            }

            {
                char path[MAX_PATH];

                {
                    auto r = ::GetTempPathA(MAX_PATH, path);
                    if ((r > MAX_PATH) || (r == 0)) {
                        Assert::Fail(L"Failed to get temporary path", LINE_INFO());
                    }
                }

                Assert::IsTrue(::GetTempFileNameA(path, "fpd", 0, path), L"GetTempFileName", LINE_INFO());

                win32_io io(path, GENERIC_READ | GENERIC_WRITE, 0, CREATE_ALWAYS);
                Assert::IsTrue(::strlen(expected) <= io.write(0, expected, ::strlen(expected)), L"write", LINE_INFO());
                ::ZeroMemory(actual, sizeof(actual));
                Assert::IsTrue(::strlen(expected) <= io.read(actual, ::strlen(expected), 0), L"read", LINE_INFO());
                Assert::AreEqual(actual, expected, L"roundtrip", LINE_INFO());
            }
        }
    };
}
