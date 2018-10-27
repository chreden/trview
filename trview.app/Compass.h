#pragma once

#include <trview.graphics/Device.h>
#include <trview.graphics/RenderTarget.h>
#include "Mesh.h"

namespace trview
{
    struct ICamera;
    struct ILevelTextureStorage;

    class Compass final
    {
    public:
        void render(const graphics::Device& device, const ICamera& camera, const ILevelTextureStorage& texture_storage);
    private:
        std::unique_ptr<graphics::RenderTarget> _render_target;
        std::unique_ptr<Mesh> _mesh;
    };
}
