#include "stdafx.h"
#include "VertexShader.h"

namespace trview
{
    VertexShader::VertexShader(const CComPtr<ID3D11Device>& device, const std::vector<uint8_t>& data)
    {
        // Check for zero length data (throw exception).
        HRESULT result = device->CreateVertexShader(&data[0], data.size(), nullptr, &_vertex_shader);
        // Do more things with the HRESULT here - throw errors if there were any as we need the shaders...
    }
}