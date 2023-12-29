// <copyright file="test_endian.cpp" company="Visualisierungsinstitut der Universität Stuttgart">
// Copyright © 2023 Visualisierungsinstitut der Universität Stuttgart.
// Licensed under the MIT licence. See LICENCE file for details.
// </copyright>
// <author>Christoph Müller</author>

#include "pch.h"


namespace fpd {

    TEST_CLASS(test_endian) {

    public:

        TEST_METHOD(definition) {
            Assert::AreNotEqual(int(endian::undefined), int(endian::big), L"big != undefined", LINE_INFO());
            Assert::AreNotEqual(int(endian::undefined), int(endian::little), L"little != undefined", LINE_INFO());
            Assert::AreNotEqual(int(endian::undefined), int(endian::system), L"system != undefined", LINE_INFO());
        }

        TEST_METHOD(swap8) {
            const std::uint8_t expected = 42;
            Assert::AreEqual(expected, swap_endian(expected, endian::little), L"little uint8_t", LINE_INFO());
            Assert::AreEqual(expected, swap_endian(expected, endian::big), L"big uint8_t", LINE_INFO());

            std::array<std::uint8_t, 42> input;
            std::fill(input.begin(), input.end(), expected);

            decltype(input) output;
            std::fill(input.begin(), input.end(), static_cast<std::uint8_t>(0));

            swap_endian(output.data(), input.data(), input.size(), endian::little);
            for (std::size_t i = 0; i < output.size(); ++i) {
                Assert::AreEqual(input[i], output[i], L"little uint8_t copy", LINE_INFO());
            }

            swap_endian(output.data(), input.data(), input.size(), endian::big);
            for (std::size_t i = 0; i < output.size(); ++i) {
                Assert::AreEqual(input[i], output[i], L"big uint8_t copy", LINE_INFO());
            }

            swap_endian(input.data(), input.size(), endian::little);
            for (std::size_t i = 0; i < output.size(); ++i) {
                Assert::AreEqual(output[i], input[i], L"little uint8_t in-place", LINE_INFO());
            }

            swap_endian(input.data(), input.size(), endian::big);
            for (std::size_t i = 0; i < output.size(); ++i) {
                Assert::AreEqual(output[i], input[i], L"big uint8_t in-place", LINE_INFO());
            }
        }

        TEST_METHOD(swap16) {
            union {
                std::uint16_t value;
                std::uint8_t bytes[2];
            } swapped, system;

            system.bytes[0] = 42;
            system.bytes[1] = 43;

            swapped.value = system.value;
            std::reverse(swapped.bytes, swapped.bytes + std::size(swapped.bytes));

            if (endian::system == endian::little) {
                Assert::AreEqual(int(system.value), int(swap_endian(system.value, endian::little)), L"little uint16_t", LINE_INFO());
                Assert::AreEqual(int(swapped.value), int(swap_endian(system.value, endian::big)), L"big uint16_t", LINE_INFO());

                std::array<decltype(swapped.value), 42> input;
                std::fill(input.begin(), input.end(), system.value);
                std::array<decltype(swapped.value), 42> output;

                std::fill(output.begin(), output.end(), static_cast<decltype(swapped.value)>(0));
                swap_endian(output.data(), input.data(), input.size(), endian::little);
                for (std::size_t i = 0; i < output.size(); ++i) {
                    Assert::AreEqual(int(system.value), int(output[i]), L"little uint16_t copy", LINE_INFO());
                }

                std::fill(output.begin(), output.end(), static_cast<decltype(swapped.value)>(0));
                swap_endian(output.data(), input.data(), input.size(), endian::big);
                for (std::size_t i = 0; i < output.size(); ++i) {
                    Assert::AreEqual(int(swapped.value), int(output[i]), L"big uint16_t copy", LINE_INFO());
                }

            } else {
                Assert::AreEqual(int(swapped.value), int(swap_endian(system.value, endian::little)), L"little uint16_t", LINE_INFO());
                Assert::AreEqual(int(system.value), int(swap_endian(system.value, endian::big)), L"big uint16_t", LINE_INFO());

                std::array<decltype(swapped.value), 42> input;
                std::fill(input.begin(), input.end(), system.value);
                std::array<decltype(swapped.value), 42> output;

                std::fill(output.begin(), output.end(), static_cast<decltype(swapped.value)>(0));
                swap_endian(output.data(), input.data(), input.size(), endian::little);
                for (std::size_t i = 0; i < output.size(); ++i) {
                    Assert::AreEqual(int(swapped.value), int(output[i]), L"little uint16_t copy", LINE_INFO());
                }

                std::fill(output.begin(), output.end(), static_cast<decltype(swapped.value)>(0));
                swap_endian(output.data(), input.data(), input.size(), endian::big);
                for (std::size_t i = 0; i < output.size(); ++i) {
                    Assert::AreEqual(int(system.value), int(output[i]), L"big uint16_t copy", LINE_INFO());
                }
            }
        }

        TEST_METHOD(swap32) {
            union {
                std::uint32_t value;
                std::uint8_t bytes[4];
            }  swapped, system;

            system.bytes[0] = 42;
            system.bytes[1] = 43;
            system.bytes[2] = 44;
            system.bytes[3] = 45;

            swapped.value = system.value;
            std::reverse(swapped.bytes, swapped.bytes + std::size(swapped.bytes));

            if (endian::system == endian::little) {
                Assert::AreEqual(int(system.value), int(swap_endian(system.value, endian::little)), L"little uint32_t", LINE_INFO());
                Assert::AreEqual(int(swapped.value), int(swap_endian(system.value, endian::big)), L"big uint32_t", LINE_INFO());
            } else {
                Assert::AreEqual(int(swapped.value), int(swap_endian(system.value, endian::little)), L"little uint32_t", LINE_INFO());
                Assert::AreEqual(int(system.value), int(swap_endian(system.value, endian::big)), L"big uint32_t", LINE_INFO());
            }
        }

        TEST_METHOD(swap64) {
            union {
                std::uint64_t value;
                std::uint8_t bytes[8];
            } swapped, system;

            system.bytes[0] = 42;
            system.bytes[1] = 43;
            system.bytes[2] = 44;
            system.bytes[3] = 45;
            system.bytes[4] = 46;
            system.bytes[5] = 47;
            system.bytes[6] = 48;
            system.bytes[7] = 49;

            swapped.value = system.value;
            std::reverse(swapped.bytes, swapped.bytes + std::size(swapped.bytes));

            if (endian::system == endian::little) {
                Assert::AreEqual(int(system.value), int(swap_endian(system.value, endian::little)), L"little uint64_t", LINE_INFO());
                Assert::AreEqual(int(swapped.value), int(swap_endian(system.value, endian::big)), L"big uint64_t", LINE_INFO());
            } else {
                Assert::AreEqual(int(swapped.value), int(swap_endian(system.value, endian::little)), L"little uint64_t", LINE_INFO());
                Assert::AreEqual(int(system.value), int(swap_endian(system.value, endian::big)), L"big uint64_t", LINE_INFO());
            }
        }
    };
}
