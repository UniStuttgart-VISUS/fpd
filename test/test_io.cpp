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
            typedef detail::io_traits<HANDLE> win32_io;
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
                Assert::IsTrue(::strlen(expected) <= win32_io::write(io, 0, expected, ::strlen(expected)), L"write", LINE_INFO());
                ::ZeroMemory(actual, sizeof(actual));
                Assert::IsTrue(::strlen(expected) <= win32_io::read(actual, ::strlen(expected), io, 0), L"read", LINE_INFO());
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
                Assert::IsTrue(::strlen(expected) <= win32_io::write(io, 0, expected, ::strlen(expected)), L"write", LINE_INFO());
                ::ZeroMemory(actual, sizeof(actual));
                Assert::IsTrue(::strlen(expected) <= win32_io::read(actual, ::strlen(expected), io, 0), L"read", LINE_INFO());
                Assert::AreEqual(actual, expected, L"roundtrip", LINE_INFO());
            }
        }

        TEST_METHOD(posix) {
            typedef detail::io_traits<int> posix_io;
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
                Assert::IsTrue(::strlen(expected) <= posix_io::write(io, 0, expected, ::strlen(expected)), L"write", LINE_INFO());
                ::ZeroMemory(actual, sizeof(actual));
                Assert::IsTrue(::strlen(expected) <= posix_io::read(actual, ::strlen(expected), io, 0), L"read", LINE_INFO());
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
                Assert::IsTrue(::strlen(expected) <= posix_io::write(io, 0, expected, ::strlen(expected)), L"write", LINE_INFO());
                ::ZeroMemory(actual, sizeof(actual));
                Assert::IsTrue(::strlen(expected) <= posix_io::read(actual, ::strlen(expected), io, 0), L"read", LINE_INFO());
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


#if defined(FPD_WITH_DSTORAGE)
        TEST_METHOD(dstorage) {
            typedef detail::io_traits<HANDLE> win32_io;
            typedef detail::io_traits<IDStorageFile> dstorage_io;
            const auto expected = "Horst";
            char actual[] = "\0\0\0\0\0\0";

            // Create a temporary file name for the test.
            wchar_t path[MAX_PATH];

            {
                auto r = ::GetTempPathW(MAX_PATH, path);
                if ((r > MAX_PATH) || (r == 0)) {
                    Assert::Fail(L"Failed to get temporary path", LINE_INFO());
                }
            }

            Assert::IsTrue(::GetTempFileNameW(path, L"fpd", 0, path), L"GetTempFileName", LINE_INFO());

            // Fill the temporary file.
            {
                win32_io io(path, GENERIC_READ | GENERIC_WRITE, 0, CREATE_ALWAYS);
                Assert::IsTrue(::strlen(expected) <= win32_io::write(io, 0, expected, ::strlen(expected)), L"write", LINE_INFO());
            }

            // Test dstorage_io.
            {
                auto factory = dstorage_io::get_factory();
                Assert::IsNotNull(factory, L"DirectStorage factory", LINE_INFO());

                ID3D12Device *device;
                Assert::IsTrue(SUCCEEDED(::D3D12CreateDevice(nullptr, D3D_FEATURE_LEVEL_12_1, IID_PPV_ARGS(&device))), L"D3D device", LINE_INFO());

                ID3D12Fence *fence;
                Assert::IsTrue(SUCCEEDED(device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&fence))), L"Fence", LINE_INFO());

                IDStorageFile *file;
                Assert::IsTrue(SUCCEEDED(factory->OpenFile(path, IID_PPV_ARGS(&file))), L"File opened", LINE_INFO());

                IDStorageQueue *queue;
                {
                    DSTORAGE_QUEUE_DESC desc{ };
                    desc.Capacity = DSTORAGE_MAX_QUEUE_CAPACITY;
                    desc.Priority = DSTORAGE_PRIORITY_NORMAL;
                    desc.SourceType = DSTORAGE_REQUEST_SOURCE_FILE;
                    desc.Device = device;
                    Assert::IsTrue(SUCCEEDED(factory->CreateQueue(&desc, IID_PPV_ARGS(&queue))), L"Queue created", LINE_INFO());
                }

                dstorage_io io(file, queue, fence);
                device->Release();
                fence->Release();
                file->Release();
                queue->Release();
                factory->Release();

                ::ZeroMemory(actual, sizeof(actual));
                Assert::IsTrue(::strlen(expected) <= dstorage_io::read(actual, ::strlen(expected), io, 0), L"read", LINE_INFO());
                Assert::AreEqual(actual, expected, L"roundtrip", LINE_INFO());
            }
        }
#endif /* defined(FPD_WITH_DSTORAGE) */

    };
}
