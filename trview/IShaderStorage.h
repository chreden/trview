#pragma once

#include <string>

namespace trview
{
    struct IShader;

    struct IShaderStorage
    {
        virtual ~IShaderStorage() = 0;

        // Get the shader with the specified name.
        // name: The name of the shader to find.
        // Returns: The shader or nullptr if the shader was not found.
        virtual IShader* shader(const std::string& name) const = 0;
    };
}
