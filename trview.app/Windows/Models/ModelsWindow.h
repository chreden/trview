#pragma once

#include "IModelsWindow.h"
#include <trview.graphics/IDevice.h>
#include <trview.graphics/IBuffer.h>
#include "../../Geometry/ITransparencyBuffer.h"

namespace trview
{
    struct IModel;
    class ModelsWindow final : public IModelsWindow
    {
    public:
        ModelsWindow(const std::shared_ptr<graphics::IDevice>& device,
            const graphics::IRenderTarget::SizeSource& render_target_source,
            const std::shared_ptr<graphics::IShaderStorage>& shader_storage,
            const graphics::IBuffer::ConstantSource& buffer_source,
            ITransparencyBuffer::Source transparency_buffer_source);
        virtual ~ModelsWindow() = default;
        void render() override;
        void set_level_texture_storage(const std::weak_ptr<ILevelTextureStorage>& level_texture_storage) override;
        void set_model_storage(const std::weak_ptr<IModelStorage>& model_storage) override;
        void set_number(int32_t number) override;
        void update(float delta) override;
    private:
        bool render_meshes_window();

        std::string _id{ "Models 0" };
        std::weak_ptr<IModelStorage> _model_storage;
        std::weak_ptr<IModel> _selected_model;
        std::unique_ptr<graphics::IRenderTarget> _render_target;
        std::shared_ptr<graphics::IDevice> _device;
        Microsoft::WRL::ComPtr<ID3D11DepthStencilState> _depth_stencil_state;
        graphics::IShader* _vertex_shader;
        graphics::IShader* _pixel_shader;
        float _rotation{ 0.0f };
        std::unique_ptr<graphics::IBuffer> _pixel_shader_data;
        std::unique_ptr<ITransparencyBuffer> _transparency_buffer;
        ITransparencyBuffer::Source _transparency_buffer_source;
    };
}
