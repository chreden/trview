#include "RasterizerStateStore.h"

namespace trview
{
    namespace graphics
    {
        RasterizerStateStore::RasterizerStateStore(const Microsoft::WRL::ComPtr<ID3D11DeviceContext>& context)
            : _context(context)
        {
            _context->RSGetState(&_rasterizer);
        }

        RasterizerStateStore::~RasterizerStateStore()
        {
            _context->RSSetState(_rasterizer.Get());
        }
    }
}
