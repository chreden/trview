#include "VertexShader.h"

namespace trview
{
    namespace graphics
    {
        VertexShader::VertexShader(const CComPtr<ID3D11Device>& device, const std::vector<uint8_t>& data, const std::vector<D3D11_INPUT_ELEMENT_DESC>& input_desc)
        {
            if (data.empty())
            {
                throw std::exception("Data for VertexShader cannot be empty");
            }

            if (input_desc.empty())
            {
                throw std::exception("InputLayout for VertexShader cannot be empty");
            }

            device->CreateVertexShader(&data[0], data.size(), nullptr, &_vertex_shader);
            device->CreateInputLayout(&input_desc[0], static_cast<uint32_t>(input_desc.size()), &data[0], static_cast<uint32_t>(data.size()), &_input_layout);
        }

        void VertexShader::apply(const CComPtr<ID3D11DeviceContext>& context)
        {
            context->IASetInputLayout(_input_layout);
            context->VSSetShader(_vertex_shader, nullptr, 0);
        }
    }
}