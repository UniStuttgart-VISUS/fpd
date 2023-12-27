// <copyright file="test_element_group.cpp" company="Visualisierungsinstitut der Universität Stuttgart">
// Copyright © 2023 Visualisierungsinstitut der Universität Stuttgart.
// Licensed under the MIT licence. See LICENCE file for details.
// </copyright>
// <author>Christoph Müller</author>

#include "pch.h"
#include "CppUnitTest.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;


namespace fpd {

    TEST_CLASS(test_element_group) {

    public:

        TEST_METHOD(default_ctor) {
            {
                element_group<char> g;
                Assert::AreEqual(std::size_t(0), g.count(), L"count", LINE_INFO());
                Assert::AreEqual(int(0), int(g.size()), L"size", LINE_INFO());
            }

            {
                element_group<wchar_t> g;
                Assert::AreEqual(std::size_t(0), g.count(), L"count", LINE_INFO());
                Assert::AreEqual(int(0), int(g.size()), L"size", LINE_INFO());
            }
        }

        TEST_METHOD(emplace_add) {
            element_group<wchar_t> g;
            Assert::AreEqual(std::size_t(0), g.count(), L"initial count", LINE_INFO());
            Assert::AreEqual(int(0), int(g.size()), L"initial size", LINE_INFO());

            g.add(semantic::position, nullptr, format::float32xyz);
            Assert::AreEqual(std::size_t(1), g.count(), L"count after add", LINE_INFO());
            Assert::AreEqual(3 * sizeof(float), std::size_t(g.size()), L"size after add", LINE_INFO());

            Assert::AreEqual(int(format::float32xyz), int(g[0].format()), L"format@0", LINE_INFO());
            Assert::AreEqual(int(0), int(g[0].offset()), L"offset@0", LINE_INFO());
            Assert::AreEqual(int(semantic::position), int(g[0].semantic()), L"semantic@0", LINE_INFO());
            Assert::AreEqual(std::wstring(L"position"), g[0].semantic_name(), L"semantic_name@0", LINE_INFO());
            Assert::AreEqual(3 * sizeof(float), std::size_t(g[0].size()), L"size@0", LINE_INFO());

            g.add(semantic::colour, nullptr, format::uint8xyzw);
            Assert::AreEqual(std::size_t(2), g.count(), L"count after second add", LINE_INFO());
            Assert::AreEqual(3 * sizeof(float) + 4, std::size_t(g.size()), L"size after second add", LINE_INFO());

            Assert::AreEqual(int(format::float32xyz), int(g[0].format()), L"format@0", LINE_INFO());
            Assert::AreEqual(int(0), int(g[0].offset()), L"offset@0", LINE_INFO());
            Assert::AreEqual(int(semantic::position), int(g[0].semantic()), L"semantic@0", LINE_INFO());
            Assert::AreEqual(std::wstring(L"position"), g[0].semantic_name(), L"semantic_name@0", LINE_INFO());
            Assert::AreEqual(3 * sizeof(float), std::size_t(g[0].size()), L"size@0", LINE_INFO());

            Assert::AreEqual(int(format::uint8xyzw), int(g[1].format()), L"format@1", LINE_INFO());
            Assert::AreEqual(int(0), int(g[1].offset()), L"offset@1", LINE_INFO());
            Assert::AreEqual(int(semantic::colour), int(g[1].semantic()), L"semantic@1", LINE_INFO());
            Assert::AreEqual(std::wstring(L"colour"), g[1].semantic_name(), L"semantic_name@1", LINE_INFO());
            Assert::AreEqual(std::size_t(4), std::size_t(g[1].size()), L"size@1", LINE_INFO());

            g.add(semantic::velocity, nullptr, format::float32xyz, 32);
            Assert::AreEqual(std::size_t(3), g.count(), L"count after second add", LINE_INFO());
            Assert::AreEqual(32 + 3 * sizeof(float), std::size_t(g.size()), L"size after third add", LINE_INFO());

            Assert::AreEqual(int(format::float32xyz), int(g[0].format()), L"format@0", LINE_INFO());
            Assert::AreEqual(int(0), int(g[0].offset()), L"offset@0", LINE_INFO());
            Assert::AreEqual(int(semantic::position), int(g[0].semantic()), L"semantic@0", LINE_INFO());
            Assert::AreEqual(std::wstring(L"position"), g[0].semantic_name(), L"semantic_name@0", LINE_INFO());
            Assert::AreEqual(3 * sizeof(float), std::size_t(g[0].size()), L"size@0", LINE_INFO());

            Assert::AreEqual(int(format::uint8xyzw), int(g[1].format()), L"format@1", LINE_INFO());
            Assert::AreEqual(int(0), int(g[1].offset()), L"offset@1", LINE_INFO());
            Assert::AreEqual(int(semantic::colour), int(g[1].semantic()), L"semantic@1", LINE_INFO());
            Assert::AreEqual(std::wstring(L"colour"), g[1].semantic_name(), L"semantic_name@1", LINE_INFO());
            Assert::AreEqual(std::size_t(4), std::size_t(g[1].size()), L"size@1", LINE_INFO());

            Assert::AreEqual(int(format::float32xyz), int(g[2].format()), L"format@2", LINE_INFO());
            Assert::AreEqual(int(32), int(g[2].offset()), L"offset@2", LINE_INFO());
            Assert::AreEqual(int(semantic::velocity), int(g[2].semantic()), L"semantic@2", LINE_INFO());
            Assert::AreEqual(std::wstring(L"velocity"), g[2].semantic_name(), L"semantic_name@2", LINE_INFO());
            Assert::AreEqual(3 * sizeof(float), std::size_t(g[2].size()), L"size@2", LINE_INFO());

            g.add(semantic::intensity, nullptr, format::float32, 0, 0, true);
            Assert::AreEqual(std::size_t(4), g.count(), L"count after third add", LINE_INFO());
            Assert::AreEqual(32 + 4 * sizeof(float), std::size_t(g.size()), L"size after third add", LINE_INFO());

            Assert::AreEqual(int(format::float32xyz), int(g[0].format()), L"format@0", LINE_INFO());
            Assert::AreEqual(int(0), int(g[0].offset()), L"offset@0", LINE_INFO());
            Assert::AreEqual(int(semantic::position), int(g[0].semantic()), L"semantic@0", LINE_INFO());
            Assert::AreEqual(std::wstring(L"position"), g[0].semantic_name(), L"semantic_name@0", LINE_INFO());
            Assert::AreEqual(3 * sizeof(float), std::size_t(g[0].size()), L"size@0", LINE_INFO());

            Assert::AreEqual(int(format::uint8xyzw), int(g[1].format()), L"format@1", LINE_INFO());
            Assert::AreEqual(int(0), int(g[1].offset()), L"offset@1", LINE_INFO());
            Assert::AreEqual(int(semantic::colour), int(g[1].semantic()), L"semantic@1", LINE_INFO());
            Assert::AreEqual(std::wstring(L"colour"), g[1].semantic_name(), L"semantic_name@1", LINE_INFO());
            Assert::AreEqual(std::size_t(4), std::size_t(g[1].size()), L"size@1", LINE_INFO());

            Assert::AreEqual(int(format::float32xyz), int(g[2].format()), L"format@2", LINE_INFO());
            Assert::AreEqual(int(32), int(g[2].offset()), L"offset@2", LINE_INFO());
            Assert::AreEqual(int(semantic::velocity), int(g[2].semantic()), L"semantic@2", LINE_INFO());
            Assert::AreEqual(std::wstring(L"velocity"), g[2].semantic_name(), L"semantic_name@2", LINE_INFO());
            Assert::AreEqual(3 * sizeof(float), std::size_t(g[2].size()), L"size@2", LINE_INFO());

            Assert::AreEqual(int(format::float32), int(g[3].format()), L"format@3", LINE_INFO());
            Assert::AreEqual(int(32 + 3 * sizeof(float)), int(g[3].offset()), L"offset@3", LINE_INFO());
            Assert::AreEqual(int(semantic::intensity), int(g[3].semantic()), L"semantic@3", LINE_INFO());
            Assert::AreEqual(std::wstring(L"intensity"), g[3].semantic_name(), L"semantic_name@3", LINE_INFO());
            Assert::AreEqual(sizeof(float), std::size_t(g[3].size()), L"size@3", LINE_INFO());
        }

        TEST_METHOD(move_add) {
            element_group<wchar_t> g;
            Assert::AreEqual(std::size_t(0), g.count(), L"initial count", LINE_INFO());
            Assert::AreEqual(int(0), int(g.size()), L"initial size", LINE_INFO());

            {
                decltype(g)::element_type e(semantic::position, format::float32xyz, 0, 0);
                g.add(std::move(e));
                Assert::AreEqual(std::size_t(1), g.count(), L"count after add", LINE_INFO());
                Assert::AreEqual(3 * sizeof(float), std::size_t(g.size()), L"size after add", LINE_INFO());
            }

            Assert::AreEqual(int(format::float32xyz), int(g[0].format()), L"format@0", LINE_INFO());
            Assert::AreEqual(int(0), int(g[0].offset()), L"offset@0", LINE_INFO());
            Assert::AreEqual(int(semantic::position), int(g[0].semantic()), L"semantic@0", LINE_INFO());
            Assert::AreEqual(std::wstring(L"position"), g[0].semantic_name(), L"semantic_name@0", LINE_INFO());
            Assert::AreEqual(3 * sizeof(float), std::size_t(g[0].size()), L"size@0", LINE_INFO());

            {
                decltype(g)::element_type e(semantic::colour, format::uint8xyzw, 0, 0);
                g.add(std::move(e));
                Assert::AreEqual(std::size_t(2), g.count(), L"count after second add", LINE_INFO());
                Assert::AreEqual(3 * sizeof(float) + 4, std::size_t(g.size()), L"size after second add", LINE_INFO());
            }

            Assert::AreEqual(int(format::float32xyz), int(g[0].format()), L"format@0", LINE_INFO());
            Assert::AreEqual(int(0), int(g[0].offset()), L"offset@0", LINE_INFO());
            Assert::AreEqual(int(semantic::position), int(g[0].semantic()), L"semantic@0", LINE_INFO());
            Assert::AreEqual(std::wstring(L"position"), g[0].semantic_name(), L"semantic_name@0", LINE_INFO());
            Assert::AreEqual(3 * sizeof(float), std::size_t(g[0].size()), L"size@0", LINE_INFO());

            Assert::AreEqual(int(format::uint8xyzw), int(g[1].format()), L"format@1", LINE_INFO());
            Assert::AreEqual(int(0), int(g[1].offset()), L"offset@1", LINE_INFO());
            Assert::AreEqual(int(semantic::colour), int(g[1].semantic()), L"semantic@1", LINE_INFO());
            Assert::AreEqual(std::wstring(L"colour"), g[1].semantic_name(), L"semantic_name@1", LINE_INFO());
            Assert::AreEqual(std::size_t(4), std::size_t(g[1].size()), L"size@1", LINE_INFO());

            {
                decltype(g)::element_type e(semantic::velocity, format::float32xyz, 32, 0);
                g.add(std::move(e));
                Assert::AreEqual(std::size_t(3), g.count(), L"count after second add", LINE_INFO());
                Assert::AreEqual(32 + 3 * sizeof(float), std::size_t(g.size()), L"size after third add", LINE_INFO());
            }

            Assert::AreEqual(int(format::float32xyz), int(g[0].format()), L"format@0", LINE_INFO());
            Assert::AreEqual(int(0), int(g[0].offset()), L"offset@0", LINE_INFO());
            Assert::AreEqual(int(semantic::position), int(g[0].semantic()), L"semantic@0", LINE_INFO());
            Assert::AreEqual(std::wstring(L"position"), g[0].semantic_name(), L"semantic_name@0", LINE_INFO());
            Assert::AreEqual(3 * sizeof(float), std::size_t(g[0].size()), L"size@0", LINE_INFO());

            Assert::AreEqual(int(format::uint8xyzw), int(g[1].format()), L"format@1", LINE_INFO());
            Assert::AreEqual(int(0), int(g[1].offset()), L"offset@1", LINE_INFO());
            Assert::AreEqual(int(semantic::colour), int(g[1].semantic()), L"semantic@1", LINE_INFO());
            Assert::AreEqual(std::wstring(L"colour"), g[1].semantic_name(), L"semantic_name@1", LINE_INFO());
            Assert::AreEqual(std::size_t(4), std::size_t(g[1].size()), L"size@1", LINE_INFO());

            Assert::AreEqual(int(format::float32xyz), int(g[2].format()), L"format@2", LINE_INFO());
            Assert::AreEqual(int(32), int(g[2].offset()), L"offset@2", LINE_INFO());
            Assert::AreEqual(int(semantic::velocity), int(g[2].semantic()), L"semantic@2", LINE_INFO());
            Assert::AreEqual(std::wstring(L"velocity"), g[2].semantic_name(), L"semantic_name@2", LINE_INFO());
            Assert::AreEqual(3 * sizeof(float), std::size_t(g[2].size()), L"size@2", LINE_INFO());

            {
                decltype(g)::element_type e(semantic::intensity, format::float32, 0, 0);
                g.add(std::move(e));
                Assert::AreEqual(std::size_t(4), g.count(), L"count after third add", LINE_INFO());
                Assert::AreEqual(32 + 4 * sizeof(float), std::size_t(g.size()), L"size after third add", LINE_INFO());
            }

            Assert::AreEqual(int(format::float32xyz), int(g[0].format()), L"format@0", LINE_INFO());
            Assert::AreEqual(int(0), int(g[0].offset()), L"offset@0", LINE_INFO());
            Assert::AreEqual(int(semantic::position), int(g[0].semantic()), L"semantic@0", LINE_INFO());
            Assert::AreEqual(std::wstring(L"position"), g[0].semantic_name(), L"semantic_name@0", LINE_INFO());
            Assert::AreEqual(3 * sizeof(float), std::size_t(g[0].size()), L"size@0", LINE_INFO());

            Assert::AreEqual(int(format::uint8xyzw), int(g[1].format()), L"format@1", LINE_INFO());
            Assert::AreEqual(int(0), int(g[1].offset()), L"offset@1", LINE_INFO());
            Assert::AreEqual(int(semantic::colour), int(g[1].semantic()), L"semantic@1", LINE_INFO());
            Assert::AreEqual(std::wstring(L"colour"), g[1].semantic_name(), L"semantic_name@1", LINE_INFO());
            Assert::AreEqual(std::size_t(4), std::size_t(g[1].size()), L"size@1", LINE_INFO());

            Assert::AreEqual(int(format::float32xyz), int(g[2].format()), L"format@2", LINE_INFO());
            Assert::AreEqual(int(32), int(g[2].offset()), L"offset@2", LINE_INFO());
            Assert::AreEqual(int(semantic::velocity), int(g[2].semantic()), L"semantic@2", LINE_INFO());
            Assert::AreEqual(std::wstring(L"velocity"), g[2].semantic_name(), L"semantic_name@2", LINE_INFO());
            Assert::AreEqual(3 * sizeof(float), std::size_t(g[2].size()), L"size@2", LINE_INFO());

            Assert::AreEqual(int(format::float32), int(g[3].format()), L"format@3", LINE_INFO());
            Assert::AreEqual(int(0), int(g[3].offset()), L"offset@3", LINE_INFO());
            Assert::AreEqual(int(semantic::intensity), int(g[3].semantic()), L"semantic@3", LINE_INFO());
            Assert::AreEqual(std::wstring(L"intensity"), g[3].semantic_name(), L"semantic_name@3", LINE_INFO());
            Assert::AreEqual(sizeof(float), std::size_t(g[3].size()), L"size@3", LINE_INFO());
        }

    };
}
