#pragma once

#include <atlbase.h>
#include <d3d11.h>
#include <DirectXMath.h>

#include <trlevel/trtypes.h>

namespace trview
{
    class Mesh
    {
    public:
        explicit Mesh(const trlevel::tr_mesh& mesh, CComPtr<ID3D11Device> device);

        void render(CComPtr<ID3D11DeviceContext> context, const DirectX::XMMATRIX& world_view_projection);
    private:
    };
}