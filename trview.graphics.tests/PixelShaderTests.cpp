#include "CppUnitTest.h"

#include <trview.graphics/PixelShader.h>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace trview
{
    namespace graphics
    {
        namespace tests
        {
            TEST_CLASS(PixelShaderTests)
            {
                // Tests that trying to create a pixel shader with no data throws.
                TEST_METHOD(EmptyData)
                {
                    graphics::Device device;
                    Assert::ExpectException<std::exception>([&device]() { PixelShader{ device, std::vector<uint8_t>() }; });
                }
            };
        }
    }
}