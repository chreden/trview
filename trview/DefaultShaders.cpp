#include "stdafx.h"
#include "DefaultShaders.h"

#include <trview.graphics/IShaderStorage.h>
#include <trview.graphics/VertexShader.h>
#include <trview.graphics/PixelShader.h>
#include <trview.common/FileLoader.h>

#include <vector>

namespace trview
{
    namespace
    {
        void load_level_shaders(const CComPtr<ID3D11Device>& device, graphics::IShaderStorage& storage)
        {
            std::vector<D3D11_INPUT_ELEMENT_DESC> input_desc(3);
            memset(&input_desc[0], 0, sizeof(D3D11_INPUT_ELEMENT_DESC) * input_desc.size());
            input_desc[0].SemanticName = "Position";
            input_desc[0].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
            input_desc[0].Format = DXGI_FORMAT_R32G32B32_FLOAT;

            input_desc[1].SemanticName = "Texcoord";
            input_desc[1].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
            input_desc[1].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
            input_desc[1].Format = DXGI_FORMAT_R32G32_FLOAT;

            input_desc[2].SemanticName = "Texcoord";
            input_desc[2].SemanticIndex = 1;
            input_desc[2].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
            input_desc[2].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
            input_desc[2].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;

            storage.add("level_vertex_shader", std::make_unique<graphics::VertexShader>(device, load_file(L"level_vertex_shader.cso"), input_desc));
            storage.add("level_pixel_shader", std::make_unique<graphics::PixelShader>(device, load_file(L"level_pixel_shader.cso")));
        }

        void load_ui_shaders(const CComPtr<ID3D11Device>& device, graphics::IShaderStorage& storage)
        {
            std::vector<D3D11_INPUT_ELEMENT_DESC> input_desc(2);
            memset(&input_desc[0], 0, sizeof(D3D11_INPUT_ELEMENT_DESC) * input_desc.size());
            input_desc[0].SemanticName = "Position";
            input_desc[0].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
            input_desc[0].Format = DXGI_FORMAT_R32G32B32_FLOAT;

            input_desc[1].SemanticName = "Texcoord";
            input_desc[1].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
            input_desc[1].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
            input_desc[1].Format = DXGI_FORMAT_R32G32_FLOAT;

            storage.add("ui_vertex_shader", std::make_unique<graphics::VertexShader>(device, load_file(L"ui_vertex_shader.cso"), input_desc));
            storage.add("ui_pixel_shader", std::make_unique<graphics::PixelShader>(device, load_file(L"ui_pixel_shader.cso")));
        }
    }

    void load_default_shaders(const CComPtr<ID3D11Device>& device, graphics::IShaderStorage& storage)
    {
        load_level_shaders(device, storage);
        load_ui_shaders(device, storage);
    }
}
