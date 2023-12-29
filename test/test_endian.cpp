// <copyright file="test_endian.cpp" company="Visualisierungsinstitut der Universität Stuttgart">
// Copyright © 2023 Visualisierungsinstitut der Universität Stuttgart.
// Licensed under the MIT licence. See LICENCE file for details.
// </copyright>
// <author>Christoph Müller</author>

#include "pch.h"
#include "CppUnitTest.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;


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
        }

        TEST_METHOD(swap16) {
            union {
                std::uint16_t value;
                std::uint8_t bytes[2];
            } expected, input;

            input.bytes[0] = 42;
            input.bytes[1] = 43;

            expected.bytes[0] = input.bytes[1];
            expected.bytes[1] = input.bytes[0];

            if (endian::system == endian::little) {
                Assert::AreEqual(int(input.value), int(swap_endian(input.value, endian::little)), L"little uint16_t", LINE_INFO());
                Assert::AreEqual(int(expected.value), int(swap_endian(input.value, endian::big)), L"big uint16_t", LINE_INFO());
            } else {
                Assert::AreEqual(int(expected.value), int(swap_endian(input.value, endian::little)), L"little uint16_t", LINE_INFO());
                Assert::AreEqual(int(input.value), int(swap_endian(input.value, endian::big)), L"big uint16_t", LINE_INFO());
            }
        }

        TEST_METHOD(swap32) {
            union {
                std::uint32_t value;
                std::uint8_t bytes[4];
            } expected, input;

            input.bytes[0] = 42;
            input.bytes[1] = 43;
            input.bytes[2] = 44;
            input.bytes[3] = 45;

            expected.bytes[0] = input.bytes[3];
            expected.bytes[1] = input.bytes[2];
            expected.bytes[2] = input.bytes[1];
            expected.bytes[3] = input.bytes[0];

            if (endian::system == endian::little) {
                Assert::AreEqual(input.value, swap_endian(input.value, endian::little), L"little uint32_t", LINE_INFO());
                Assert::AreEqual(expected.value, swap_endian(input.value, endian::big), L"big uint32_t", LINE_INFO());
            } else {
                Assert::AreEqual(expected.value, swap_endian(input.value, endian::little), L"little uint32_t", LINE_INFO());
                Assert::AreEqual(input.value, swap_endian(input.value, endian::big), L"big uint32_t", LINE_INFO());
            }
        }

        TEST_METHOD(swap64) {
            union {
                std::uint64_t value;
                std::uint8_t bytes[8];
            } expected, input;

            input.bytes[0] = 42;
            input.bytes[1] = 43;
            input.bytes[2] = 44;
            input.bytes[3] = 45;
            input.bytes[4] = 46;
            input.bytes[5] = 47;
            input.bytes[6] = 48;
            input.bytes[7] = 49;

            expected.bytes[0] = input.bytes[7];
            expected.bytes[1] = input.bytes[6];
            expected.bytes[2] = input.bytes[5];
            expected.bytes[3] = input.bytes[4];
            expected.bytes[4] = input.bytes[3];
            expected.bytes[5] = input.bytes[2];
            expected.bytes[6] = input.bytes[1];
            expected.bytes[7] = input.bytes[0];

            if (endian::system == endian::little) {
                Assert::AreEqual(input.value, swap_endian(input.value, endian::little), L"little uint64_t", LINE_INFO());
                Assert::AreEqual(expected.value, swap_endian(input.value, endian::big), L"big uint64_t", LINE_INFO());
            } else {
                Assert::AreEqual(expected.value, swap_endian(input.value, endian::little), L"little uint64_t", LINE_INFO());
                Assert::AreEqual(input.value, swap_endian(input.value, endian::big), L"big uint64_t", LINE_INFO());
            }
        }
    };
}
