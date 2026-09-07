export module trview.app:DefaultShaders;

import std;

import trview.graphics;

namespace trview
{
    export void load_default_shaders(const std::shared_ptr<graphics::IDevice>& device, const std::shared_ptr<graphics::IShaderStorage>& storage);
}
