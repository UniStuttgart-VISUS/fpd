// <copyright file="test_semantic.cpp" company="Visualisierungsinstitut der Universität Stuttgart">
// Copyright © 2023 Visualisierungsinstitut der Universität Stuttgart.
// Licensed under the MIT licence. See LICENCE file for details.
// </copyright>
// <author>Christoph Müller</author>

#include "pch.h"
#include "CppUnitTest.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;


namespace fpd {

    TEST_CLASS(test_semantic) {

    public:

        TEST_METHOD(user_defined) {
            Assert::IsTrue(std::uint64_t(semantic::user_defined) > std::uint64_t(semantic::colour), L"user_defined > colour", LINE_INFO());
            Assert::IsTrue(std::uint64_t(semantic::user_defined) > std::uint64_t(semantic::intensity), L"user_defined > intensity", LINE_INFO());
            Assert::IsTrue(std::uint64_t(semantic::user_defined) > std::uint64_t(semantic::position), L"user_defined > position", LINE_INFO());
            Assert::IsTrue(std::uint64_t(semantic::user_defined) > std::uint64_t(semantic::position_radius), L"user_defined > position_radius", LINE_INFO());
            Assert::IsTrue(std::uint64_t(semantic::user_defined) > std::uint64_t(semantic::radius), L"user_defined > radius", LINE_INFO());
            Assert::IsTrue(std::uint64_t(semantic::user_defined) > std::uint64_t(semantic::velocity), L"user_defined > velocity", LINE_INFO());
        }
    };
}
