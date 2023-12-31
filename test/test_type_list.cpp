// <copyright file="test_type_list.cpp" company="Visualisierungsinstitut der Universität Stuttgart">
// Copyright © 2023 Visualisierungsinstitut der Universität Stuttgart.
// Licensed under the MIT licence. See LICENCE file for details.
// </copyright>
// <author>Christoph Müller</author>

#include "pch.h"


namespace fpd {

    TEST_CLASS(test_type_list) {

    public:

        TEST_METHOD(cat) {
            {
                typedef type_list<> old_list;
                typedef int element;
                typedef type_list_cat<old_list, element> new_list;
                Assert::IsTrue(std::is_same<type_list<element>, new_list>::value, L"cat to empty", LINE_INFO());
            }

            {
                typedef type_list<float> old_list;
                typedef int element;
                typedef type_list_cat<old_list, element> new_list;
                Assert::IsTrue(std::is_same<type_list<float, element>, new_list>::value, L"cat to single", LINE_INFO());
            }

            {
                typedef type_list<float, double> old_list;
                typedef int element;
                typedef type_list_cat<old_list, element> new_list;
                Assert::IsTrue(std::is_same<type_list<float, double, element>, new_list>::value, L"cat to list", LINE_INFO());
            }
        }

        TEST_METHOD(size) {
            Assert::AreEqual(std::size_t(0), type_list_size<type_list<>>::value, L"empty", LINE_INFO());
            Assert::AreEqual(std::size_t(1), type_list_size<type_list<int>>::value, L"one", LINE_INFO());
            Assert::AreEqual(std::size_t(2), type_list_size<type_list<int, float>>::value, L"two", LINE_INFO());
            Assert::AreEqual(std::size_t(3), type_list_size<type_list<int, float, double>>::value, L"three", LINE_INFO());
        }

        TEST_METHOD(size_of) {
            Assert::AreEqual(std::size_t(0), type_list_sizeof<type_list<>>::value, L"empty", LINE_INFO());
            Assert::AreEqual(sizeof(int), type_list_sizeof<type_list<int>>::value, L"int", LINE_INFO());
            Assert::AreEqual(sizeof(int) + sizeof(float), type_list_sizeof<type_list<int, float>>::value, L"int, float", LINE_INFO());
            Assert::AreEqual(sizeof(int) + sizeof(float) + sizeof(double), type_list_sizeof<type_list<int, float, double>>::value, L"int, float, double", LINE_INFO());
        }

        TEST_METHOD(max_sizeof) {
            Assert::AreEqual(std::size_t(0), type_list_max_sizeof<type_list<>>::value, L"empty", LINE_INFO());
            Assert::AreEqual(sizeof(int), type_list_max_sizeof<type_list<int>>::value, L"int", LINE_INFO());
            Assert::AreEqual(sizeof(int), type_list_max_sizeof<type_list<int, float>>::value, L"int, float", LINE_INFO());
            Assert::AreEqual(sizeof(double), type_list_max_sizeof<type_list<int, float, double>>::value, L"int, float, double", LINE_INFO());
        }
    };
}
