#include "RenderTargetStore.h"

namespace trview
{
    namespace ui
    {
        namespace render
        {
            RenderTargetStore::RenderTargetStore(CComPtr<ID3D11DeviceContext> context)
                : _context(context)
            {
                _context->OMGetRenderTargets(1, &_render_target.p, nullptr);
            }

            RenderTargetStore::~RenderTargetStore()
            {
                _context->OMSetRenderTargets(1, &_render_target.p, nullptr);
            }
        }
    }
}