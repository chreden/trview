module;

#include <wrl/client.h>
#include <d3d11.h>

export module trview.graphics:IShader;

namespace trview
{
    namespace graphics
    {
        export struct IShader
        {
            virtual ~IShader() = 0;

            virtual void apply(const Microsoft::WRL::ComPtr<ID3D11DeviceContext>& context) = 0;
        };
    }
}

