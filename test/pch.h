// <copyright file="pch.h" company="Visualisierungsinstitut der Universität Stuttgart">
// Copyright © 2023 Visualisierungsinstitut der Universität Stuttgart.
// Licensed under the MIT licence. See LICENCE file for details.
// </copyright>
// <author>Christoph Müller</author>

#pragma once

#include <algorithm>

#include "CppUnitTest.h"

#include "fpd/bounding_box.h"
#include "fpd/convert_string.h"
#include "fpd/dependent_bool.h"
#include "fpd/dstorage_io.h"
#include "fpd/element.h"
#include "fpd/element_group.h"
#include "fpd/encoding.h"
#include "fpd/endian.h"
#include "fpd/file.h"
#include "fpd/format.h"
#include "fpd/format_description.h"
#include "fpd/format_traits.h"
#include "fpd/format_variant.h"
#include "fpd/header_segment.h"
#include "fpd/invoke.h"
#include "fpd/literal.h"
#include "fpd/make_unsigned.h"
#include "fpd/memory_io.h"
#include "fpd/meta_data.h"
#include "fpd/meta_data_builder.h"
#include "fpd/mmpld.h"
#include "fpd/pkd_tree_builder.h"
#include "fpd/posix_io.h"
#include "fpd/semantic.h"
#include "fpd/spatial_hierarchy.h"
#include "fpd/swap_bytes.h"
#include "fpd/type_list.h"
#include "fpd/type_size.h"
#include "fpd/win32_io.h"


using namespace Microsoft::VisualStudio::CppUnitTestFramework;
