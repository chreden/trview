#pragma once

#include <d3d11.h>
#include <wrl/client.h>

#include <trlevel/LevelVersion.h>

#include "../Geometry/Model/IModelStorage.h"
#include "../Geometry/Model/IModel.h"
#include "../Graphics/ILevelTextureStorage.h"

namespace trview
{
    struct ICamera;
    class Skybox final
    {
    public:
        explicit Skybox(const std::shared_ptr<graphics::IDevice>& device,
            const IModelStorage& model_storage,
            const trlevel::PlatformAndVersion& platform_and_version,
            const std::weak_ptr<ILevelTextureStorage>& texture_storage);
        void render(const ICamera& camera);

        static float offset;
    private:
        Microsoft::WRL::ComPtr<ID3D11DeviceContext> _context;
        Microsoft::WRL::ComPtr<ID3D11DepthStencilState> _skybox_depth_stencil;
        std::weak_ptr<IModel> _model;
        trlevel::PlatformAndVersion _platform_and_version;
        std::shared_ptr<ITransparencyBuffer> _transparency;
    };
}
