#include "Skybox.h"
#include "../Camera/ICamera.h"
#include <trlevel/ILevel.h>
#include "../Geometry/TransparencyBuffer.h"

namespace trview
{
    float Skybox::offset = 0.0f;

    using namespace DirectX::SimpleMath;

    Skybox::Skybox(const std::shared_ptr<graphics::IDevice>& device,
        const IModelStorage& model_storage,
        const trlevel::PlatformAndVersion& platform_and_version,
        const std::weak_ptr<ILevelTextureStorage>& texture_storage)
        : _context(device->context()), _platform_and_version(platform_and_version)
    {
        auto skybox_id = trlevel::get_skybox_id(platform_and_version);
        if (skybox_id)
        {
            _model = model_storage.find_by_type_id(static_cast<uint16_t>(skybox_id.value()));
        }

        D3D11_DEPTH_STENCIL_DESC stencil_desc;
        memset(&stencil_desc, 0, sizeof(stencil_desc));
        stencil_desc.DepthEnable = true;
        stencil_desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
        stencil_desc.DepthFunc = D3D11_COMPARISON_LESS;
        stencil_desc.StencilEnable = true;
        stencil_desc.StencilReadMask = 0xFF;
        stencil_desc.StencilWriteMask = 0xFF;
        stencil_desc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
        stencil_desc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
        stencil_desc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
        stencil_desc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
        stencil_desc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
        stencil_desc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
        stencil_desc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
        stencil_desc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
        _skybox_depth_stencil = device->create_depth_stencil_state(stencil_desc);

        _transparency = std::make_shared<TransparencyBuffer>(device, texture_storage);
    }

    void Skybox::render(const ICamera& camera)
    {
        if (auto model = _model.lock())
        {
            Microsoft::WRL::ComPtr<ID3D11DepthStencilState> old_depth_state;
            _context->OMGetDepthStencilState(&old_depth_state, nullptr);
            _context->OMSetDepthStencilState(_skybox_depth_stencil.Get(), 1);
            // const float offset =
            //     _platform_and_version.version < trlevel::LevelVersion::Tomb4 ?
            //     // model->bounding_box().Extents.y :
            //     0.0f : 
            //     0.0f;
            // const float ex = model->bounding_box().Center.y;
            // const float to_use = offset == 0 ? (ex * -4) : offset;
            // const Matrix world = Matrix::CreateTranslation(camera.rendering_position() - Vector3(0, to_use, 0));
            // const Matrix world = Matrix::CreateScale(offset) * Matrix::CreateTranslation(camera.rendering_position());
            const Matrix world = Matrix::CreateTranslation(camera.rendering_position());//  Matrix::CreateScale(offset)* Matrix::CreateTranslation(camera.rendering_position());
            model->render(world, camera.view_projection(), Colour::White);

            _transparency->reset();
            model->render_transparency(world, *_transparency, Colour::White);
            _transparency->sort(camera.rendering_position());
            _transparency->render(camera, false);

            _context->OMSetDepthStencilState(old_depth_state.Get(), 1);
        }
    }
}
