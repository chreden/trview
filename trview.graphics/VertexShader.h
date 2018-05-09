#pragma once

#include <cstdint>
#include <vector>

#include "IShader.h"

namespace trview
{
    namespace graphics
    {
        class VertexShader final : public IShader
        {
        public:
            // Create a vertex shader from the specified data using the given device.
            // device: The D3D device to use to create the vertex shader.
            // data: The data to use to compile the shader.
            // input_desc: The input layout description.
            VertexShader(const Microsoft::WRL::ComPtr<ID3D11Device>& device, const std::vector<uint8_t>& data, const std::vector<D3D11_INPUT_ELEMENT_DESC>& input_desc);

            virtual ~VertexShader() = default;

            virtual void apply(const Microsoft::WRL::ComPtr<ID3D11DeviceContext>& context) override;
        private:
            Microsoft::WRL::ComPtr<ID3D11VertexShader> _vertex_shader;
            Microsoft::WRL::ComPtr<ID3D11InputLayout> _input_layout;
        };
    }
}
