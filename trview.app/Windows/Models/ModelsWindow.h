#pragma once

#include "IModelsWindow.h"
#include <trview.graphics/IDevice.h>

namespace trview
{
    struct IMesh;
    class ModelsWindow final : public IModelsWindow
    {
    public:
        ModelsWindow(const std::shared_ptr<graphics::IDevice>& device,
            const graphics::IRenderTarget::SizeSource& render_target_source,
            const std::shared_ptr<graphics::IShaderStorage>& shader_storage);
        virtual ~ModelsWindow() = default;
        void render() override;
        void set_model_storage(const std::weak_ptr<IModelStorage>& model_storage) override;
        void set_number(int32_t number) override;
        void update(float delta) override;
    private:
        bool render_meshes_window();

        std::string _id{ "Models 0" };
        std::weak_ptr<IModelStorage> _model_storage;
        std::weak_ptr<IMesh> _selected_mesh;
        std::unique_ptr<graphics::IRenderTarget> _render_target;
        std::shared_ptr<graphics::IDevice> _device;
        Microsoft::WRL::ComPtr<ID3D11DepthStencilState> _depth_stencil_state;
        graphics::IShader* _vertex_shader;
        graphics::IShader* _pixel_shader;
    };
}
