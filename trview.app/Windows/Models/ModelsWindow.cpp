#include "ModelsWindow.h"
#include "../../Graphics/IMeshStorage.h"
#include "../../Geometry/IMesh.h"

#include <trview.graphics/RenderTargetStore.h>
#include <trview.graphics/ViewportStore.h>

namespace trview
{
    IModelsWindow::~IModelsWindow()
    {
    }

    ModelsWindow::ModelsWindow(const std::shared_ptr<graphics::IDevice>& device, const graphics::IRenderTarget::SizeSource& render_target_source, const std::shared_ptr<graphics::IShaderStorage>& shader_storage)
        : _device(device), _render_target(render_target_source(512, 512, graphics::IRenderTarget::DepthStencilMode::Enabled))
    {
        _vertex_shader = shader_storage->get("level_vertex_shader");
        _pixel_shader = shader_storage->get("level_pixel_shader");

        D3D11_DEPTH_STENCIL_DESC ui_depth_stencil_desc;
        memset(&ui_depth_stencil_desc, 0, sizeof(ui_depth_stencil_desc));
        _depth_stencil_state = device->create_depth_stencil_state(ui_depth_stencil_desc);
    }

    void ModelsWindow::render()
    {
        if (!render_meshes_window())
        {
            on_window_closed();
            return;
        }
    }

    void ModelsWindow::set_model_storage(const std::weak_ptr<IModelStorage>& model_storage)
    {
        _model_storage = model_storage;
    }

    void ModelsWindow::set_number(int32_t number)
    {
        _id = std::format("Models {}", number);
    }

    void ModelsWindow::update(float delta)
    {
        delta;
    }

    bool ModelsWindow::render_meshes_window()
    {
        using namespace DirectX::SimpleMath;
        // lemoa

        auto selected_mesh = _selected_mesh.lock();

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

            if (selected_mesh)
            {
                Matrix world = Matrix::Identity;
                Matrix view = Matrix::CreateLookAt(Vector3(0, 0, -3), Vector3(), Vector3(0, 1, 0));
                Matrix projection = Matrix::CreatePerspectiveFieldOfView(DirectX::XM_PIDIV4, 1.0f, 0.1f, 1000.0f);
                Matrix transform = world * view * projection;
                selected_mesh->render(transform, Colour::White);
            }
        }
        // render_internal(context);
        // _force_redraw = false;

        bool stay_open = true;
        ImGui::PushStyleVar(ImGuiStyleVar_WindowMinSize, ImVec2(540, 500));
        if (ImGui::Begin(_id.c_str(), &stay_open))
        {
            if (ImGui::BeginTable("Meshes List", 1, ImGuiTableFlags_ScrollY, ImVec2(100, 0)))
            {
                ImGui::TableSetupColumn("ID");
                ImGui::TableSetupScrollFreeze(1, 1);
                ImGui::TableHeadersRow();

                if (auto model_Storage = _model_storage.lock())
                {
                    /*
                    for (const auto model : _model_storage->models())
                    {
                        ImGui::TableNextRow();
                        ImGui::TableNextColumn();

                        auto mesh_ptr = mesh.second.lock();
                        bool selected = mesh_ptr == selected_mesh;
                        if (ImGui::Selectable(std::format("{}", mesh.first).c_str(), &selected, ImGuiSelectableFlags_SelectOnNav))
                        {
                            _selected_mesh = mesh.second;
                        }
                    }
                    */
                }
                ImGui::EndTable();
            }

            ImGui::SameLine();

            const auto texture = _render_target->texture();
            ImGui::Image(texture.view().Get(), ImVec2(texture.size().width, texture.size().height));
        }
        ImGui::End();
        ImGui::PopStyleVar();
        return stay_open;
    }
}
