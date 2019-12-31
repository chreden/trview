#include "Compass.h"
#include <trview.app/Camera/ICamera.h>
#include <trview.app/Geometry/MeshVertex.h>
#include <trview.graphics/Sprite.h>
#include <trview.graphics/IShaderStorage.h>
#include <trview.graphics/RenderTargetStore.h>
#include <trview.graphics/ViewportStore.h>

namespace trview
{
    using namespace graphics;
    using namespace DirectX;
    using namespace DirectX::SimpleMath;

    namespace
    {
        const float HalfPi = 1.5707963267948966192313216916398f;
        const float Pi = 3.1415926535897932384626433832796f;
        const float View_Size = 200;
        const float Nodule_Size = 0.05f;

        const std::unordered_map<Compass::Axis, std::wstring> axis_type_names
        {
            { Compass::Axis::Pos_X, L"+X" },
            { Compass::Axis::Pos_Y, L"+Y" },
            { Compass::Axis::Pos_Z, L"+Z" },
            { Compass::Axis::Neg_X, L"-X" },
            { Compass::Axis::Neg_Y, L"-Y" },
            { Compass::Axis::Neg_Z, L"-Z" },
        };

        const std::unordered_map<Compass::Axis, BoundingBox> nodule_boxes
        {
            { Compass::Axis::Pos_X, BoundingBox(Vector3(0.5f, 0, 0), Vector3(Nodule_Size)) },
            { Compass::Axis::Pos_Y, BoundingBox(Vector3(0, 0.5f, 0), Vector3(Nodule_Size)) },
            { Compass::Axis::Pos_Z, BoundingBox(Vector3(0, 0, 0.5f), Vector3(Nodule_Size)) },
            { Compass::Axis::Neg_X, BoundingBox(Vector3(-0.5f, 0, 0), Vector3(Nodule_Size)) },
            { Compass::Axis::Neg_Y, BoundingBox(Vector3(0, -0.5f, 0), Vector3(Nodule_Size)) },
            { Compass::Axis::Neg_Z, BoundingBox(Vector3(0, 0, -0.5f), Vector3(Nodule_Size)) },
        };
    }

    Compass::Compass(const graphics::Device& device, const IShaderStorage& shader_storage)
        : _mesh_camera(Size(View_Size, View_Size)),
          _mesh(create_cube_mesh(device)),
          _sprite(std::make_unique<Sprite>(device, shader_storage, Size(View_Size, View_Size))),
          _render_target(std::make_unique<RenderTarget>(device, static_cast<uint32_t>(View_Size), static_cast<uint32_t>(View_Size), RenderTarget::DepthStencilMode::Enabled))
    {
    }

