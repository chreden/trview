#pragma once

#include <trview.graphics/Device.h>

namespace trview
{
    namespace graphics
    {
        struct IShaderStorage;
    }

    void load_default_shaders(const graphics::Device& device, graphics::IShaderStorage& storage);
}
