#pragma once

namespace trview
{
    namespace graphics
    {
        struct IShaderStorage;
        struct IDevice;
    }

    void load_default_shaders(const graphics::IDevice& device, graphics::IShaderStorage& storage);
}
