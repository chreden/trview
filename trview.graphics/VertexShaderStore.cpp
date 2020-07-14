#include "VertexShaderStore.h"

using namespace Microsoft::WRL;

namespace trview
{
    namespace graphics
    {
        VertexShaderStore::VertexShaderStore(const ComPtr<ID3D11DeviceContext>& context)
            : _context(context)
        {
            _context->IAGetInputLayout(&_input_layout);
            _context->VSGetShader(&_vertex_shader, nullptr, nullptr);
        }

        VertexShaderStore::~VertexShaderStore()
        {
            _context->IASetInputLayout(_input_layout.Get());
            _context->VSSetShader(_vertex_shader.Get(), nullptr, 0);
        }
    }
}