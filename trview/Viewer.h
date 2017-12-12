#pragma once

#include <Windows.h>
#include <d3d11.h>
#include <atlbase.h>

#include <vector>
#include <string>
#include <memory>

#include "..\trlevel\ILevel.h"

#include "TextureWindow.h"

namespace trview
{
    class Viewer
    {
    public:
        explicit Viewer(HWND window);

        void render();

        void open(const std::wstring filename);

        void cycle();
    private:
        void initialise_d3d(HWND window);

        CComPtr<IDXGISwapChain>         _swap_chain;
        CComPtr<ID3D11Device>           _device;
        CComPtr<ID3D11DeviceContext>    _context;
        CComPtr<ID3D11RenderTargetView> _render_target_view;

        struct Texture
        {
            CComPtr<ID3D11Texture2D>          texture;
            CComPtr<ID3D11ShaderResourceView> view;
        };

        std::vector<Texture> _level_textures;
        std::unique_ptr<trlevel::ILevel> _current_level;
        std::unique_ptr<TextureWindow> _texture_window;

        CComPtr<ID3D11BlendState> _blend_state;
    };
}
