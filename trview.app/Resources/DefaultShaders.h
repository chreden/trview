#pragma once

namespace trview
{
    namespace graphics
    {
        struct IShaderStorage;
        struct IDevice;
    }

    void load_default_shaders(const std::shared_ptr<graphics::IDevice>& device, const std::shared_ptr<graphics::IShaderStorage>& storage);
}
