#pragma once

#include <trview.graphics/Device.h>
#include <trview.graphics/RenderTarget.h>
#include "Mesh.h"
#include "Camera.h"

namespace trview
{
    struct ILevelTextureStorage;

    namespace graphics 
    {
        struct IShaderStorage;
    }

    class Compass final
    {
    public:
        Compass();
        void render(const graphics::Device& device, const ICamera& camera, const ILevelTextureStorage& texture_storage, const graphics::IShaderStorage& shader_storage);
    private:
        std::unique_ptr<graphics::RenderTarget> _render_target;
        std::unique_ptr<Mesh> _mesh;
        Camera _mesh_camera;
    };
}
