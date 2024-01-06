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

        TEST_METHOD(posix) {
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

                posix_io io(path, _O_RDWR | _O_CREAT | _O_TRUNC);
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

                posix_io io(path, _O_RDWR | _O_CREAT | _O_TRUNC);
                Assert::IsTrue(::strlen(expected) <= io.write(0, expected, ::strlen(expected)), L"write", LINE_INFO());
                ::ZeroMemory(actual, sizeof(actual));
                Assert::IsTrue(::strlen(expected) <= io.read(actual, ::strlen(expected), 0), L"read", LINE_INFO());
                Assert::AreEqual(actual, expected, L"roundtrip", LINE_INFO());
            }
        }


        TEST_METHOD(memory) {
            const auto expected = "Horst";
            char actual[] = "\0\0\0\0\0\0";

            {
                std::array<std::uint8_t, 32> buffer;
                memory_io io(buffer.data(), buffer.size());
                Assert::IsTrue(::strlen(expected) <= io.write(0, expected, ::strlen(expected)), L"write", LINE_INFO());
                ::ZeroMemory(actual, sizeof(actual));
                Assert::IsTrue(::strlen(expected) <= io.read(actual, ::strlen(expected), 0), L"read", LINE_INFO());
                Assert::AreEqual(actual, expected, L"roundtrip", LINE_INFO());
            }

            {
                Assert::ExpectException<std::system_error>([&](void) {
                    memory_io io(nullptr, 0);
                    io.write(0, expected, ::strlen(expected));
                }, L"Write to empty buffer", LINE_INFO());
            }

            {
                Assert::ExpectException<std::system_error>([&](void) {
                    std::array<std::uint8_t, 32> buffer;
                    memory_io io(buffer.data(), buffer.size());
                    io.write(buffer.size(), expected, ::strlen(expected));
                }, L"Write out of range", LINE_INFO());
            }

            {
                Assert::ExpectException<std::system_error>([&](void) {
                    memory_io io(nullptr, 0);
                    io.read(actual, ::strlen(expected), 0);
                }, L"Read from empty buffer", LINE_INFO());
            }

            {
                Assert::ExpectException<std::system_error>([&](void) {
                    std::array<std::uint8_t, 32> buffer;
                    memory_io io(buffer.data(), buffer.size());
                    io.read(actual, ::strlen(expected), buffer.size());
                }, L"Read out of range", LINE_INFO());
            }

            {
                std::array<std::uint8_t, 32> buffer;
                memory_io io(buffer.data(), buffer.size());
                Assert::AreEqual(std::size_t(1), io.write(31, expected, ::strlen(expected)), L"partial write", LINE_INFO());
                ::ZeroMemory(actual, sizeof(actual));
                Assert::AreEqual(std::size_t(1), io.read(actual, ::strlen(expected), 31), L"partical read", LINE_INFO());
                Assert::AreEqual(actual[0], expected[0], L"roundtrip", LINE_INFO());
            }
        }
    };
}
