#include "stdafx.h"
#include "ShaderStorage.h"

namespace trview
{
    ShaderStorage::ShaderStorage(const CComPtr<ID3D11Device>& device)
        : _device(device)
    {
    }

    IShader* ShaderStorage::shader(const std::string & name) const
    {
        return nullptr;
    }
}
