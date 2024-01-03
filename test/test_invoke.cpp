// <copyright file="test_invoke.cpp" company="Visualisierungsinstitut der Universität Stuttgart">
// Copyright © 2024 Visualisierungsinstitut der Universität Stuttgart.
// Licensed under the MIT licence. See LICENCE file for details.
// </copyright>
// <author>Christoph Müller</author>

#include "pch.h"


namespace fpd {

    TEST_CLASS(test_invoke) {

    public:

        static inline void inc(int& i) { ++i; }

        TEST_METHOD(function) {
            Assert::AreEqual(42.0f, invoke(std::floorf, 42.1f), L"std::floorf", LINE_INFO());

            {
                int i = 41;
                invoke(test_invoke::inc, i);
                Assert::AreEqual(42, i, L"void-returning function", LINE_INFO());
            }
        }

        TEST_METHOD(lambda) {
            {
                int i = 41;
                Assert::AreEqual(42, invoke([&i](void) { return ++i; }), L"lambda w/ return", LINE_INFO());
            }

            {
                int i = 41;
                invoke([&i](void) { ++i; });
                Assert::AreEqual(42, i, L"lambda w/o return", LINE_INFO());
            }
        }

        TEST_METHOD(functor) {
            Assert::IsTrue(invoke(std::less<int>(), 17, 42), L"std::less", LINE_INFO());
        }
    };
}
