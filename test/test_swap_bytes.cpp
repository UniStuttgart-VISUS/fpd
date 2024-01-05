// <copyright file="test_swap_bytes.cpp" company="Visualisierungsinstitut der Universität Stuttgart">
// Copyright © 2023 Visualisierungsinstitut der Universität Stuttgart.
// Licensed under the MIT licence. See LICENCE file for details.
// </copyright>
// <author>Christoph Müller</author>

#include "pch.h"


namespace fpd {

    TEST_CLASS(test_swap_bytes) {

    public:

        TEST_METHOD(int8) {
            typedef std::int8_t type;
            const auto expected = type(42);
            Assert::AreEqual(expected, swap_bytes(expected), L"nothing happens", LINE_INFO());
        }

        TEST_METHOD(uint8) {
            typedef std::uint8_t type;
            const auto expected = type(42);
            Assert::AreEqual(expected, swap_bytes(expected), L"nothing happens", LINE_INFO());
        }

        TEST_METHOD(int16) {
            typedef std::int16_t type;
            union {
                type value;
                std::uint8_t bytes[sizeof(type)];
            } original, swapped;

            original.bytes[0] = 42;
            original.bytes[1] = 43;
            std::reverse_copy(original.bytes, original.bytes + sizeof(type), swapped.bytes);

            Assert::AreEqual(int(swapped.value), int(swap_bytes(original.value)), L"bytes swapped", LINE_INFO());
        }

        TEST_METHOD(uint16) {
            typedef std::uint16_t type;
            union {
                type value;
                std::uint8_t bytes[sizeof(type)];
            } original, swapped;

            original.bytes[0] = 42;
            original.bytes[1] = 43;
            std::reverse_copy(original.bytes, original.bytes + sizeof(type), swapped.bytes);

            Assert::AreEqual(int(swapped.value), int(swap_bytes(original.value)), L"bytes swapped", LINE_INFO());
        }

        TEST_METHOD(int32) {
            typedef std::int32_t type;
            union {
                type value;
                std::uint8_t bytes[sizeof(type)];
            } original, swapped;

            original.bytes[0] = 42;
            original.bytes[1] = 43;
            original.bytes[2] = 44;
            original.bytes[3] = 45;
            std::reverse_copy(original.bytes, original.bytes + sizeof(type), swapped.bytes);

            Assert::AreEqual(swapped.value, swap_bytes(original.value), L"bytes swapped", LINE_INFO());
        }

        TEST_METHOD(uint32) {
            typedef std::uint32_t type;
            union {
                type value;
                std::uint8_t bytes[sizeof(type)];
            } original, swapped;

            original.bytes[0] = 42;
            original.bytes[1] = 43;
            original.bytes[2] = 44;
            original.bytes[3] = 45;
            std::reverse_copy(original.bytes, original.bytes + sizeof(type), swapped.bytes);

            Assert::AreEqual(swapped.value, swap_bytes(original.value), L"bytes swapped", LINE_INFO());
        }

        TEST_METHOD(int64) {
            typedef std::int64_t type;
            union {
                type value;
                std::uint8_t bytes[sizeof(type)];
            } original, swapped;

            original.bytes[0] = 42;
            original.bytes[1] = 43;
            original.bytes[2] = 44;
            original.bytes[3] = 45;
            original.bytes[4] = 46;
            original.bytes[5] = 47;
            original.bytes[6] = 48;
            original.bytes[7] = 49;
            std::reverse_copy(original.bytes, original.bytes + sizeof(type), swapped.bytes);

            Assert::AreEqual(swapped.value, swap_bytes(original.value), L"bytes swapped", LINE_INFO());
        }

        TEST_METHOD(uint64) {
            typedef std::uint64_t type;
            union {
                type value;
                std::uint8_t bytes[sizeof(type)];
            } original, swapped;

            original.bytes[0] = 42;
            original.bytes[1] = 43;
            original.bytes[2] = 44;
            original.bytes[3] = 45;
            original.bytes[4] = 46;
            original.bytes[5] = 47;
            original.bytes[6] = 48;
            original.bytes[7] = 49;
            std::reverse_copy(original.bytes, original.bytes + sizeof(type), swapped.bytes);

            Assert::AreEqual(swapped.value, swap_bytes(original.value), L"bytes swapped", LINE_INFO());
        }

        TEST_METHOD(float32) {
            typedef float type;
            union {
                type value;
                std::uint8_t bytes[sizeof(type)];
            } original, swapped;

            original.bytes[0] = 42;
            original.bytes[1] = 43;
            original.bytes[2] = 44;
            original.bytes[3] = 45;
            std::reverse_copy(original.bytes, original.bytes + sizeof(type), swapped.bytes);

            Assert::AreEqual(swapped.value, swap_bytes(original.value), L"bytes swapped", LINE_INFO());
        }

        TEST_METHOD(float64) {
            typedef double type;
            union {
                type value;
                std::uint8_t bytes[sizeof(type)];
            } original, swapped;

            original.bytes[0] = 42;
            original.bytes[1] = 43;
            original.bytes[2] = 44;
            original.bytes[3] = 45;
            original.bytes[4] = 46;
            original.bytes[5] = 47;
            original.bytes[6] = 48;
            original.bytes[7] = 49;
            std::reverse_copy(original.bytes, original.bytes + sizeof(type), swapped.bytes);

            Assert::AreEqual(swapped.value, swap_bytes(original.value), L"bytes swapped", LINE_INFO());
        }

        TEST_METHOD(int8stream) {
            typedef std::int8_t type;
            std::array<type, 32> data;
            std::iota(data.begin(), data.end(), type(0));

            swap_bytes(data.data(), data.size());

            for (std::size_t i = 0; i < data.size(); ++i) {
                Assert::AreEqual(type(i), data[i], L"nothing happens", LINE_INFO());
            }
        }

        TEST_METHOD(uint8stream) {
            typedef std::uint8_t type;
            std::array<type, 32> data;
            std::iota(data.begin(), data.end(), type(0));

            swap_bytes(data.data(), data.size());

            for (std::size_t i = 0; i < data.size(); ++i) {
                Assert::AreEqual(type(i), data[i], L"nothing happens", LINE_INFO());
            }
        }

        TEST_METHOD(int16stream) {
            typedef std::int16_t type;

            std::array<type, 32> data;
            std::iota(data.begin(), data.end(), type(255));

            swap_bytes(data.data(), data.size());

            for (std::size_t i = 0; i < data.size(); ++i) {
                union {
                    type value;
                    std::uint8_t bytes[sizeof(type)];
                } swapped { type(255 + i) };
                std::reverse(swapped.bytes, swapped.bytes + sizeof(type));

                Assert::AreEqual(int(swapped.value), int(data[i]), L"bytes swapped", LINE_INFO());
            }
        }

        TEST_METHOD(uint16stream) {
            typedef std::uint16_t type;

            std::array<type, 32> data;
            std::iota(data.begin(), data.end(), type(255));

            swap_bytes(data.data(), data.size());

            for (std::size_t i = 0; i < data.size(); ++i) {
                union {
                    type value;
                    std::uint8_t bytes[sizeof(type)];
                } swapped { type(255 + i) };
                std::reverse(swapped.bytes, swapped.bytes + sizeof(type));

                Assert::AreEqual(int(swapped.value), int(data[i]), L"bytes swapped", LINE_INFO());
            }
        }

        TEST_METHOD(int32stream) {
            typedef std::int32_t type;

            std::array<type, 32> data;
            std::iota(data.begin(), data.end(), type(255));

            swap_bytes(data.data(), data.size());

            for (std::size_t i = 0; i < data.size(); ++i) {
                union {
                    type value;
                    std::uint8_t bytes[sizeof(type)];
                } swapped { type(255 + i) };
                std::reverse(swapped.bytes, swapped.bytes + sizeof(type));

                Assert::AreEqual(swapped.value, data[i], L"bytes swapped", LINE_INFO());
            }
        }

        TEST_METHOD(uint32stream) {
            typedef std::uint32_t type;

            std::array<type, 32> data;
            std::iota(data.begin(), data.end(), type(255));

            swap_bytes(data.data(), data.size());

            for (std::size_t i = 0; i < data.size(); ++i) {
                union {
                    type value;
                    std::uint8_t bytes[sizeof(type)];
                } swapped { type(255 + i) };
                std::reverse(swapped.bytes, swapped.bytes + sizeof(type));

                Assert::AreEqual(swapped.value, data[i], L"bytes swapped", LINE_INFO());
            }
        }

        TEST_METHOD(int64stream) {
            typedef std::int64_t type;

            std::array<type, 32> data;
            std::iota(data.begin(), data.end(), type(255));

            swap_bytes(data.data(), data.size());

            for (std::size_t i = 0; i < data.size(); ++i) {
                union {
                    type value;
                    std::uint8_t bytes[sizeof(type)];
                } swapped { type(255 + i) };
                std::reverse(swapped.bytes, swapped.bytes + sizeof(type));

                Assert::AreEqual(swapped.value, data[i], L"bytes swapped", LINE_INFO());
            }
        }

        TEST_METHOD(uint64stream) {
            typedef std::uint64_t type;

            std::array<type, 32> data;
            std::iota(data.begin(), data.end(), type(255));

            swap_bytes(data.data(), data.size());

            for (std::size_t i = 0; i < data.size(); ++i) {
                union {
                    type value;
                    std::uint8_t bytes[sizeof(type)];
                } swapped { type(255 + i) };
                std::reverse(swapped.bytes, swapped.bytes + sizeof(type));

                Assert::AreEqual(swapped.value, data[i], L"bytes swapped", LINE_INFO());
            }
        }

        TEST_METHOD(float32stream) {
            typedef float type;

            std::array<type, 32> data;
            std::iota(data.begin(), data.end(), type(255));

            swap_bytes(data.data(), data.size());

            for (std::size_t i = 0; i < data.size(); ++i) {
                union {
                    type value;
                    std::uint8_t bytes[sizeof(type)];
                } swapped { type(255 + i) };
                std::reverse(swapped.bytes, swapped.bytes + sizeof(type));

                Assert::AreEqual(swapped.value, data[i], L"bytes swapped", LINE_INFO());
            }
        }

        TEST_METHOD(float64stream) {
            typedef double type;

            std::array<type, 32> data;
            std::iota(data.begin(), data.end(), type(255));

            swap_bytes(data.data(), data.size());

            for (std::size_t i = 0; i < data.size(); ++i) {
                union {
                    type value;
                    std::uint8_t bytes[sizeof(type)];
                } swapped { type(255 + i) };
                std::reverse(swapped.bytes, swapped.bytes + sizeof(type));

                Assert::AreEqual(swapped.value, data[i], L"bytes swapped", LINE_INFO());
            }
        }

        TEST_METHOD(stream2) {
            typedef std::uint16_t type;
            std::array<type, 32> data;
            std::iota(data.begin(), data.end(), std::uint16_t(UCHAR_MAX));

            swap_bytes(data.data(), data.size(), sizeof(type));

            for (std::size_t i = 1; i < data.size(); ++i) {
                union {
                    type value;
                    std::uint8_t bytes[sizeof(type)];
                } swapped { type(UCHAR_MAX + i) };
                std::reverse(swapped.bytes, swapped.bytes + sizeof(type));

                Assert::AreEqual(int(swapped.value), int(data[i]), L"bytes swapped", LINE_INFO());
            }
        }

        TEST_METHOD(stream4) {
            typedef std::uint32_t type;
            std::array<type, 32> data;
            std::iota(data.begin(), data.end(), std::uint32_t(USHRT_MAX));

            swap_bytes(data.data(), data.size(), sizeof(type));

            for (std::size_t i = 1; i < data.size(); ++i) {
                union {
                    type value;
                    std::uint8_t bytes[sizeof(type)];
                } swapped { type(USHRT_MAX) + type(i) };
                std::reverse(swapped.bytes, swapped.bytes + sizeof(type));

                Assert::AreEqual(swapped.value, data[i], L"bytes swapped", LINE_INFO());
            }
        }

        TEST_METHOD(stream8) {
            typedef std::uint64_t type;
            std::array<type, 32> data;
            std::iota(data.begin(), data.end(), std::uint64_t(UINT_MAX));

            swap_bytes(data.data(), data.size(), sizeof(type));

            for (std::size_t i = 1; i < data.size(); ++i) {
                union {
                    type value;
                    std::uint8_t bytes[sizeof(type)];
                } swapped { type(UINT_MAX) + i };
                std::reverse(swapped.bytes, swapped.bytes + sizeof(type));

                Assert::AreEqual(swapped.value, data[i], L"bytes swapped", LINE_INFO());
            }
        }
    };
}
