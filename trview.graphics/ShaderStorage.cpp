#include "ShaderStorage.h"

namespace trview
{
    namespace graphics
    {
        ShaderStorage::ShaderStorage(const CComPtr<ID3D11Device>& device)
            : _device(device)
        {
        }

        // Add the shader to the store.
        // name: The name of the shader.
        // shader: The shader.
        void ShaderStorage::add(const std::string& name, std::unique_ptr<IShader> shader)
        {
            _shaders.insert({ name, std::move(shader) });
        }

        // Get the shader with the specified name.
        // name: The name of the shader to find.
        // Returns: The shader or nullptr if the shader was not found.
        IShader* ShaderStorage::shader(const std::string & name) const
        {
            auto shader = _shaders.find(name);
            if (shader == _shaders.end())
            {
                return nullptr;
            }
            return shader->second.get();
        }
    }
}
