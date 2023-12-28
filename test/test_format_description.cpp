// <copyright file="test_format_description.cpp" company="Visualisierungsinstitut der Universität Stuttgart">
// Copyright © 2023 Visualisierungsinstitut der Universität Stuttgart.
// Licensed under the MIT licence. See LICENCE file for details.
// </copyright>
// <author>Christoph Müller</author>

#include "pch.h"
#include "CppUnitTest.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;


namespace fpd {

    TEST_CLASS(test_format_description) {

    public:

        TEST_METHOD(add) {
            format_description<char> fd;
            Assert::AreEqual(std::size_t(0), fd.count(), L"initial count", LINE_INFO());

            fd.add(semantic::position_radius, format::float32xyzw, 0, 0, 0, true);
            Assert::AreEqual(std::size_t(1), fd.count(), L"First group added", LINE_INFO());
            Assert::AreEqual(std::size_t(1), fd[0].count(), L"First group has one element", LINE_INFO());

            Assert::AreEqual(int(format::float32xyzw), int(fd(0, 0).format()), L"format@0, 0", LINE_INFO());
            Assert::AreEqual(int(0), int(fd(0, 0).offset()), L"offset@0, 0", LINE_INFO());
            Assert::AreEqual(int(semantic::position_radius), int(fd(0, 0).semantic()), L"semantic@0, 0", LINE_INFO());
            Assert::AreEqual(std::string("position_radius"), fd(0, 0).semantic_name(), L"semantic_name@0, 0", LINE_INFO());
            Assert::AreEqual(4 * sizeof(float), std::size_t(fd(0, 0).size()), L"size@0, 0", LINE_INFO());

            fd.add(semantic::colour, format::uint8xyzw, 1, 0, 0, false);
            Assert::AreEqual(std::size_t(2), fd.count(), L"Second group added", LINE_INFO());
            Assert::AreEqual(std::size_t(1), fd[0].count(), L"First group has one element", LINE_INFO());
            Assert::AreEqual(std::size_t(1), fd[1].count(), L"Second group has one element", LINE_INFO());

            Assert::AreEqual(int(format::float32xyzw), int(fd(0, 0).format()), L"format@0, 0", LINE_INFO());
            Assert::AreEqual(int(0), int(fd(0, 0).offset()), L"offset@0, 0", LINE_INFO());
            Assert::AreEqual(int(semantic::position_radius), int(fd(0, 0).semantic()), L"semantic@0, 0", LINE_INFO());
            Assert::AreEqual(std::string("position_radius"), fd(0, 0).semantic_name(), L"semantic_name@0, 0", LINE_INFO());
            Assert::AreEqual(4 * sizeof(float), std::size_t(fd(0, 0).size()), L"size@0, 0", LINE_INFO());

            Assert::AreEqual(int(format::uint8xyzw), int(fd(1, 0).format()), L"format@1, 0", LINE_INFO());
            Assert::AreEqual(int(0), int(fd(1, 0).offset()), L"offset@1, 0", LINE_INFO());
            Assert::AreEqual(int(semantic::colour), int(fd(1, 0).semantic()), L"semantic@1, 0", LINE_INFO());
            Assert::AreEqual(std::string("colour"), fd(1, 0).semantic_name(), L"semantic_name@1, 0", LINE_INFO());
            Assert::AreEqual(4 * sizeof(std::uint8_t), std::size_t(fd(1, 0).size()), L"size@1, 0", LINE_INFO());

            fd.add(user_defined_semantic(0), "hugo", format::float32, 1, 0, 0, true);
            Assert::AreEqual(std::size_t(2), fd.count(), L"No new group added", LINE_INFO());
            Assert::AreEqual(std::size_t(1), fd[0].count(), L"First group has one element", LINE_INFO());
            Assert::AreEqual(std::size_t(2), fd[1].count(), L"Second group has two elements", LINE_INFO());

            Assert::AreEqual(int(format::float32xyzw), int(fd(0, 0).format()), L"format@0, 0", LINE_INFO());
            Assert::AreEqual(int(0), int(fd(0, 0).offset()), L"offset@0, 0", LINE_INFO());
            Assert::AreEqual(int(semantic::position_radius), int(fd(0, 0).semantic()), L"semantic@0, 0", LINE_INFO());
            Assert::AreEqual(std::string("position_radius"), fd(0, 0).semantic_name(), L"semantic_name@0, 0", LINE_INFO());
            Assert::AreEqual(4 * sizeof(float), std::size_t(fd(0, 0).size()), L"size@0, 0", LINE_INFO());

            Assert::AreEqual(int(format::uint8xyzw), int(fd(1, 0).format()), L"format@1, 0", LINE_INFO());
            Assert::AreEqual(int(0), int(fd(1, 0).offset()), L"offset@1, 0", LINE_INFO());
            Assert::AreEqual(int(semantic::colour), int(fd(1, 0).semantic()), L"semantic@1, 0", LINE_INFO());
            Assert::AreEqual(std::string("colour"), fd(1, 0).semantic_name(), L"semantic_name@1, 0", LINE_INFO());
            Assert::AreEqual(4 * sizeof(std::uint8_t), std::size_t(fd(1, 0).size()), L"size@1, 0", LINE_INFO());

            Assert::AreEqual(int(format::float32), int(fd(1, 1).format()), L"format@1, 1", LINE_INFO());
            Assert::AreEqual(4 * sizeof(std::uint8_t), std::size_t(fd(1, 1).offset()), L"offset@1, 1", LINE_INFO());
            Assert::AreEqual(int(user_defined_semantic(0)), int(fd(1, 1).semantic()), L"semantic@1, 1", LINE_INFO());
            Assert::AreEqual(std::string("hugo"), fd(1, 1).semantic_name(), L"semantic_name@1, 1", LINE_INFO());
            Assert::AreEqual(sizeof(float), std::size_t(fd(1, 1).size()), L"size@1, 1", LINE_INFO());

            fd.add({ user_defined_semantic(1), "horst", format::int32, 1, 0, 0 }, true);
            Assert::AreEqual(std::size_t(2), fd.count(), L"No new group added", LINE_INFO());
            Assert::AreEqual(std::size_t(1), fd[0].count(), L"First group has one element", LINE_INFO());
            Assert::AreEqual(std::size_t(3), fd[1].count(), L"Second group has three elements", LINE_INFO());

            Assert::AreEqual(int(format::float32xyzw), int(fd(0, 0).format()), L"format@0, 0", LINE_INFO());
            Assert::AreEqual(int(0), int(fd(0, 0).offset()), L"offset@0, 0", LINE_INFO());
            Assert::AreEqual(int(semantic::position_radius), int(fd(0, 0).semantic()), L"semantic@0, 0", LINE_INFO());
            Assert::AreEqual(std::string("position_radius"), fd(0, 0).semantic_name(), L"semantic_name@0, 0", LINE_INFO());
            Assert::AreEqual(4 * sizeof(float), std::size_t(fd(0, 0).size()), L"size@0, 0", LINE_INFO());

            Assert::AreEqual(int(format::uint8xyzw), int(fd(1, 0).format()), L"format@1, 0", LINE_INFO());
            Assert::AreEqual(int(0), int(fd(1, 0).offset()), L"offset@1, 0", LINE_INFO());
            Assert::AreEqual(int(semantic::colour), int(fd(1, 0).semantic()), L"semantic@1, 0", LINE_INFO());
            Assert::AreEqual(std::string("colour"), fd(1, 0).semantic_name(), L"semantic_name@1, 0", LINE_INFO());
            Assert::AreEqual(4 * sizeof(std::uint8_t), std::size_t(fd(1, 0).size()), L"size@1, 0", LINE_INFO());

            Assert::AreEqual(int(format::float32), int(fd(1, 1).format()), L"format@1, 1", LINE_INFO());
            Assert::AreEqual(4 * sizeof(std::uint8_t), std::size_t(fd(1, 1).offset()), L"offset@1, 1", LINE_INFO());
            Assert::AreEqual(int(user_defined_semantic(0)), int(fd(1, 1).semantic()), L"semantic@1, 1", LINE_INFO());
            Assert::AreEqual(std::string("hugo"), fd(1, 1).semantic_name(), L"semantic_name@1, 1", LINE_INFO());
            Assert::AreEqual(sizeof(float), std::size_t(fd(1, 1).size()), L"size@1, 1", LINE_INFO());

            Assert::AreEqual(int(format::int32), int(fd(1, 2).format()), L"format@1, 2", LINE_INFO());
            Assert::AreEqual(4 * sizeof(std::uint8_t) + sizeof(float), std::size_t(fd(1, 2).offset()), L"offset@1, 2", LINE_INFO());
            Assert::AreEqual(int(user_defined_semantic(1)), int(fd(1, 2).semantic()), L"semantic@1, 2", LINE_INFO());
            Assert::AreEqual(std::string("horst"), fd(1, 2).semantic_name(), L"semantic_name@1, 2", LINE_INFO());
            Assert::AreEqual(sizeof(std::int32_t), std::size_t(fd(1, 2).size()), L"size@1, 2", LINE_INFO());
        }

        TEST_METHOD(initialiser) {
            format_description<wchar_t> fd {
                { semantic::position_radius, format::float32xyzw, 0, 0, 0 },
                { semantic::colour, format::uint8xyzw, 1, 0, 0 },
                { user_defined_semantic(0), L"hugo", format::float32, 1, 0, 0},
                { user_defined_semantic(1), L"horst", format::int32, 1, 0, 0 }
            };

            Assert::AreEqual(std::size_t(2), fd.count(), L"Two groups", LINE_INFO());
            Assert::AreEqual(std::size_t(1), fd[0].count(), L"First group has one element", LINE_INFO());
            Assert::AreEqual(std::size_t(3), fd[1].count(), L"Second group has three elements", LINE_INFO());

            Assert::AreEqual(int(format::float32xyzw), int(fd(0, 0).format()), L"format@0, 0", LINE_INFO());
            Assert::AreEqual(int(0), int(fd(0, 0).offset()), L"offset@0, 0", LINE_INFO());
            Assert::AreEqual(int(semantic::position_radius), int(fd(0, 0).semantic()), L"semantic@0, 0", LINE_INFO());
            Assert::AreEqual(std::wstring(L"position_radius"), fd(0, 0).semantic_name(), L"semantic_name@0, 0", LINE_INFO());
            Assert::AreEqual(4 * sizeof(float), std::size_t(fd(0, 0).size()), L"size@0, 0", LINE_INFO());

            Assert::AreEqual(int(format::uint8xyzw), int(fd(1, 0).format()), L"format@1, 0", LINE_INFO());
            Assert::AreEqual(int(0), int(fd(1, 0).offset()), L"offset@1, 0", LINE_INFO());
            Assert::AreEqual(int(semantic::colour), int(fd(1, 0).semantic()), L"semantic@1, 0", LINE_INFO());
            Assert::AreEqual(std::wstring(L"colour"), fd(1, 0).semantic_name(), L"semantic_name@1, 0", LINE_INFO());
            Assert::AreEqual(4 * sizeof(std::uint8_t), std::size_t(fd(1, 0).size()), L"size@1, 0", LINE_INFO());

            Assert::AreEqual(int(format::float32), int(fd(1, 1).format()), L"format@1, 1", LINE_INFO());
            Assert::AreEqual(std::size_t(0), std::size_t(fd(1, 1).offset()), L"offset@1, 1", LINE_INFO());
            Assert::AreEqual(int(user_defined_semantic(0)), int(fd(1, 1).semantic()), L"semantic@1, 1", LINE_INFO());
            Assert::AreEqual(std::wstring(L"hugo"), fd(1, 1).semantic_name(), L"semantic_name@1, 1", LINE_INFO());
            Assert::AreEqual(sizeof(float), std::size_t(fd(1, 1).size()), L"size@1, 1", LINE_INFO());

            Assert::AreEqual(int(format::int32), int(fd(1, 2).format()), L"format@1, 2", LINE_INFO());
            Assert::AreEqual(std::size_t(0), std::size_t(fd(1, 2).offset()), L"offset@1, 2", LINE_INFO());
            Assert::AreEqual(int(user_defined_semantic(1)), int(fd(1, 2).semantic()), L"semantic@1, 2", LINE_INFO());
            Assert::AreEqual(std::wstring(L"horst"), fd(1, 2).semantic_name(), L"semantic_name@1, 2", LINE_INFO());
            Assert::AreEqual(sizeof(std::int32_t), std::size_t(fd(1, 2).size()), L"size@1, 2", LINE_INFO());

            Assert::AreEqual(4 * sizeof(float) + 4 + sizeof(float) + sizeof(std::int32_t), fd.size(), L"Total size", LINE_INFO());
        }
    };
}
