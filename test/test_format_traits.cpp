// <copyright file="test_format_traits.cpp" company="Visualisierungsinstitut der Universität Stuttgart">
// Copyright © 2023 - 2025 Visualisierungsinstitut der Universität Stuttgart.
// Licensed under the MIT licence. See LICENCE file for details.
// </copyright>
// <author>Christoph Müller</author>

#include "pch.h"


namespace fpd {

    TEST_CLASS(test_format_traits) {

    public:

        TEST_METHOD(test_forward_name) {
            Assert::AreEqual("int8x", detail::format_forward_traits<format::int8x>::name<char>(), L"int8x (char)", LINE_INFO());
            Assert::AreEqual(L"int8x", detail::format_forward_traits<format::int8x>::name<wchar_t>(), L"int8x (wchar_t)", LINE_INFO());
            Assert::AreEqual("uint8x", detail::format_forward_traits<format::uint8x>::name<char>(), L"uint8x (char)", LINE_INFO());
            Assert::AreEqual(L"uint8x", detail::format_forward_traits<format::uint8x>::name<wchar_t>(), L"uint8x (wchar_t)", LINE_INFO());
            Assert::AreEqual("int8xy", detail::format_forward_traits<format::int8xy>::name<char>(), L"int8xy (char)", LINE_INFO());
            Assert::AreEqual(L"int8xy", detail::format_forward_traits<format::int8xy>::name<wchar_t>(), L"int8xy (wchar_t)", LINE_INFO());
            Assert::AreEqual("uint8xy", detail::format_forward_traits<format::uint8xy>::name<char>(), L"uint8xy (char)", LINE_INFO());
            Assert::AreEqual(L"uint8xy", detail::format_forward_traits<format::uint8xy>::name<wchar_t>(), L"uint8xy (wchar_t)", LINE_INFO());
            Assert::AreEqual("int8xyz", detail::format_forward_traits<format::int8xyz>::name<char>(), L"int8xyz (char)", LINE_INFO());
            Assert::AreEqual(L"int8xyz", detail::format_forward_traits<format::int8xyz>::name<wchar_t>(), L"int8xyz (wchar_t)", LINE_INFO());
            Assert::AreEqual("uint8xyz", detail::format_forward_traits<format::uint8xyz>::name<char>(), L"uint8xyz (char)", LINE_INFO());
            Assert::AreEqual(L"uint8xyz", detail::format_forward_traits<format::uint8xyz>::name<wchar_t>(), L"uint8xyz (wchar_t)", LINE_INFO());
            Assert::AreEqual("int8xyzw", detail::format_forward_traits<format::int8xyzw>::name<char>(), L"int8xyzw (char)", LINE_INFO());
            Assert::AreEqual(L"int8xyzw", detail::format_forward_traits<format::int8xyzw>::name<wchar_t>(), L"int8xyzw (wchar_t)", LINE_INFO());
            Assert::AreEqual("uint8xyzw", detail::format_forward_traits<format::uint8xyzw>::name<char>(), L"uint8xyzw (char)", LINE_INFO());
            Assert::AreEqual(L"uint8xyzw", detail::format_forward_traits<format::uint8xyzw>::name<wchar_t>(), L"uint8xyzw (wchar_t)", LINE_INFO());
        }

        TEST_METHOD(test_forward_type) {
            Assert::IsTrue(std::is_same_v<std::int8_t, typename detail::format_forward_traits<format::int8x>::type>, L"int8x", LINE_INFO());
            Assert::IsTrue(std::is_same_v<std::int8_t, detail::format_t<format::int8x>>, L"int8x", LINE_INFO());

            Assert::IsTrue(std::is_same_v<std::uint8_t, typename detail::format_forward_traits<format::uint8x>::type>, L"uint8x", LINE_INFO());
            Assert::IsTrue(std::is_same_v<std::uint8_t, detail::format_t<format::uint8x>>, L"uint8x", LINE_INFO());

            Assert::IsTrue(std::is_same_v<std::array<std::int8_t, 2>, typename detail::format_forward_traits<format::int8xy>::type>, L"int8xy", LINE_INFO());
            Assert::IsTrue(std::is_same_v<std::array<std::int8_t, 2>, detail::format_t<format::int8xy>>, L"int8xy", LINE_INFO());

            Assert::IsTrue(std::is_same_v<std::array<std::uint8_t, 2>, typename detail::format_forward_traits<format::uint8xy>::type>, L"uint8xy", LINE_INFO());
            Assert::IsTrue(std::is_same_v<std::array<std::uint8_t, 2>, detail::format_t<format::uint8xy>>, L"uint8xy", LINE_INFO());

            Assert::IsTrue(std::is_same_v<std::array<std::int8_t, 3>, typename detail::format_forward_traits<format::int8xyz>::type>, L"int8xyz", LINE_INFO());
            Assert::IsTrue(std::is_same_v<std::array<std::int8_t, 3>, detail::format_t<format::int8xyz>>, L"int8xyz", LINE_INFO());

            Assert::IsTrue(std::is_same_v<std::array<std::uint8_t, 3>, typename detail::format_forward_traits<format::uint8xyz>::type>, L"uint8xyz", LINE_INFO());
            Assert::IsTrue(std::is_same_v<std::array<std::uint8_t, 3>, detail::format_t<format::uint8xyz>>, L"uint8xyz", LINE_INFO());

            Assert::IsTrue(std::is_same_v<std::array<std::int8_t, 4>, typename detail::format_forward_traits<format::int8xyzw>::type>, L"int8xyzw", LINE_INFO());
            Assert::IsTrue(std::is_same_v<std::array<std::int8_t, 4>, detail::format_t<format::int8xyzw>>, L"int8xyzw", LINE_INFO());

            Assert::IsTrue(std::is_same_v<std::array<std::uint8_t, 4>, typename detail::format_forward_traits<format::uint8xyzw>::type>, L"uint8xyzw", LINE_INFO());
            Assert::IsTrue(std::is_same_v<std::array<std::uint8_t, 4>, detail::format_t<format::uint8xyzw>>, L"uint8xyzw", LINE_INFO());
        }

        TEST_METHOD(test_reverse_name) {
            Assert::AreEqual("int8x", detail::format_forward_traits<format::int8x>::name<char>(), L"int8x (char)", LINE_INFO());
            Assert::AreEqual(L"int8x", detail::format_forward_traits<format::int8x>::name<wchar_t>(), L"int8x (wchar_t)", LINE_INFO());
            Assert::AreEqual("uint8x", detail::format_forward_traits<format::uint8x>::name<char>(), L"uint8x (char)", LINE_INFO());
            Assert::AreEqual(L"uint8x", detail::format_forward_traits<format::uint8x>::name<wchar_t>(), L"uint8x (wchar_t)", LINE_INFO());
            Assert::AreEqual("int8xy", detail::format_forward_traits<format::int8xy>::name<char>(), L"int8xy (char)", LINE_INFO());
            Assert::AreEqual(L"int8xy", detail::format_forward_traits<format::int8xy>::name<wchar_t>(), L"int8xy (wchar_t)", LINE_INFO());
            Assert::AreEqual("uint8xy", detail::format_forward_traits<format::uint8xy>::name<char>(), L"uint8xy (char)", LINE_INFO());
            Assert::AreEqual(L"uint8xy", detail::format_forward_traits<format::uint8xy>::name<wchar_t>(), L"uint8xy (wchar_t)", LINE_INFO());
            Assert::AreEqual("int8xyz", detail::format_forward_traits<format::int8xyz>::name<char>(), L"int8xyz (char)", LINE_INFO());
            Assert::AreEqual(L"int8xyz", detail::format_forward_traits<format::int8xyz>::name<wchar_t>(), L"int8xyz (wchar_t)", LINE_INFO());
            Assert::AreEqual("uint8xyz", detail::format_forward_traits<format::uint8xyz>::name<char>(), L"uint8xyz (char)", LINE_INFO());
            Assert::AreEqual(L"uint8xyz", detail::format_forward_traits<format::uint8xyz>::name<wchar_t>(), L"uint8xyz (wchar_t)", LINE_INFO());
            Assert::AreEqual("int8xyzw", detail::format_forward_traits<format::int8xyzw>::name<char>(), L"int8xyzw (char)", LINE_INFO());
            Assert::AreEqual(L"int8xyzw", detail::format_forward_traits<format::int8xyzw>::name<wchar_t>(), L"int8xyzw (wchar_t)", LINE_INFO());
            Assert::AreEqual("uint8xyzw", detail::format_forward_traits<format::uint8xyzw>::name<char>(), L"uint8xyzw (char)", LINE_INFO());
            Assert::AreEqual(L"uint8xyzw", detail::format_forward_traits<format::uint8xyzw>::name<wchar_t>(), L"uint8xyzw (wchar_t)", LINE_INFO());
        }

        TEST_METHOD(test_reverse_format) {
            Assert::AreEqual(int(format::int8x), int(detail::format_reverse_traits<std::int8_t>::format), L"int8x", LINE_INFO());
            Assert::AreEqual(int(format::int8x), int(detail::format_v<std::int8_t>), L"int8x", LINE_INFO());

            Assert::AreEqual(int(format::uint8x), int(detail::format_reverse_traits<std::uint8_t>::format), L"uint8x", LINE_INFO());
            Assert::AreEqual(int(format::uint8x), int(detail::format_v<std::uint8_t>), L"uint8x", LINE_INFO());

            Assert::AreEqual(int(format::int8xy), int(detail::format_reverse_traits<std::array<std::int8_t, 2>>::format), L"int8xy", LINE_INFO());
            Assert::AreEqual(int(format::int8xy), int(detail::format_v<std::array<std::int8_t, 2>>), L"int8xy", LINE_INFO());

            Assert::AreEqual(int(format::uint8xy), int(detail::format_reverse_traits<std::array<std::uint8_t, 2>>::format), L"uint8xy", LINE_INFO());
            Assert::AreEqual(int(format::uint8xy), int(detail::format_v<std::array<std::uint8_t, 2>>), L"uint8xy", LINE_INFO());

            Assert::AreEqual(int(format::int8xyz), int(detail::format_reverse_traits<std::array<std::int8_t, 3>>::format), L"int8xyz", LINE_INFO());
            Assert::AreEqual(int(format::int8xyz), int(detail::format_v<std::array<std::int8_t, 3>>), L"int8xyz", LINE_INFO());

            Assert::AreEqual(int(format::uint8xyz), int(detail::format_reverse_traits<std::array<std::uint8_t, 3>>::format), L"uint8xyz", LINE_INFO());
            Assert::AreEqual(int(format::uint8xyz), int(detail::format_v<std::array<std::uint8_t, 3>>), L"uint8xyz", LINE_INFO());

            Assert::AreEqual(int(format::int8xyzw), int(detail::format_reverse_traits<std::array<std::int8_t, 4>>::format), L"int8xyzw", LINE_INFO());
            Assert::AreEqual(int(format::int8xyzw), int(detail::format_v<std::array<std::int8_t, 4>>), L"int8xyzw", LINE_INFO());

            Assert::AreEqual(int(format::uint8xyzw), int(detail::format_reverse_traits<std::array<std::uint8_t, 4>>::format), L"uint8xyzw", LINE_INFO());
            Assert::AreEqual(int(format::uint8xyzw), int(detail::format_v<std::array<std::uint8_t, 4>>), L"uint8xyzw", LINE_INFO());
        }

        TEST_METHOD(test_to_dxgi) {
            Assert::AreEqual(int(DXGI_FORMAT_R8_SINT), int(to_dxgi(format::int8x)), L"int8x", LINE_INFO());
            Assert::AreEqual(int(DXGI_FORMAT_R32G32B32_FLOAT), int(to_dxgi(format::float32xyz)), L"float32xyz", LINE_INFO());
            Assert::AreEqual(int(DXGI_FORMAT_UNKNOWN), int(to_dxgi(format::unknown)), L"unknown", LINE_INFO());
            Assert::AreEqual(int(DXGI_FORMAT_UNKNOWN), int(to_dxgi(format::int8xyz)), L"int8xyz", LINE_INFO());
        }

        TEST_METHOD(test_to_opengl) {
            Assert::AreEqual(int(GL_BYTE), int(to_opengl(format::int8x)), L"int8x", LINE_INFO());
            Assert::AreEqual(int(GL_FLOAT), int(to_opengl(format::float32xyz)), L"float32xyz", LINE_INFO());
            Assert::AreEqual(int(GL_NONE), int(to_opengl(format::unknown)), L"unknown", LINE_INFO());
            Assert::AreEqual(int(GL_BYTE), int(to_opengl(format::int8xyz)), L"int8xyz", LINE_INFO());
        }

        TEST_METHOD(test_to_string) {
            Assert::AreEqual(L"int8x", to_string<wchar_t>(format::int8x), L"int8x", LINE_INFO());
            Assert::AreEqual("int8x", to_string<char>(format::int8), L"int8", LINE_INFO());
            Assert::ExpectException<std::logic_error>([](void) {
                to_string<char>(static_cast<format>(UINT_MAX));
            }, L"UINT_MAX", LINE_INFO());
        }

        TEST_METHOD(test_to_size) {
            Assert::AreEqual(std::size_t(1), to_size(format::int8x), L"int8x", LINE_INFO());
            Assert::AreEqual(std::size_t(1), to_size(format::int8), L"int8", LINE_INFO());
            Assert::AreEqual(std::size_t(0), to_size(format::unknown), L"unknown", LINE_INFO());
            Assert::ExpectException<std::logic_error>([](void) {
                to_string<char>(static_cast<format>(UINT_MAX));
            }, L"UINT_MAX", LINE_INFO());
        }

        TEST_METHOD(test_to_vulkan) {
#if defined(FPD_WITH_VULKAN)
            Assert::AreEqual(int(VK_FORMAT_R8_SINT), int(to_vulkan(format::int8x)), L"int8x", LINE_INFO());
            Assert::AreEqual(int(VK_FORMAT_R32G32B32_SFLOAT), int(to_vulkan(format::float32xyz)), L"float32xyz", LINE_INFO());
            Assert::AreEqual(int(VK_FORMAT_UNDEFINED), int(to_vulkan(format::unknown)), L"unknown", LINE_INFO());
            Assert::AreEqual(int(VK_FORMAT_R8G8B8_SINT), int(to_vulkan(format::int8xyz)), L"int8xyz", LINE_INFO());
#endif /* defined(FPD_WITH_VULKAN) */
        }

        TEST_METHOD(test_dispatch_format) {
            Assert::IsTrue(dispatch_format<bool>(format::int8x, [](const auto tag) {
                return std::is_same_v<std::decay_t<decltype(tag)>, std::int8_t *>;
            }), L"int8x", LINE_INFO());
            Assert::IsTrue(dispatch_format<bool>(format::int8xy, [](const auto tag) {
                return std::is_same_v<std::decay_t<decltype(tag)>, std::array<std::int8_t, 2> *>;
            }), L"int8xy", LINE_INFO());
        }

        TEST_METHOD(test_scalar_formats) {
            Assert::IsTrue(detail::in_format_list_v<scalar_formats, format::int8x>, L"int8x", LINE_INFO());
            Assert::IsTrue(detail::in_format_list_v<scalar_formats, format::int32x>, L"int32x", LINE_INFO());
            Assert::IsTrue(detail::in_format_list_v<scalar_formats, format::float32>, L"float32", LINE_INFO());
            Assert::IsFalse(detail::in_format_list_v<scalar_formats, format::float32xy>, L"float32xy", LINE_INFO());
        }

        TEST_METHOD(test_to_float) {
            Assert::AreEqual(int(format::float32), int(to_floating_point(format::float32)), L"float32", LINE_INFO());
            Assert::AreEqual(int(format::float32), int(to_floating_point(format::int8x)), L"int8x", LINE_INFO());
            Assert::AreEqual(int(format::float32), int(to_floating_point(format::int32x)), L"int32x", LINE_INFO());
            Assert::AreEqual(int(format::float32xy), int(to_floating_point(format::int32xy)), L"int32xy", LINE_INFO());
            Assert::AreEqual(int(format::float64), int(to_floating_point(format::float64)), L"float64", LINE_INFO());
            Assert::AreEqual(int(format::float64), int(to_floating_point(format::int64)), L"int64", LINE_INFO());
            Assert::AreEqual(int(format::float64xyzw), int(to_floating_point(format::int64xyzw)), L"int64xyzw", LINE_INFO());
        }

        TEST_METHOD(test_make_format) {
            Assert::AreEqual(int(format::float32), int(make_format(format::float32, 1)), L"float32, 1", LINE_INFO());
            Assert::AreEqual(int(format::float32xy), int(make_format(format::float32, 2)), L"float32, 2", LINE_INFO());
            Assert::AreEqual(int(format::float32xyz), int(make_format(format::float32xy, 3)), L"float32xy, 3", LINE_INFO());
            Assert::AreEqual(int(format::float32xyzw), int(make_format(format::float32xy, 4)), L"float32xy, 4", LINE_INFO());
            Assert::AreEqual(int(format::unknown), int(make_format(format::float32xy, 0)), L"float32xy, 0", LINE_INFO());
            Assert::AreEqual(int(format::unknown), int(make_format(format::float32xy, 5)), L"float32xy, 5", LINE_INFO());
        }
    };
}
