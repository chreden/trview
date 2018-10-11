#pragma once

#include <wrl/client.h>
#include <d3d11.h>

namespace trview
{
    namespace graphics
    {
        class VertexShaderStore final
        {
        public:
            VertexShaderStore(const Microsoft::WRL::ComPtr<ID3D11DeviceContext>& context);
            ~VertexShaderStore();
        private:
            Microsoft::WRL::ComPtr<ID3D11DeviceContext> _context;
            Microsoft::WRL::ComPtr<ID3D11InputLayout>   _input_layout;
            Microsoft::WRL::ComPtr<ID3D11VertexShader>  _vertex_shader;
        };
    }
}