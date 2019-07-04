#include "gtest/gtest.h"
#include <trview.graphics/PixelShader.h>

using namespace trview::graphics;

/// Tests that trying to create a pixel shader with no data throws.
TEST(PixelShader, EmptyDataThrows)
{
    ASSERT_THROW(PixelShader(Device(), std::vector<uint8_t>()), std::exception);
};
