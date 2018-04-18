#include "stdafx.h"
#include "DefaultShaders.h"

#include "IShaderStorage.h"
#include <trview.common/FileLoader.h>

#include <vector>

namespace trview
{
    void load_default_shaders(const CComPtr<ID3D11Device>& device, IShaderStorage& storage)
    {
        D3D11_INPUT_ELEMENT_DESC input_desc[3];
        memset(&input_desc, 0, sizeof(input_desc));
        input_desc[0].SemanticName = "Position";
        input_desc[0].SemanticIndex = 0;
        input_desc[0].InstanceDataStepRate = 0;
        input_desc[0].InputSlot = 0;
        input_desc[0].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
        input_desc[0].Format = DXGI_FORMAT_R32G32B32_FLOAT;

        input_desc[1].SemanticName = "Texcoord";
        input_desc[1].SemanticIndex = 0;
        input_desc[1].InstanceDataStepRate = 0;
        input_desc[1].InputSlot = 0;
        input_desc[1].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
        input_desc[1].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
        input_desc[1].Format = DXGI_FORMAT_R32G32_FLOAT;

        input_desc[2].SemanticName = "Texcoord";
        input_desc[2].SemanticIndex = 1;
        input_desc[2].InstanceDataStepRate = 0;
        input_desc[2].InputSlot = 0;
        input_desc[2].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
        input_desc[2].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
        input_desc[2].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;

        std::vector<char> vs_data = load_file(L"level_vertex_shader.cso");

        // Create the vertex shader.


        std::vector<char> ps_data = load_file(L"level_pixel_shader.cso");
        // create pixel shader.....
    }
}
