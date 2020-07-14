#include "stdafx.h"
#include "RenderTargetStore.h"

namespace trview
{
    namespace graphics
    {
        RenderTargetStore::RenderTargetStore(const Microsoft::WRL::ComPtr<ID3D11DeviceContext>& context)
            : _context(context)
        {
            _context->OMGetRenderTargets(1, &_render_target, &_depth_stencil);
        }

        RenderTargetStore::~RenderTargetStore()
        {
            _context->OMSetRenderTargets(1, _render_target.GetAddressOf(), _depth_stencil.Get());
        }
    }
}