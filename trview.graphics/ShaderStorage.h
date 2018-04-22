#pragma once

#include <unordered_map>

#include "IShaderStorage.h"
#include "IShader.h"

namespace trview
{
    namespace graphics
    {
        class ShaderStorage final : public IShaderStorage
        {
        public:
            virtual ~ShaderStorage() = default;

            ShaderStorage() = default;

            // Add the shader to the store.
            // name: The name of the shader.
            // shader: The shader.
            // Throws: std::exception if shader was nullptr.
            virtual void add(const std::string& name, std::unique_ptr<IShader> shader) override;

            // Get the shader with the specified name.
            // name: The name of the shader to find.
            // Returns: The shader or nullptr if the shader was not found.
            virtual IShader* get(const std::string& name) const override;
        private:
            std::unordered_map<std::string, std::unique_ptr<IShader>> _shaders;
        };
    }
}