    void Compass::render(const Device& device, const ICamera& camera, const ILevelTextureStorage& texture_storage)
    {
        if (!_visible)
        {
            return;
        }

        auto context = device.context();

        {
            RenderTargetStore rs_store(context);
            ViewportStore vp_store(context);
            _render_target->apply(context);
            _render_target->clear(context, Color(0.0f, 0.0f, 0.0f, 0.0f));

            // Have a camera that looks at the compass and match rotation to the real camera
            _mesh_camera.set_target(Vector3::Zero);
            _mesh_camera.set_zoom(2.0f);
            _mesh_camera.set_rotation_pitch(camera.rotation_pitch());
            _mesh_camera.set_rotation_yaw(camera.rotation_yaw());

            const auto view_projection = _mesh_camera.view_projection();
            const float thickness = 0.015f;
            const auto scale = Matrix::CreateScale(thickness, 1.0f, thickness);

            _mesh->render(context, scale * view_projection, texture_storage, Color(1.0f, 0.0f, 0.0f));
            _mesh->render(context, scale * Matrix::CreateRotationZ(HalfPi) * view_projection, texture_storage, Color(0.0f, 1.0f, 0.0f));
            _mesh->render(context, scale * Matrix::CreateRotationX(HalfPi) * view_projection, texture_storage, Color(0.0f, 0.0f, 1.0f));

            // Nodules for each direction - they can be clicked.
            const auto nodule_scale = Matrix::CreateScale(0.05f);
            // Y
            _mesh->render(context, nodule_scale * Matrix::CreateTranslation(0, 0.5f, 0) * view_projection, texture_storage, Color(1.0f, 0.0f, 0.0f));
            _mesh->render(context, nodule_scale * Matrix::CreateTranslation(0, -0.5f, 0) * view_projection, texture_storage, Color(1.0f, 0.0f, 0.0f));
            // X
            _mesh->render(context, nodule_scale * Matrix::CreateTranslation(0.5f, 0, 0) * view_projection, texture_storage, Color(0.0f, 1.0f, 0.0f));
            _mesh->render(context, nodule_scale * Matrix::CreateTranslation(-0.5f, 0, 0) * view_projection, texture_storage, Color(0.0f, 1.0f, 0.0f));
            // Z
            _mesh->render(context, nodule_scale * Matrix::CreateTranslation(0, 0, 0.5f) * view_projection, texture_storage, Color(0.0f, 0.0f, 1.0f));
            _mesh->render(context, nodule_scale * Matrix::CreateTranslation(0, 0, -0.5f) * view_projection, texture_storage, Color(0.0f, 0.0f, 1.0f));
        }

        auto screen_size = camera.view_size();
        _sprite->set_host_size(screen_size);
        _sprite->render(context, _render_target->texture(), screen_size.width - View_Size, screen_size.height - View_Size, View_Size, View_Size);
    }

    bool Compass::pick(const Point& mouse_position, const Size& screen_size, Axis& axis)
    {
        if (!_visible)
        {
            return false;
        }

        // Convert the mouse position into coordinates of the compass window.
        const auto view_top_left = Point(screen_size.width - View_Size, screen_size.height - View_Size);
        const auto view_pos = mouse_position - view_top_left;
        const auto view_size = Size(View_Size, View_Size);

        if (!view_pos.is_between(Point(), Point(View_Size, View_Size)))
        {
            return false;
        }

        const Vector3 position = _mesh_camera.position();
        auto world = Matrix::CreateTranslation(position);

        Vector3 direction = XMVector3Unproject(Vector3(view_pos.x, view_pos.y, 1), 0, 0, view_size.width, view_size.height, 0, 1.0f, _mesh_camera.projection(), _mesh_camera.view(), world);
        direction.Normalize();

        bool hit = false;
        float nearest = 0;
        for (const auto& box : nodule_boxes)
        {
            float box_distance = 0;
            if (box.second.Intersects(position, direction, box_distance) && (!hit || box_distance < nearest))
            {
                nearest = box_distance;
                axis = box.first;
                hit = true;
            }
        }

        return hit;
    }

    void Compass::set_visible(bool value)
    {
        _visible = value;
    }

    std::wstring axis_name(Compass::Axis axis)
    {
        auto name = axis_type_names.find(axis);
        if (name == axis_type_names.end())
        {
            return L"Unknown";
        }
        return name->second;
    }

    void align_camera_to_axis(ICamera& camera, Compass::Axis axis)
    {
        float yaw = camera.rotation_yaw();
        float pitch = camera.rotation_pitch();

        switch (axis)
        {
        case Compass::Axis::Pos_X:
            yaw = HalfPi;
            pitch = 0;
            break;
        case Compass::Axis::Pos_Y:
            pitch = -HalfPi;
            break;
        case Compass::Axis::Pos_Z:
            yaw = 0;
            pitch = 0;
            break;
        case Compass::Axis::Neg_X:
            yaw = -HalfPi;
            pitch = 0;
            break;
        case Compass::Axis::Neg_Y:
            pitch = HalfPi;
            break;
        case Compass::Axis::Neg_Z:
            yaw = Pi;
            pitch = 0;
            break;
        }

        camera.rotate_to_yaw(yaw);
        camera.rotate_to_pitch(pitch);
    }
}
