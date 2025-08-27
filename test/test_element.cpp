// <copyright file="test_element.cpp" company="Visualisierungsinstitut der Universität Stuttgart">
// Copyright © 2023 Visualisierungsinstitut der Universität Stuttgart.
// Licensed under the MIT licence. See LICENCE file for details.
// </copyright>
// <author>Christoph Müller</author>

#include "pch.h"


namespace fpd {

    TEST_CLASS(test_element) {

    public:

        TEST_METHOD(default_ctor) {
            {
                element<char> e;
                Assert::AreEqual(int(format::unknown), int(e.format()), L"format", LINE_INFO());
                Assert::AreEqual(0, int(e.offset()), L"offset", LINE_INFO());
                Assert::AreEqual(int(semantic::unknown), int(e.semantic()), L"semantic", LINE_INFO());
                Assert::AreEqual(std::string(), e.semantic_name(), L"semantic_name", LINE_INFO());
                Assert::AreEqual(0, int(e.size()), L"size", LINE_INFO());
            }

            {
                element<wchar_t> e;
                Assert::AreEqual(int(format::unknown), int(e.format()), L"format", LINE_INFO());
                Assert::AreEqual(0, int(e.offset()), L"offset", LINE_INFO());
                Assert::AreEqual(int(semantic::unknown), int(e.semantic()), L"semantic", LINE_INFO());
                Assert::AreEqual(std::wstring(), e.semantic_name(), L"semantic_name", LINE_INFO());
                Assert::AreEqual(0, int(e.size()), L"size", LINE_INFO());
            }
        }

        TEST_METHOD(full_ctor) {
            {
                element<char> e(semantic::position, "bla", format::float32xyz, 42, 0);
                Assert::AreEqual(int(format::float32xyz), int(e.format()), L"format", LINE_INFO());
                Assert::AreEqual(42, int(e.offset()), L"offset", LINE_INFO());
                Assert::AreEqual(int(semantic::position), int(e.semantic()), L"semantic", LINE_INFO());
                Assert::AreEqual(std::string("position"), e.semantic_name(), L"semantic_name", LINE_INFO());
                Assert::AreEqual(3 * sizeof(float), std::size_t(e.size()), L"size", LINE_INFO());
            }

            {
                element<wchar_t> e(semantic::position_radius, L"bla", format::float32xyzw, 42, 0);
                Assert::AreEqual(int(format::float32xyzw), int(e.format()), L"format", LINE_INFO());
                Assert::AreEqual(42, int(e.offset()), L"offset", LINE_INFO());
                Assert::AreEqual(int(semantic::position_radius), int(e.semantic()), L"semantic", LINE_INFO());
                Assert::AreEqual(std::wstring(L"position_radius"), e.semantic_name(), L"semantic_name", LINE_INFO());
                Assert::AreEqual(4 * sizeof(float), std::size_t(e.size()), L"size", LINE_INFO());
            }

            {
                element<char> e(semantic::user_defined, "bla", format::int32xyz, 42, 0);
                Assert::AreEqual(int(format::int32xyz), int(e.format()), L"format", LINE_INFO());
                Assert::AreEqual(42, int(e.offset()), L"offset", LINE_INFO());
                Assert::AreEqual(std::uint64_t(semantic::user_defined), std::uint64_t(e.semantic()), L"semantic", LINE_INFO());
                Assert::AreEqual(std::string("bla"), e.semantic_name(), L"semantic_name", LINE_INFO());
                Assert::AreEqual(3 * sizeof(float), std::size_t(e.size()), L"size", LINE_INFO());
            }

            {
                element<wchar_t> e(user_defined_semantic(43), L"bla", format::uint32xyzw, 42, 0);
                Assert::AreEqual(int(format::uint32xyzw), int(e.format()), L"format", LINE_INFO());
                Assert::AreEqual(42, int(e.offset()), L"offset", LINE_INFO());
                Assert::AreEqual(int(user_defined_semantic(43)), int(e.semantic()), L"semantic", LINE_INFO());
                Assert::AreEqual(std::wstring(L"bla"), e.semantic_name(), L"semantic_name", LINE_INFO());
                Assert::AreEqual(4 * sizeof(float), std::size_t(e.size()), L"size", LINE_INFO());
            }

            {
                element<char> e(semantic::user_defined, "bla", format::unknown, 42, 2);
                Assert::AreEqual(int(format::unknown), int(e.format()), L"format", LINE_INFO());
                Assert::AreEqual(42, int(e.offset()), L"offset", LINE_INFO());
                Assert::AreEqual(std::uint64_t(semantic::user_defined), std::uint64_t(e.semantic()), L"semantic", LINE_INFO());
                Assert::AreEqual(std::string("bla"), e.semantic_name(), L"semantic_name", LINE_INFO());
                Assert::AreEqual(std::size_t(2), std::size_t(e.size()), L"size", LINE_INFO());
            }

            {
                element<wchar_t> e(user_defined_semantic(43), L"bla", format::unknown, 42, 17);
                Assert::AreEqual(int(format::unknown), int(e.format()), L"format", LINE_INFO());
                Assert::AreEqual(42, int(e.offset()), L"offset", LINE_INFO());
                Assert::AreEqual(std::uint64_t(user_defined_semantic(43)), std::uint64_t(e.semantic()), L"semantic", LINE_INFO());
                Assert::AreEqual(std::wstring(L"bla"), e.semantic_name(), L"semantic_name", LINE_INFO());
                Assert::AreEqual(std::size_t(17), std::size_t(e.size()), L"size", LINE_INFO());
            }

            Assert::ExpectException<std::invalid_argument>([](void) {
                element<char> e(semantic::user_defined, "", format::int32xyz, 42, 0);
            }, L"Empty user-defined semantic", LINE_INFO());

            Assert::ExpectException<std::invalid_argument>([](void) {
                element<wchar_t> e(user_defined_semantic(43), L"", format::int32xyz, 42, 0);
            }, L"Empty user-defined semantic", LINE_INFO());

            Assert::ExpectException<std::invalid_argument>([](void) {
                element<char> e(semantic::position, "", format::unknown, 42, 0);
            }, L"Unknown format", LINE_INFO());

            Assert::ExpectException<std::invalid_argument>([](void) {
                element<wchar_t> e(semantic::position, L"", format::unknown, 42, 0);
            }, L"Unknown format", LINE_INFO());
        }

        TEST_METHOD(built_in_ctor) {
            {
                element<char> e(semantic::position, format::float32xyz, 42, 0);
                Assert::AreEqual(int(format::float32xyz), int(e.format()), L"format", LINE_INFO());
                Assert::AreEqual(42, int(e.offset()), L"offset", LINE_INFO());
                Assert::AreEqual(int(semantic::position), int(e.semantic()), L"semantic", LINE_INFO());
                Assert::AreEqual(std::string("position"), e.semantic_name(), L"semantic_name", LINE_INFO());
                Assert::AreEqual(3 * sizeof(float), std::size_t(e.size()), L"size", LINE_INFO());
            }

            {
                element<wchar_t> e(semantic::position_radius, format::float32xyzw, 42, 0);
                Assert::AreEqual(int(format::float32xyzw), int(e.format()), L"format", LINE_INFO());
                Assert::AreEqual(42, int(e.offset()), L"offset", LINE_INFO());
                Assert::AreEqual(int(semantic::position_radius), int(e.semantic()), L"semantic", LINE_INFO());
                Assert::AreEqual(std::wstring(L"position_radius"), e.semantic_name(), L"semantic_name", LINE_INFO());
                Assert::AreEqual(4 * sizeof(float), std::size_t(e.size()), L"size", LINE_INFO());
            }

            {
                element<char> e(semantic::position_radius, format::unknown, 42, 2);
                Assert::AreEqual(int(format::unknown), int(e.format()), L"format", LINE_INFO());
                Assert::AreEqual(42, int(e.offset()), L"offset", LINE_INFO());
                Assert::AreEqual(int(semantic::position_radius), int(e.semantic()), L"semantic", LINE_INFO());
                Assert::AreEqual(std::string("position_radius"), e.semantic_name(), L"semantic_name", LINE_INFO());
                Assert::AreEqual(std::size_t(2), std::size_t(e.size()), L"size", LINE_INFO());
            }

            {
                element<wchar_t> e(semantic::position_radius, format::unknown, 42, 17);
                Assert::AreEqual(int(format::unknown), int(e.format()), L"format", LINE_INFO());
                Assert::AreEqual(42, int(e.offset()), L"offset", LINE_INFO());
                Assert::AreEqual(int(semantic::position_radius), int(e.semantic()), L"semantic", LINE_INFO());
                Assert::AreEqual(std::wstring(L"position_radius"), e.semantic_name(), L"semantic_name", LINE_INFO());
                Assert::AreEqual(std::size_t(17), std::size_t(e.size()), L"size", LINE_INFO());
            }

            Assert::ExpectException<std::invalid_argument>([](void) {
                element<char> e(semantic::user_defined, format::unknown, 42, 2);
            }, L"User-defined semantic", LINE_INFO());

            Assert::ExpectException<std::invalid_argument>([](void) {
                element<wchar_t> e(user_defined_semantic(17), format::unknown, 42, 17);
            }, L"User-defined semantic", LINE_INFO());

            Assert::ExpectException<std::invalid_argument>([](void) {
                element<char> e(semantic::position, format::unknown, 42, 0);
            }, L"Unknown format", LINE_INFO());

            Assert::ExpectException<std::invalid_argument>([](void) {
                element<wchar_t> e(semantic::position, format::unknown, 42, 0);
            }, L"Unknown format", LINE_INFO());
        }


        TEST_METHOD(initialiser_ctor) {
            {
                element_initialiser<char> i(semantic::position, "bla", format::float32xyz, 28, 42, 0);
                element<char> e(i);
                Assert::AreEqual(int(format::float32xyz), int(e.format()), L"format", LINE_INFO());
                Assert::AreEqual(42, int(e.offset()), L"offset", LINE_INFO());
                Assert::AreEqual(int(semantic::position), int(e.semantic()), L"semantic", LINE_INFO());
                Assert::AreEqual(std::string("position"), e.semantic_name(), L"semantic_name", LINE_INFO());
                Assert::AreEqual(3 * sizeof(float), std::size_t(e.size()), L"size", LINE_INFO());
            }

            {
                element_initialiser<wchar_t> i(semantic::position_radius, L"bla", format::float32xyzw, 28, 42, 0);
                element<wchar_t>e(i);
                Assert::AreEqual(int(format::float32xyzw), int(e.format()), L"format", LINE_INFO());
                Assert::AreEqual(42, int(e.offset()), L"offset", LINE_INFO());
                Assert::AreEqual(int(semantic::position_radius), int(e.semantic()), L"semantic", LINE_INFO());
                Assert::AreEqual(std::wstring(L"position_radius"), e.semantic_name(), L"semantic_name", LINE_INFO());
                Assert::AreEqual(4 * sizeof(float), std::size_t(e.size()), L"size", LINE_INFO());
            }

            {
                element_initialiser<char> i(semantic::user_defined, "bla", format::int32xyz, 28, 42, 0);
                element<char>e(i);
                Assert::AreEqual(int(format::int32xyz), int(e.format()), L"format", LINE_INFO());
                Assert::AreEqual(42, int(e.offset()), L"offset", LINE_INFO());
                Assert::AreEqual(std::uint64_t(semantic::user_defined), std::uint64_t(e.semantic()), L"semantic", LINE_INFO());
                Assert::AreEqual(std::string("bla"), e.semantic_name(), L"semantic_name", LINE_INFO());
                Assert::AreEqual(3 * sizeof(float), std::size_t(e.size()), L"size", LINE_INFO());
            }

            {
                element_initialiser<wchar_t> i(user_defined_semantic(43), L"bla", format::uint32xyzw, 28, 42, 0);
                element<wchar_t>e(i);
                Assert::AreEqual(int(format::uint32xyzw), int(e.format()), L"format", LINE_INFO());
                Assert::AreEqual(42, int(e.offset()), L"offset", LINE_INFO());
                Assert::AreEqual(int(user_defined_semantic(43)), int(e.semantic()), L"semantic", LINE_INFO());
                Assert::AreEqual(std::wstring(L"bla"), e.semantic_name(), L"semantic_name", LINE_INFO());
                Assert::AreEqual(4 * sizeof(float), std::size_t(e.size()), L"size", LINE_INFO());
            }

            {
                element_initialiser<char> i(semantic::user_defined, "bla", format::unknown, 28, 42, 2);
                element<char>e(i);
                Assert::AreEqual(int(format::unknown), int(e.format()), L"format", LINE_INFO());
                Assert::AreEqual(42, int(e.offset()), L"offset", LINE_INFO());
                Assert::AreEqual(std::uint64_t(semantic::user_defined), std::uint64_t(e.semantic()), L"semantic", LINE_INFO());
                Assert::AreEqual(std::string("bla"), e.semantic_name(), L"semantic_name", LINE_INFO());
                Assert::AreEqual(std::size_t(2), std::size_t(e.size()), L"size", LINE_INFO());
            }

            {
                element_initialiser<wchar_t> i(user_defined_semantic(43), L"bla", format::unknown, 28, 42, 17);
                element<wchar_t>e(i);
                Assert::AreEqual(int(format::unknown), int(e.format()), L"format", LINE_INFO());
                Assert::AreEqual(42, int(e.offset()), L"offset", LINE_INFO());
                Assert::AreEqual(int(user_defined_semantic(43)), int(e.semantic()), L"semantic", LINE_INFO());
                Assert::AreEqual(std::wstring(L"bla"), e.semantic_name(), L"semantic_name", LINE_INFO());
                Assert::AreEqual(std::size_t(17), std::size_t(e.size()), L"size", LINE_INFO());
            }

            Assert::ExpectException<std::invalid_argument>([](void) {
                element_initialiser<char> i(semantic::user_defined, "", format::int32xyz, 28, 42, 0);
                element<char>e(i);
            }, L"Empty user-defined semantic", LINE_INFO());

            Assert::ExpectException<std::invalid_argument>([](void) {
                element_initialiser<wchar_t> i(user_defined_semantic(43), L"", format::int32xyz, 28, 42, 0);
                element<wchar_t> e(i);
            }, L"Empty user-defined semantic", LINE_INFO());

            Assert::ExpectException<std::invalid_argument>([](void) {
                element_initialiser<char> i(semantic::position, "", format::unknown, 28, 42, 0);
                element<char>e(i);
            }, L"Unknown format", LINE_INFO());

            Assert::ExpectException<std::invalid_argument>([](void) {
                element_initialiser<wchar_t> i(semantic::position, L"", format::unknown, 28, 42, 0);
                element<wchar_t>e(i);
            }, L"Unknown format", LINE_INFO());
        }

    };
}
