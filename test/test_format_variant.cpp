// <copyright file="test_format_variant.cpp" company="Visualisierungsinstitut der Universität Stuttgart">
// Copyright © 2023 Visualisierungsinstitut der Universität Stuttgart.
// Licensed under the MIT licence. See LICENCE file for details.
// </copyright>
// <author>Christoph Müller</author>

#include "pch.h"


namespace fpd {

    TEST_CLASS(test_format_variant) {

    public:

        TEST_METHOD(ctor) {
            {
                format_variant v;
                Assert::IsFalse(bool(v), L"initialised invalid", LINE_INFO());
                Assert::AreEqual(int(v.format()), int(format::unknown), L"unknown", LINE_INFO());
            }
            {
                format_variant v(std::int32_t(12));
                Assert::IsTrue(bool(v), L"initialised valid", LINE_INFO());
                Assert::AreEqual(int(v.format()), int(format::int32), L"int32", LINE_INFO());
            }

        }

        TEST_METHOD(size) {
            {
                basic_format_variant<format_list<>> v;
                Assert::AreEqual(std::size_t(0), v.size(), L"empty", LINE_INFO());
            }

            {
                basic_format_variant<format_list<format::int32>> v;
                Assert::AreEqual(sizeof(std::int32_t), v.size(), L"int", LINE_INFO());
            }

            {
                basic_format_variant<format_list<format::int32, format::float32>> v;
                Assert::AreEqual(sizeof(std::int32_t), v.size(), L"int, float", LINE_INFO());
            }

            {
                basic_format_variant<format_list<format::int32, format::float64>> v;
                Assert::AreEqual(sizeof(double), v.size(), L"int, float, double", LINE_INFO());
            }

            {
                // Note: this might fail in regression tests if a new format was added. Adjust accordingly.
                format_variant v;
                Assert::AreEqual(4 * sizeof(double), v.size(), L"format_variant", LINE_INFO());
            }
        }

        TEST_METHOD(supports) {
            {
                basic_format_variant<format_list<>> v;
                Assert::IsFalse(v.supports(format::float32), L"float32", LINE_INFO());
                Assert::IsFalse(v.supports<format::float32>(), L"float32 comile time", LINE_INFO());
                Assert::IsFalse(v.supports<float>(), L"float", LINE_INFO());
            }

            {
                basic_format_variant<format_list<format::int32, format::float64>> v;

                Assert::IsFalse(v.supports(format::float32), L"float32", LINE_INFO());
                Assert::IsFalse(v.supports<format::float32>(), L"float32 comile time", LINE_INFO());
                Assert::IsFalse(v.supports<float>(), L"float", LINE_INFO());

                Assert::IsTrue(v.supports(format::int32), L"int32", LINE_INFO());
                Assert::IsTrue(v.supports<format::int32>(), L"int32 comile time", LINE_INFO());
                Assert::IsTrue(v.supports<int>(), L"int", LINE_INFO());
            }
        }

        TEST_METHOD(value_ctor) {
            {
                basic_format_variant<format_list<format::int32, format::float64>> v(42);
                Assert::AreEqual(int(format::int32), int(v.format()), L"Format derived from argument", LINE_INFO());
                Assert::IsNotNull(v.try_get<int>(), L"Get via type valid", LINE_INFO());
                Assert::IsNull(v.try_get<double>(), L"Get via wrong type invalid", LINE_INFO());
                Assert::IsNotNull(v.try_get<format::int32>(), L"Get via format valid", LINE_INFO());
                Assert::IsNull(v.try_get<format::float64>(), L"Get via wrong format invalid", LINE_INFO());
                Assert::AreEqual(42, v.get<format::int32>(), L"Get via format", LINE_INFO());
                Assert::AreEqual(42, v.get<int>(), L"Get via type", LINE_INFO());
            }
        }

        TEST_METHOD(runtime_ctor) {
            Assert::ExpectException<std::invalid_argument>([](void) {
                basic_format_variant<format_list<format::int32>> v(format::int32, nullptr, sizeof(int));
            }, L"Cannot initialise from nullptr", LINE_INFO());

            Assert::ExpectException<std::invalid_argument>([](void) {
                int i = 42;
                basic_format_variant<format_list<format::int32>> v(format::int32, &i, 0);
            }, L"Cannot initialise from insufficient data", LINE_INFO());

            {
                int i = 42;
                basic_format_variant<format_list<format::int32, format::float64>> v(format::int32, &i, sizeof(i));
                Assert::AreEqual(int(format::int32), int(v.format()), L"Format derived from argument", LINE_INFO());
                Assert::IsNotNull(v.try_get<int>(), L"Get via type valid", LINE_INFO());
                Assert::IsNull(v.try_get<double>(), L"Get via wrong type invalid", LINE_INFO());
                Assert::IsNotNull(v.try_get<format::int32>(), L"Get via format valid", LINE_INFO());
                Assert::IsNull(v.try_get<format::float64>(), L"Get via wrong format invalid", LINE_INFO());
                Assert::AreEqual(42, v.get<format::int32>(), L"Get via format", LINE_INFO());
                Assert::AreEqual(42, v.get<int>(), L"Get via type", LINE_INFO());
            }

            {
                double d = 42;
                basic_format_variant<format_list<format::int32, format::float64>> v(format::float64, &d, sizeof(d));
                Assert::AreEqual(int(format::float64), int(v.format()), L"Format derived from argument", LINE_INFO());
                Assert::IsNotNull(v.try_get<double>(), L"Get via type valid", LINE_INFO());
                Assert::IsNull(v.try_get<int>(), L"Get via wrong type invalid", LINE_INFO());
                Assert::IsNotNull(v.try_get<format::float64>(), L"Get via format valid", LINE_INFO());
                Assert::IsNull(v.try_get<format::int32>(), L"Get via wrong format invalid", LINE_INFO());
                Assert::AreEqual(42.0, v.get<format::float64>(), L"Get via format", LINE_INFO());
                Assert::AreEqual(42.0, v.get<double>(), L"Get via type", LINE_INFO());
            }
        }

        TEST_METHOD(copy_ctor) {
            basic_format_variant<format_list<format::int32, format::float64>> v1(42.0);
            Assert::IsTrue(bool(v1), L"Initialised valid", LINE_INFO());
            Assert::AreEqual(42.0, v1.get<double>(), L"Get via type", LINE_INFO());

            basic_format_variant<format_list<format::int32, format::float64>> v2(v1);
            Assert::IsTrue(bool(v2), L"Initialised valid", LINE_INFO());
            Assert::AreEqual(42.0, v2.get<double>(), L"Get via type", LINE_INFO());
            Assert::IsTrue(bool(v1), L"Source still valid", LINE_INFO());
            Assert::AreEqual(42.0, v1.get<double>(), L"Source unchanged", LINE_INFO());

            basic_format_variant<format_list<format::float64>> v3(v1);
            Assert::IsTrue(bool(v3), L"Initialised valid", LINE_INFO());
            Assert::AreEqual(42.0, v3.get<double>(), L"Get via type", LINE_INFO());
            Assert::IsTrue(bool(v1), L"Source still valid", LINE_INFO());
            Assert::AreEqual(42.0, v1.get<double>(), L"Source unchanged", LINE_INFO());

            const basic_format_variant<format_list<format::float64>> empty;
            basic_format_variant<format_list<format::int32>> v4(empty);
            Assert::IsFalse(bool(v4), L"Initialised empty", LINE_INFO());

            Assert::IsTrue(bool(v2), L"Valid value in object to be cloned", LINE_INFO());
            Assert::ExpectException<std::invalid_argument>([&v2](void) {
                basic_format_variant<format_list<format::float32>> v(v2);
            }, L"Initialise from unsupported", LINE_INFO());
        }

        TEST_METHOD(move_ctor) {
            basic_format_variant<format_list<format::int32, format::float64>> v1(42.0);
            Assert::IsTrue(bool(v1), L"Initialised valid", LINE_INFO());
            Assert::AreEqual(42.0, v1.get<double>(), L"Get via type", LINE_INFO());

            basic_format_variant<format_list<format::int32, format::float64>> v2(std::move(v1));
            Assert::IsTrue(bool(v2), L"Initialised valid", LINE_INFO());
            Assert::AreEqual(42.0, v2.get<double>(), L"Get via type", LINE_INFO());
            Assert::IsFalse(bool(v1), L"Source now invalid", LINE_INFO());
        }

        TEST_METHOD(assignment) {
            basic_format_variant<format_list<format::int32, format::float64>> v1(42.0);
            Assert::IsTrue(bool(v1), L"Initialised valid", LINE_INFO());
            Assert::AreEqual(42.0, v1.get<double>(), L"Get via type", LINE_INFO());

            basic_format_variant<format_list<format::int32, format::float64>> v2(17);
            Assert::IsTrue(bool(v2), L"Initialised valid", LINE_INFO());
            Assert::AreEqual(17, v2.get<int>(), L"Get via type", LINE_INFO());
            v2 = v1;
            Assert::IsTrue(bool(v2), L"Target still valid", LINE_INFO());
            Assert::AreEqual(42.0, v1.get<double>(), L"Target assigned", LINE_INFO());
            Assert::IsTrue(bool(v1), L"Source still valid", LINE_INFO());
            Assert::AreEqual(42.0, v1.get<double>(), L"Source unchanged", LINE_INFO());

            basic_format_variant<format_list<format::float64>> v3(43.0);
            Assert::IsTrue(bool(v3), L"Initialised valid", LINE_INFO());
            Assert::AreEqual(43.0, v3.get<double>(), L"Get via type", LINE_INFO());
            v3 = v1;
            Assert::IsTrue(bool(v2), L"Target still valid", LINE_INFO());
            Assert::AreEqual(42.0, v1.get<double>(), L"Target assigned", LINE_INFO());
            Assert::IsTrue(bool(v1), L"Source still valid", LINE_INFO());
            Assert::AreEqual(42.0, v1.get<double>(), L"Source unchanged", LINE_INFO());

            const basic_format_variant<format_list<format::float64>> empty;
            basic_format_variant<format_list<format::int32>> v4(17);
            Assert::IsFalse(bool(empty), L"Initialised empty", LINE_INFO());
            v4 = empty;
            Assert::IsFalse(bool(v4), L"Assigned empty", LINE_INFO());

            Assert::ExpectException<std::invalid_argument>([&v2](void) {
                basic_format_variant<format_list<format::float32>> v(17.0f);
                Assert::IsTrue(bool(v), L"Initialised valid", LINE_INFO());
                Assert::AreEqual(17.0f, v.get<float>(), L"Get via type", LINE_INFO());
                v2 = v;
            }, L"Assign unsupported", LINE_INFO());
        }

        TEST_METHOD(move_assignment) {
            basic_format_variant<format_list<format::int32, format::float64>> v1(42.0);
            Assert::IsTrue(bool(v1), L"Initialised valid", LINE_INFO());
            Assert::AreEqual(42.0, v1.get<double>(), L"Get via type", LINE_INFO());

            basic_format_variant<format_list<format::int32, format::float64>> v2(17);
            Assert::IsTrue(bool(v2), L"Initialised valid", LINE_INFO());
            Assert::AreEqual(17, v2.get<int>(), L"Get via type", LINE_INFO());
            v2 = std::move(v1);
            Assert::AreEqual(42.0, v2.get<double>(), L"Get via type", LINE_INFO());
            Assert::IsFalse(bool(v1), L"Source now invalid", LINE_INFO());

            v2 = v2;
            Assert::IsTrue(bool(v2), L"Assignment on self valud", LINE_INFO());
            Assert::AreEqual(42.0, v2.get<double>(), L"Get via type", LINE_INFO());
        }

        TEST_METHOD(value_assignment) {
            basic_format_variant<format_list<format::int32, format::float64>> v(42.0);
            Assert::IsTrue(bool(v), L"Initialised valid", LINE_INFO());
            Assert::AreEqual(int(format::float64), int(v.format()), L"format", LINE_INFO());
            Assert::AreEqual(42.0, v.get<double>(), L"Get via type", LINE_INFO());

            v = 42;
            Assert::IsTrue(bool(v), L"Initialised valid", LINE_INFO());
            Assert::AreEqual(int(format::int32), int(v.format()), L"format", LINE_INFO());
            Assert::AreEqual(42, v.get<int>(), L"Get via type", LINE_INFO());
        }
    };
}
