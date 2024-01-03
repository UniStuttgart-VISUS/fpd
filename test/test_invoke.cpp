// <copyright file="test_invoke.cpp" company="Visualisierungsinstitut der Universität Stuttgart">
// Copyright © 2024 Visualisierungsinstitut der Universität Stuttgart.
// Licensed under the MIT licence. See LICENCE file for details.
// </copyright>
// <author>Christoph Müller</author>

#include "pch.h"


namespace fpd {

    TEST_CLASS(test_invoke) {

    public:

        struct inc_container final {
            inline int inc_ret(const int i) { return i + 1; }
            inline void inc_sfx(int& i) { ++i; }
        };

        static inline void static_inc(int& i) { ++i; }

        TEST_METHOD(function) {
            Assert::AreEqual(42.0f, FPD_NAMESPACE::invoke(std::floorf, 42.1f), L"std::floorf", LINE_INFO());

            {
                int i = 41;
                FPD_NAMESPACE::invoke(test_invoke::static_inc, i);
                Assert::AreEqual(42, i, L"void-returning function", LINE_INFO());
            }
        }

        TEST_METHOD(lambda) {
            {
                int i = 41;
                Assert::AreEqual(42, FPD_NAMESPACE::invoke([&i](void) { return ++i; }), L"lambda w/ return", LINE_INFO());
            }

            {
                int i = 41;
                FPD_NAMESPACE::invoke([&i](void) { ++i; });
                Assert::AreEqual(42, i, L"lambda w/o return", LINE_INFO());
            }
        }

        TEST_METHOD(functor) {
            Assert::IsTrue(FPD_NAMESPACE::invoke(std::less<int>(), 17, 42), L"std::less", LINE_INFO());
        }

        TEST_METHOD(pointer2member) {
            inc_container c;

            Assert::AreEqual(42, FPD_NAMESPACE::invoke(&inc_container::inc_ret, &c, 41), L"instance from pointer", LINE_INFO());
            Assert::AreEqual(42, FPD_NAMESPACE::invoke(&inc_container::inc_ret, c, 41), L"instance from reference", LINE_INFO());
            Assert::AreEqual(42, FPD_NAMESPACE::invoke(&inc_container::inc_ret, std::ref(c), 41), L"instance from reference_wrapper", LINE_INFO());

            {
                int i = 41;
                invoke(&inc_container::inc_sfx, &c, i);
                Assert::AreEqual(42, i, L"instance from pointer", LINE_INFO());
            }

            {
                int i = 41;
                invoke(&inc_container::inc_sfx, c, i);
                Assert::AreEqual(42, i, L"instance from reference", LINE_INFO());
            }

            {
                int i = 41;
                invoke(&inc_container::inc_sfx, std::ref(c), i);
                Assert::AreEqual(42, i, L"instance from reference_wrapper", LINE_INFO());
            }
        }
    };
}
