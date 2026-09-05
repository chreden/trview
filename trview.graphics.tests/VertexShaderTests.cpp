#include "gtest/gtest.h"
#include <d3d11.h>

import trview.graphics;
import std;
import std.compat;

using namespace trview::graphics;

/// Tests that trying to create a vertex shader with no data throws.
TEST(VertexShader, EmptyDataThrows)
{
    ASSERT_THROW(VertexShader(Device(), std::vector<uint8_t>(), std::vector<D3D11_INPUT_ELEMENT_DESC>(1)), std::exception);
}

/// Tests that trying to create a vertex shader with no input descriptions throws.
TEST(VertexShader, EmptyInputDescriptionThrows)
{
    ASSERT_THROW(VertexShader(Device(), std::vector<uint8_t>(1), std::vector<D3D11_INPUT_ELEMENT_DESC>()), std::exception);
}
