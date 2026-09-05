module;

#include <wrl/client.h>
#include <d3d11.h>

export module trview.graphics:VertexShaderStore;

namespace trview
{
    namespace graphics
    {
        export class VertexShaderStore final
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