#pragma once

#include <atlbase.h>
#include <d3d11.h>
#include <unordered_map>

#include "IShaderStorage.h"
#include "IShader.h"

namespace trview
{
    class ShaderStorage final : public IShaderStorage 
    {
    public:
        virtual ~ShaderStorage() = default;

        explicit ShaderStorage(const CComPtr<ID3D11Device>& device);

        // Add the shader to the store.
        // name: The name of the shader.
        // shader: The shader.
        virtual void add(const std::string& name, std::unique_ptr<IShader> shader) override;

        // Get the shader with the specified name.
        // name: The name of the shader to find.
        // Returns: The shader or nullptr if the shader was not found.
        virtual IShader* shader(const std::string& name) const override;
    private:
        CComPtr<ID3D11Device> _device;
        std::unordered_map<std::string, std::unique_ptr<IShader>> _shaders;
    };
}
