#pragma once

#include <atlbase.h>
#include <d3d11.h>

#include <cstdint>
#include <vector>

#include "IShader.h"

namespace trview
{
    class VertexShader final : public IShader
    {
    public:
        // Create a vertex shader from the specified data using the given device.
        // device: The D3D device to use to create the vertex shader.
        // data: The data to use to compile the shader.
        VertexShader(const CComPtr<ID3D11Device>& device, const std::vector<uint8_t>& data);

        virtual ~VertexShader() = default;
    private:
        CComPtr<ID3D11VertexShader> _vertex_shader;
    };
}
