#pragma once

#include <vector>
#include <cstdint>

#include "IShader.h"
#include <trview.graphics/Device.h>

namespace trview
{
    namespace graphics
    {
        class PixelShader final : public IShader
        {
        public:
            PixelShader(const graphics::Device& device, const std::vector<uint8_t>& data);

            virtual ~PixelShader() = default;

            virtual void apply(const Microsoft::WRL::ComPtr<ID3D11DeviceContext>& context) override;
        private:
            Microsoft::WRL::ComPtr<ID3D11PixelShader> _pixel_shader;
        };
    }
}

