// <copyright file="test_meta_data.cpp" company="Visualisierungsinstitut der Universität Stuttgart">
// Copyright © 2023 Visualisierungsinstitut der Universität Stuttgart.
// Licensed under the MIT licence. See LICENCE file for details.
// </copyright>
// <author>Christoph Müller</author>

#include "pch.h"


namespace fpd {

    TEST_CLASS(test_bounding_box) {

    public:

        TEST_METHOD(default_ctor) {
            bounding_box bb;
            Assert::AreEqual(int(format::unknown), int(bb.format()), L"format", LINE_INFO());
            Assert::IsFalse(bool(bb), L"not valid", LINE_INFO());
            Assert::IsNull(bb.data(), L"data", LINE_INFO());
            Assert::IsTrue(bb.empty(), L"empty", LINE_INFO());
            Assert::IsNull(bb[bounding_box::value::max_x], L"max_x", LINE_INFO());
            Assert::IsNull(bb[bounding_box::value::max_y], L"max_y", LINE_INFO());
            Assert::IsNull(bb[bounding_box::value::max_z], L"max_z", LINE_INFO());
            Assert::IsNull(bb[bounding_box::value::min_x], L"min_x", LINE_INFO());
            Assert::IsNull(bb[bounding_box::value::min_y], L"min_y", LINE_INFO());
            Assert::IsNull(bb[bounding_box::value::min_z], L"min_z", LINE_INFO());
            Assert::ExpectException<std::logic_error>([&bb](void) { bb.get<float>(bounding_box::value::max_x); }, L"get max_x", LINE_INFO());
            Assert::ExpectException<std::logic_error>([&bb](void) { bb.get<float>(bounding_box::value::max_y); }, L"get max_y", LINE_INFO());
            Assert::ExpectException<std::logic_error>([&bb](void) { bb.get<float>(bounding_box::value::max_z); }, L"get max_z", LINE_INFO());
            Assert::ExpectException<std::logic_error>([&bb](void) { bb.get<float>(bounding_box::value::min_x); }, L"get min_x", LINE_INFO());
            Assert::ExpectException<std::logic_error>([&bb](void) { bb.get<float>(bounding_box::value::min_y); }, L"get min_y", LINE_INFO());
            Assert::ExpectException<std::logic_error>([&bb](void) { bb.get<float>(bounding_box::value::min_y); }, L"get min_y", LINE_INFO());
        }

        TEST_METHOD(format_ctor) {
            bounding_box bb(format::float32);
            Assert::AreEqual(int(format::float32), int(bb.format()), L"format", LINE_INFO());
            Assert::IsTrue(bool(bb), L"valid", LINE_INFO());
            Assert::IsNotNull(bb.data(), L"data", LINE_INFO());
            Assert::IsTrue(bb.empty(), L"empty", LINE_INFO());
            Assert::IsNotNull(bb[bounding_box::value::max_x], L"max_x", LINE_INFO());
            Assert::IsNotNull(bb[bounding_box::value::max_y], L"max_y", LINE_INFO());
            Assert::IsNotNull(bb[bounding_box::value::max_z], L"max_z", LINE_INFO());
            Assert::IsNotNull(bb[bounding_box::value::min_x], L"min_x", LINE_INFO());
            Assert::IsNotNull(bb[bounding_box::value::min_y], L"min_y", LINE_INFO());
            Assert::IsNotNull(bb[bounding_box::value::min_z], L"min_z", LINE_INFO());
            Assert::AreEqual(0.0f, bb.get<float>(bounding_box::value::max_x), L"get max_x", LINE_INFO());
            Assert::AreEqual(0.0f, bb.get<float>(bounding_box::value::max_y), L"get max_y", LINE_INFO());
            Assert::AreEqual(0.0f, bb.get<float>(bounding_box::value::max_z), L"get max_z", LINE_INFO());
            Assert::AreEqual(0.0f, bb.get<float>(bounding_box::value::min_x), L"get min_x", LINE_INFO());
            Assert::AreEqual(0.0f, bb.get<float>(bounding_box::value::min_y), L"get min_y", LINE_INFO());
            Assert::AreEqual(0.0f, bb.get<float>(bounding_box::value::min_z), L"get min_y", LINE_INFO());
            Assert::ExpectException<std::logic_error>([&bb](void) { bb.get<int>(bounding_box::value::max_x); }, L"get max_x wrong type", LINE_INFO());
            Assert::ExpectException<std::logic_error>([&bb](void) { bb.get<int>(bounding_box::value::max_y); }, L"get max_y wrong type", LINE_INFO());
            Assert::ExpectException<std::logic_error>([&bb](void) { bb.get<int>(bounding_box::value::max_z); }, L"get max_z wrong type", LINE_INFO());
            Assert::ExpectException<std::logic_error>([&bb](void) { bb.get<int>(bounding_box::value::min_x); }, L"get min_x wrong type", LINE_INFO());
            Assert::ExpectException<std::logic_error>([&bb](void) { bb.get<int>(bounding_box::value::min_y); }, L"get min_y wrong type", LINE_INFO());
            Assert::ExpectException<std::logic_error>([&bb](void) { bb.get<int>(bounding_box::value::min_z); }, L"get min_y wrong type", LINE_INFO());
        }

        TEST_METHOD(component_ctor) {
            bounding_box bb(1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f);
            Assert::AreEqual(int(format::float32), int(bb.format()), L"format", LINE_INFO());
            Assert::IsTrue(bool(bb), L"valid", LINE_INFO());
            Assert::IsNotNull(bb.data(), L"data", LINE_INFO());
            Assert::IsFalse(bb.empty(), L"empty", LINE_INFO());
            Assert::IsNotNull(bb[bounding_box::value::max_x], L"max_x", LINE_INFO());
            Assert::IsNotNull(bb[bounding_box::value::max_y], L"max_y", LINE_INFO());
            Assert::IsNotNull(bb[bounding_box::value::max_z], L"max_z", LINE_INFO());
            Assert::IsNotNull(bb[bounding_box::value::min_x], L"min_x", LINE_INFO());
            Assert::IsNotNull(bb[bounding_box::value::min_y], L"min_y", LINE_INFO());
            Assert::IsNotNull(bb[bounding_box::value::min_z], L"min_z", LINE_INFO());
            Assert::AreEqual(4.0f, bb.get<float>(bounding_box::value::max_x), L"get max_x", LINE_INFO());
            Assert::AreEqual(5.0f, bb.get<float>(bounding_box::value::max_y), L"get max_y", LINE_INFO());
            Assert::AreEqual(6.0f, bb.get<float>(bounding_box::value::max_z), L"get max_z", LINE_INFO());
            Assert::AreEqual(1.0f, bb.get<float>(bounding_box::value::min_x), L"get min_x", LINE_INFO());
            Assert::AreEqual(2.0f, bb.get<float>(bounding_box::value::min_y), L"get min_y", LINE_INFO());
            Assert::AreEqual(3.0f, bb.get<float>(bounding_box::value::min_z), L"get min_y", LINE_INFO());
            Assert::ExpectException<std::logic_error>([&bb](void) { bb.get<int>(bounding_box::value::max_x); }, L"get max_x wrong type", LINE_INFO());
            Assert::ExpectException<std::logic_error>([&bb](void) { bb.get<int>(bounding_box::value::max_y); }, L"get max_y wrong type", LINE_INFO());
            Assert::ExpectException<std::logic_error>([&bb](void) { bb.get<int>(bounding_box::value::max_z); }, L"get max_z wrong type", LINE_INFO());
            Assert::ExpectException<std::logic_error>([&bb](void) { bb.get<int>(bounding_box::value::min_x); }, L"get min_x wrong type", LINE_INFO());
            Assert::ExpectException<std::logic_error>([&bb](void) { bb.get<int>(bounding_box::value::min_y); }, L"get min_y wrong type", LINE_INFO());
            Assert::ExpectException<std::logic_error>([&bb](void) { bb.get<int>(bounding_box::value::min_z); }, L"get min_y wrong type", LINE_INFO());
        }

        TEST_METHOD(minmax) {
            bounding_box bb(1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f);
            Assert::AreEqual(4.0f, (bb.max<float>)(0), L"max(0)", LINE_INFO());
            Assert::AreEqual(5.0f, (bb.max<float>)(1), L"max(1)", LINE_INFO());
            Assert::AreEqual(6.0f, (bb.max<float>)(2), L"max(2)", LINE_INFO());
            Assert::ExpectException<std::out_of_range>([&bb](void) { (bb.max<float>)(3); }, L"max(3)", LINE_INFO());
            Assert::AreEqual(1.0f, (bb.min<float>)(0), L"min(0)", LINE_INFO());
            Assert::AreEqual(2.0f, (bb.min<float>)(1), L"min(1)", LINE_INFO());
            Assert::AreEqual(3.0f, (bb.min<float>)(2), L"min(2)", LINE_INFO());
            Assert::ExpectException<std::out_of_range>([&bb](void) { (bb.min<float>)(3); }, L"min(3)", LINE_INFO());

            Assert::AreEqual(4.0f, (bb.max<float>)(0), L"max(0)", LINE_INFO());
            (bb.max)(0, 42.0f);
            Assert::AreEqual(42.0f, (bb.max<float>)(0), L"max(0)", LINE_INFO());
            (bb.min)(0, 42);
            Assert::AreEqual(42.0f, (bb.min<float>)(0), L"min(0)", LINE_INFO());
        }
    };
}
