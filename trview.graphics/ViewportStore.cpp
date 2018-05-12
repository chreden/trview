#include "ViewportStore.h"

namespace trview
{
    namespace graphics
    {
        ViewportStore::ViewportStore(const Microsoft::WRL::ComPtr<ID3D11DeviceContext>& context)
            : _context(context)
        {
            UINT number_of_viewports = 0;
            _context->RSGetViewports(&number_of_viewports, nullptr);

            if (number_of_viewports > 0)
            {
                _viewports.resize(number_of_viewports);
                _context->RSGetViewports(&number_of_viewports, &_viewports[0]);
            }
        }

        ViewportStore::~ViewportStore()
        {
            if (!_viewports.empty())
            {
                _context->RSSetViewports(static_cast<UINT>(_viewports.size()), &_viewports[0]);
            }
        }
    }
}
