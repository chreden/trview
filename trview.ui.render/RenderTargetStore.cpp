#include "RenderTargetStore.h"

namespace trview
{
    namespace ui
    {
        namespace render
        {
            RenderTargetStore::RenderTargetStore(const Microsoft::WRL::ComPtr<ID3D11DeviceContext>& context)
                : _context(context)
            {
                _context->OMGetRenderTargets(1, &_render_target, nullptr);
            }

            RenderTargetStore::~RenderTargetStore()
            {
                _context->OMSetRenderTargets(1, &_render_target, nullptr);
            }
        }
    }
}