#include "stdafx.h"
#include "VertexShader.h"

namespace trview
{
    VertexShader::VertexShader(const CComPtr<ID3D11Device>& device, const std::vector<uint8_t>& data, const std::vector<D3D11_INPUT_ELEMENT_DESC>& input_desc)
    {
        // Check for zero length data (throw exception).
        HRESULT result = device->CreateVertexShader(&data[0], data.size(), nullptr, &_vertex_shader);
        // Do more things with the HRESULT here - throw errors if there were any as we need the shaders...
        result = device->CreateInputLayout(&input_desc[0], input_desc.size(), &data[0], data.size(), &_input_layout);
    }

    void VertexShader::apply(const CComPtr<ID3D11DeviceContext>& context)
    {
        context->IASetInputLayout(_input_layout);
        context->VSSetShader(_vertex_shader, nullptr, 0);
    }
}