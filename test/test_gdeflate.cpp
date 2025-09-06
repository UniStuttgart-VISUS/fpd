// <copyright file="test_gdeflate.cpp" company="Visualisierungsinstitut der Universität Stuttgart">
// Copyright © 2025 Visualisierungsinstitut der Universität Stuttgart.
// Licensed under the MIT licence. See LICENCE file for details.
// </copyright>
// <author>Christoph Müller</author>

#include "pch.h"


namespace fpd {

    TEST_CLASS(test_gdeflate) {

    public:

#if defined(FPD_WITH_DSTORAGE)
        //TEST_METHOD(dstorage) {
        //    std::vector<std::uint32_t> data(10000);
        //    std::uint32_t n = 0;
        //    std::generate(data.begin(), data.end(), [&](void) { return ++n; });

        //    const auto blocks = detail::io_traits<IDStorageFile>::compress<std::uint8_t>(
        //        DSTORAGE_COMPRESSION_FORMAT_GDEFLATE,
        //        data.begin(),
        //        data.end(),
        //        4);

        //    Assert::AreEqual(std::size_t(4), blocks.size(), L"requested # of blocks", LINE_INFO());
        //}
#endif /* defined(FPD_WITH_DSTORAGE) */

    };
}
