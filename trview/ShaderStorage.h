#pragma once

#include <atlbase.h>
#include <d3d11.h>

#include "IShaderStorage.h"

namespace trview
{
    class ShaderStorage final : public IShaderStorage 
    {
    public:
        virtual ~ShaderStorage() = default;

        explicit ShaderStorage(const CComPtr<ID3D11Device>& device);

        // Get the shader with the specified name.
        // name: The name of the shader to find.
        // Returns: The shader or nullptr if the shader was not found.
        IShader* shader(const std::string& name) const override;
    private:
        CComPtr<ID3D11Device> _device;
    };
}
