#include "gtest/gtest.h"

import trview.graphics;

using namespace trview::graphics;

/// Tests that trying to create a pixel shader with no data throws.
TEST(PixelShader, EmptyDataThrows)
{
    ASSERT_THROW(PixelShader(Device(), std::vector<uint8_t>()), std::exception);
};
