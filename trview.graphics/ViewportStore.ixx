module;

#include <wrl/client.h>
#include <d3d11.h>

export module trview.graphics:ViewportStore;

import std;

namespace trview
{
    namespace graphics
    {
        class ViewportStore final
        {
        public:
            explicit ViewportStore(const Microsoft::WRL::ComPtr<ID3D11DeviceContext>& context);
            ~ViewportStore();
        private:
            Microsoft::WRL::ComPtr<ID3D11DeviceContext> _context;
            std::vector<D3D11_VIEWPORT> _viewports;
        };
    }
}