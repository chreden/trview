#pragma once

namespace trview
{
    namespace graphics
    {
        struct IShaderStorage;
        class Device;
    }

    void load_default_shaders(const graphics::Device& device, graphics::IShaderStorage& storage);
}
