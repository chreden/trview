#include "DefaultShaders.h"
#include "ResourceHelper.h"
#include "resource.h"
#include <trview.graphics/Device.h>
#include <trview.graphics/IShaderStorage.h>
#include <trview.graphics/VertexShader.h>
#include <trview.graphics/PixelShader.h>

using namespace Microsoft::WRL;

namespace trview
{
    namespace
    {
        std::vector<uint8_t> get_shader_resource(int id)
        {
            auto resource = get_resource_memory(id, L"SHADER");
            return std::vector<uint8_t>(resource.data, resource.data + resource.size);
        }

        void load_level_shaders(const graphics::IDevice& device, graphics::IShaderStorage& storage)
        {
            std::vector<D3D11_INPUT_ELEMENT_DESC> input_desc(4);
            memset(&input_desc[0], 0, sizeof(D3D11_INPUT_ELEMENT_DESC) * input_desc.size());
            input_desc[0].SemanticName = "Position";
            input_desc[0].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
            input_desc[0].Format = DXGI_FORMAT_R32G32B32_FLOAT;

            input_desc[1].SemanticName = "Normal";
            input_desc[1].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
            input_desc[1].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
            input_desc[1].Format = DXGI_FORMAT_R32G32B32_FLOAT;

            input_desc[2].SemanticName = "Texcoord";
            input_desc[2].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
            input_desc[2].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
            input_desc[2].Format = DXGI_FORMAT_R32G32_FLOAT;

            input_desc[3].SemanticName = "Texcoord";
            input_desc[3].SemanticIndex = 1;
            input_desc[3].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
            input_desc[3].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
            input_desc[3].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;

            storage.add("level_vertex_shader", std::make_unique<graphics::VertexShader>(device, get_shader_resource(IDR_LEVEL_VERTEX_SHADER), input_desc));
            storage.add("level_pixel_shader", std::make_unique<graphics::PixelShader>(device, get_shader_resource(IDR_LEVEL_PIXEL_SHADER)));
            storage.add("selection_pixel_shader", std::make_unique<graphics::PixelShader>(device, get_shader_resource(IDR_SELECTION_SHADER)));
        }

        void load_ui_shaders(const graphics::IDevice& device, graphics::IShaderStorage& storage)
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

            storage.add("ui_vertex_shader", std::make_unique<graphics::VertexShader>(device, get_shader_resource(IDR_UI_VERTEX_SHADER), input_desc));
            storage.add("ui_pixel_shader", std::make_unique<graphics::PixelShader>(device, get_shader_resource(IDR_UI_PIXEL_SHADER)));
        }
    }

    void load_default_shaders(const std::shared_ptr<graphics::IDevice>& device, const std::shared_ptr<graphics::IShaderStorage>& storage)
    {
        load_level_shaders(*device, *storage);
        load_ui_shaders(*device, *storage);
    }
}
