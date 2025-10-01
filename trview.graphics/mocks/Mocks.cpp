#include "stdafx.h"

#include <gmock/gmock.h>

#include "mocks/ISamplerState.h"

// Fix for printing ComPtr - removes value printing:
namespace testing
{
    namespace internal
    {
        template <typename T>
        class UniversalPrinter<const Microsoft::WRL::ComPtr<T>&> {
        public:
            // MSVC warns about adding const to a function type, so we want to
            // disable the warning.
            GTEST_DISABLE_MSC_WARNINGS_PUSH_(4180)

                static void Print(const Microsoft::WRL::ComPtr<T>& value, ::std::ostream* os) {
                UniversalPrint(value, os);
            }

            GTEST_DISABLE_MSC_WARNINGS_POP_()
        };
    }
}

#include "D3D/ID3D11DeviceContext.h"
#include "IDevice.h"
#include "IDeviceWindow.h"
#include "IRenderTarget.h"
#include "IShader.h"
#include "IShaderStorage.h"
#include "ISprite.h"
#include "IBuffer.h"

namespace trview
{
    namespace graphics
    {
        namespace mocks
        {
            MockD3D11DeviceContext::MockD3D11DeviceContext() {}
            MockD3D11DeviceContext::~MockD3D11DeviceContext() {}

            MockDevice::MockDevice() {}
            MockDevice::~MockDevice() {}

            MockDeviceWindow::MockDeviceWindow() {}
            MockDeviceWindow::~MockDeviceWindow() {}

            MockRenderTarget::MockRenderTarget() {}
            MockRenderTarget::~MockRenderTarget() {}

            MockShader::MockShader() {}
            MockShader::~MockShader() {}

            MockShaderStorage::MockShaderStorage() {}
            MockShaderStorage::~MockShaderStorage() {}

            MockSprite::MockSprite() {}
            MockSprite::~MockSprite() {}

            MockBuffer::MockBuffer() {}
            MockBuffer::~MockBuffer() {}

            MockSamplerState::MockSamplerState() {}
            MockSamplerState::~MockSamplerState() {}
        }
    }
}