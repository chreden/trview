module;
export module trview.graphics:IShaderStorage;

import std;
import :IShader;

namespace trview
{
    namespace graphics
    {
        export struct IShaderStorage
        {
            virtual ~IShaderStorage() = 0;

            // Add the shader to the store.
            // name: The name of the shader.
            // shader: The shader.
            virtual void add(const std::string& name, std::unique_ptr<IShader> shader) = 0;

            // Get the shader with the specified name.
            // name: The name of the shader to find.
            // Returns: The shader or nullptr if the shader was not found.
            virtual IShader* get(const std::string& name) const = 0;
        };
    }
}
