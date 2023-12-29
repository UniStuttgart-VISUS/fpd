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
            Assert::AreEqual(expected, swap_endian(expected, endian::little), L"little single", LINE_INFO());
            Assert::AreEqual(expected, swap_endian(expected, endian::big), L"big single", LINE_INFO());

            std::array<std::uint8_t, 42> input;
            std::fill(input.begin(), input.end(), expected);

            decltype(input) output;
            std::fill(input.begin(), input.end(), static_cast<std::uint8_t>(0));

            swap_endian(output.data(), input.data(), input.size(), endian::little);
            for (std::size_t i = 0; i < output.size(); ++i) {
                Assert::AreEqual(input[i], output[i], L"little copy", LINE_INFO());
            }

            swap_endian(output.data(), input.data(), input.size(), endian::big);
            for (std::size_t i = 0; i < output.size(); ++i) {
                Assert::AreEqual(input[i], output[i], L"big copy", LINE_INFO());
            }

            swap_endian(input.data(), input.size(), endian::little);
            for (std::size_t i = 0; i < output.size(); ++i) {
                Assert::AreEqual(output[i], input[i], L"little in-place", LINE_INFO());
            }

            swap_endian(input.data(), input.size(), endian::big);
            for (std::size_t i = 0; i < output.size(); ++i) {
                Assert::AreEqual(output[i], input[i], L"big in-place", LINE_INFO());
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

            const auto big = int((endian::system == endian::little) ? swapped.value : system.value);
            const auto little = int((endian::system == endian::little) ? system.value : swapped.value);

            Assert::AreEqual(little, int(swap_endian(system.value, endian::little)), L"little single", LINE_INFO());
            Assert::AreEqual(big, int(swap_endian(system.value, endian::big)), L"big single", LINE_INFO());

            std::array<decltype(swapped.value), 42> input;
            std::fill(input.begin(), input.end(), system.value);
            std::array<decltype(swapped.value), 42> output;

            std::fill(output.begin(), output.end(), static_cast<decltype(swapped.value)>(0));
            swap_endian(output.data(), input.data(), input.size(), endian::little);
            for (std::size_t i = 0; i < output.size(); ++i) {
                Assert::AreEqual(little, int(output[i]), L"little copy", LINE_INFO());
            }

            std::fill(output.begin(), output.end(), static_cast<decltype(swapped.value)>(0));
            swap_endian(output.data(), input.data(), input.size(), endian::big);
            for (std::size_t i = 0; i < output.size(); ++i) {
                Assert::AreEqual(big, int(output[i]), L"big copy", LINE_INFO());
            }

            std::fill(output.begin(), output.end(), system.value);
            swap_endian(output.data(), output.size(), endian::little);
            for (std::size_t i = 0; i < output.size(); ++i) {
                Assert::AreEqual(little, int(output[i]), L"little in-place", LINE_INFO());
            }

            std::fill(output.begin(), output.end(), system.value);
            swap_endian(output.data(), output.size(), endian::big);
            for (std::size_t i = 0; i < output.size(); ++i) {
                Assert::AreEqual(big, int(output[i]), L"big in-place", LINE_INFO());
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

            const auto big = (endian::system == endian::little) ? swapped.value : system.value;
            const auto little = (endian::system == endian::little) ? system.value : swapped.value;

            Assert::AreEqual(little, swap_endian(system.value, endian::little), L"little single", LINE_INFO());
            Assert::AreEqual(big, swap_endian(system.value, endian::big), L"big single", LINE_INFO());

            std::array<decltype(swapped.value), 42> input;
            std::fill(input.begin(), input.end(), system.value);
            std::array<decltype(swapped.value), 42> output;

            std::fill(output.begin(), output.end(), static_cast<decltype(swapped.value)>(0));
            swap_endian(output.data(), input.data(), input.size(), endian::little);
            for (std::size_t i = 0; i < output.size(); ++i) {
                Assert::AreEqual(little, output[i], L"little copy", LINE_INFO());
            }

            std::fill(output.begin(), output.end(), static_cast<decltype(swapped.value)>(0));
            swap_endian(output.data(), input.data(), input.size(), endian::big);
            for (std::size_t i = 0; i < output.size(); ++i) {
                Assert::AreEqual(big, output[i], L"big copy", LINE_INFO());
            }

            std::fill(output.begin(), output.end(), system.value);
            swap_endian(output.data(), output.size(), endian::little);
            for (std::size_t i = 0; i < output.size(); ++i) {
                Assert::AreEqual(little, output[i], L"little in-place", LINE_INFO());
            }

            std::fill(output.begin(), output.end(), system.value);
            swap_endian(output.data(), output.size(), endian::big);
            for (std::size_t i = 0; i < output.size(); ++i) {
                Assert::AreEqual(big, output[i], L"big in-place", LINE_INFO());
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

            const auto big = (endian::system == endian::little) ? swapped.value : system.value;
            const auto little = (endian::system == endian::little) ? system.value : swapped.value;

            Assert::AreEqual(little, swap_endian(system.value, endian::little), L"little single", LINE_INFO());
            Assert::AreEqual(big, swap_endian(system.value, endian::big), L"big single", LINE_INFO());

            std::array<decltype(swapped.value), 42> input;
            std::fill(input.begin(), input.end(), system.value);
            std::array<decltype(swapped.value), 42> output;

            std::fill(output.begin(), output.end(), static_cast<decltype(swapped.value)>(0));
            swap_endian(output.data(), input.data(), input.size(), endian::little);
            for (std::size_t i = 0; i < output.size(); ++i) {
                Assert::AreEqual(little, output[i], L"little copy", LINE_INFO());
            }

            std::fill(output.begin(), output.end(), static_cast<decltype(swapped.value)>(0));
            swap_endian(output.data(), input.data(), input.size(), endian::big);
            for (std::size_t i = 0; i < output.size(); ++i) {
                Assert::AreEqual(big, output[i], L"big copy", LINE_INFO());
            }

            std::fill(output.begin(), output.end(), system.value);
            swap_endian(output.data(), output.size(), endian::little);
            for (std::size_t i = 0; i < output.size(); ++i) {
                Assert::AreEqual(little, output[i], L"little in-place", LINE_INFO());
            }

            std::fill(output.begin(), output.end(), system.value);
            swap_endian(output.data(), output.size(), endian::big);
            for (std::size_t i = 0; i < output.size(); ++i) {
                Assert::AreEqual(big, output[i], L"big in-place", LINE_INFO());
            }
        }
    };
}
