#include "ModelsWindow.h"
#include "../../Geometry/Model/IModelStorage.h"
#include "../../Geometry/Model/IModel.h"
#include "../../Messages/Messages.h"
#include "../../Elements/ILevel.h"

#include <trview.graphics/RenderTargetStore.h>
#include <trview.graphics/ViewportStore.h>

namespace trview
{
    namespace
    {
#pragma warning(push)
#pragma warning(disable : 4324)
#pragma pack(push, 16)
        __declspec(align(16)) struct PixelShaderData
        {
            bool disable_transparency;
        };
#pragma pack(pop)
#pragma warning(pop)
    }

    ModelsWindow::ModelsWindow(const std::shared_ptr<graphics::IDevice>& device,
        const graphics::IRenderTarget::SizeSource& render_target_source,
        const std::shared_ptr<graphics::IShaderStorage>& shader_storage,
        const graphics::IBuffer::ConstantSource& buffer_source,
        ITransparencyBuffer::Source transparency_buffer_source,
        const graphics::ISamplerState::Source& sampler_source,
        std::unique_ptr<input::IMouse> mouse,
        const std::weak_ptr<IMessageSystem>& messaging)
        : _device(device), _render_target(render_target_source(512, 512, graphics::IRenderTarget::DepthStencilMode::Enabled)), _transparency_buffer_source(transparency_buffer_source),
        _sampler_state(sampler_source(graphics::ISamplerState::AddressMode::Clamp)), _mouse(std::move(mouse)), _messaging(messaging)
    {
        _vertex_shader = shader_storage->get("level_vertex_shader");
        _pixel_shader = shader_storage->get("level_pixel_shader");

        D3D11_DEPTH_STENCIL_DESC stencil_desc;
        memset(&stencil_desc, 0, sizeof(stencil_desc));
        stencil_desc.DepthEnable = true;
        stencil_desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
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
        _depth_stencil_state = device->create_depth_stencil_state(stencil_desc);

        _pixel_shader_data = buffer_source(sizeof(PixelShaderData));

        _token_store += _mouse->mouse_down += [&](auto button) { _camera_input.mouse_down(button); };
        _token_store += _mouse->mouse_up += [&](auto button) { _camera_input.mouse_up(button); };
        _token_store += _mouse->mouse_move += [&](long x, long y) { _camera_input.mouse_move(x, y); };
        _token_store += _camera_input.on_rotate += [&](float x, float y)
            {
                if (_mouse_over)
                {
                    const float low_sensitivity = 200.0f;
                    const float high_sensitivity = 25.0f;
                    const float sensitivity = low_sensitivity + (high_sensitivity - low_sensitivity);
                    _rotation += x / sensitivity;
                    _rotation_pitch += y / sensitivity;
                }
            };
    }

    void ModelsWindow::render()
    {
        if (!render_meshes_window())
        {
            on_window_closed();
            return;
        }
    }

    void ModelsWindow::set_level_texture_storage(const std::weak_ptr<ILevelTextureStorage>& level_texture_storage)
    {
        _transparency_buffer = _transparency_buffer_source(level_texture_storage);
        _selected_model.reset();
    }

    void ModelsWindow::set_model_storage(const std::weak_ptr<IModelStorage>& model_storage)
    {
        _model_storage = model_storage;
    }

    void ModelsWindow::set_number(int32_t number)
    {
        _id = std::format("Models {}", number);
    }

    void ModelsWindow::update(float)
    {
    }

    bool ModelsWindow::render_meshes_window()
    {
        _mouse_over = false;

        using namespace DirectX::SimpleMath;

        auto selected_model = _selected_model.lock();

        auto context = _device->context();
        context->OMSetDepthStencilState(_depth_stencil_state.Get(), 1);
        _render_target->clear(Color(0.2f, 0.2f, 0.2f));
        {
            context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
            _vertex_shader->apply(context);
            _pixel_shader->apply(context);

            graphics::RenderTargetStore rs_store(context);
            graphics::ViewportStore vp_store(context);
            _render_target->apply();

            if (selected_model)
            {
                _sampler_state->apply();

                const auto box = selected_model->bounding_box();
                const float ex = Vector3(box.Extents).Length();
                const auto camera_pos = Vector3(0, box.Center.y - 0.25f, ex * 3.5f);

                Matrix world = Matrix::Identity;
                Matrix camera_rotation = Matrix::CreateFromYawPitchRoll(_rotation, _rotation_pitch, 0);

                const auto eye = Vector3::Transform(camera_pos, camera_rotation);

                Matrix view = Matrix::CreateLookAt(eye, box.Center, Vector3(0, -1, 0));
                Matrix projection = Matrix::CreatePerspectiveFieldOfView(DirectX::XM_PIDIV4, 1.0f, 0.1f, 1000.0f);
                Matrix view_projection = view * projection;

                graphics::set_data(*_pixel_shader_data, context, PixelShaderData{ false });
                _pixel_shader_data->apply(context, graphics::IBuffer::ApplyTo::PS);

                selected_model->render(world, view_projection, Colour::White);

                _transparency_buffer->reset();
                selected_model->render_transparency(world, *_transparency_buffer, Colour::White);
                _transparency_buffer->sort(eye);

                graphics::set_data(*_pixel_shader_data, context, PixelShaderData{ false });
                _pixel_shader_data->apply(context, graphics::IBuffer::ApplyTo::PS);
                _transparency_buffer->render(view_projection);
            }
        }

        bool stay_open = true;
        ImGui::PushStyleVar(ImGuiStyleVar_WindowMinSize, ImVec2(540, 500));
        if (ImGui::Begin(_id.c_str(), &stay_open))
        {
            if (ImGui::BeginTable("Meshes List", 1, ImGuiTableFlags_ScrollY, ImVec2(100, 0)))
            {
                ImGui::TableSetupColumn("ID");
                ImGui::TableSetupScrollFreeze(1, 1);
                ImGui::TableHeadersRow();

                if (auto model_storage = _model_storage.lock())
                {
                    for (const auto model : model_storage->models())
                    {
                        ImGui::TableNextRow();
                        ImGui::TableNextColumn();

                        auto model_ptr = model.lock();
                        bool selected = model_ptr == selected_model;
                        if (ImGui::Selectable(std::format("{}", model_ptr->type_id()).c_str(), &selected, ImGuiSelectableFlags_SelectOnNav))
                        {
                            _selected_model = model;
                        }
                    }
                }
                ImGui::EndTable();
            }

            ImGui::SameLine();

            const auto texture = _render_target->texture();
            ImGui::Image(texture.view().Get(), ImVec2(texture.size().width, texture.size().height));
            _mouse_over = ImGui::GetIO().WantCaptureMouse && ImGui::IsItemHovered();
        }
        ImGui::End();
        ImGui::PopStyleVar();
        return stay_open;
    }

    void ModelsWindow::initialise()
    {
        messages::get_open_level(_messaging, weak_from_this());
    }

    void ModelsWindow::receive_message(const Message& message)
    {
        if (auto level = messages::read_open_level(message))
        {
            if (auto level_ptr = level->lock())
            {
                set_level_texture_storage(level_ptr->texture_storage());
                set_model_storage(level_ptr->model_storage());
            }
        }
    }

    std::string ModelsWindow::title() const
    {
        return _id;
    }

    std::string ModelsWindow::type() const
    {
        return "Models";
    }
}
