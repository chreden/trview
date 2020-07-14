#include "ShaderStorage.h"

namespace trview
{
    namespace graphics
    {
        // Add the shader to the store.
        // name: The name of the shader.
        // shader: The shader.
        // Throws: std::exception if shader was nullptr.
        void ShaderStorage::add(const std::string& name, std::unique_ptr<IShader> shader)
        {
            if (!shader)
            {
                const auto error{ std::string("Could not add null shader to store with name '") + name + "'" };
                throw std::exception(error.c_str());
            }

            _shaders.insert({ name, std::move(shader) });
        }

        // Get the shader with the specified name.
        // name: The name of the shader to find.
        // Returns: The shader or nullptr if the shader was not found.
        IShader* ShaderStorage::get(const std::string & name) const
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
