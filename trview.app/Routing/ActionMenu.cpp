#define NOMINMAX
#include "ActionMenu.h"
#include <trview.app/Geometry/TransparencyBuffer.h>
#include <trview.app/Camera/ICamera.h>
#include <algorithm>
#include "Action.h"

using namespace trview::graphics;
using namespace DirectX::SimpleMath;
using namespace DirectX;

namespace trview
{
    ActionMenu::ActionMenu()
    {
        generate_geometry();
    }

    void ActionMenu::generate_geometry()
    {
        float u = 0.0f;
        float v = 0.0f;
        float width = 1.0f;
        float height = 1.0f;

        // Generate quad.
        using namespace DirectX::SimpleMath;
        std::vector<MeshVertex> vertices
        {
            { Vector3(-0.5f, -0.5f, 0), Vector3::Zero, Vector2(u, v + height), Color(1,1,1,1) },
            { Vector3(0.5f, -0.5f, 0), Vector3::Zero, Vector2(u + width, v + height), Color(1,1,1,1) },
            { Vector3(-0.5f, 0.5f, 0), Vector3::Zero, Vector2(u, v), Color(1,1,1,1) },
            { Vector3(0.5f, 0.5f, 0), Vector3::Zero, Vector2(u + width, v), Color(1,1,1,1) },
        };

        std::vector<TransparentTriangle> transparent_triangles
        {
            { vertices[0].pos, vertices[1].pos, vertices[2].pos, vertices[0].uv, vertices[1].uv, vertices[2].uv, TransparentTriangle::Untextured, TransparentTriangle::Mode::Normal },
            { vertices[2].pos, vertices[1].pos, vertices[3].pos, vertices[2].uv, vertices[1].uv, vertices[3].uv, TransparentTriangle::Untextured, TransparentTriangle::Mode::Normal },
        };

        _action = std::make_unique<ActionNodeOption>(transparent_triangles);
    }

    void ActionMenu::render(const Device& device, const ICamera& camera, TransparencyBuffer& transparency_buffer, const Vector3& previous_waypoint, const Vector3& next_waypoint, bool selected)
    {
        // Render the action sprite above the midpoint of the path.
        // Figure out which way the route is going....
        auto to = next_waypoint - previous_waypoint;
        to.Normalize();
        to *= 0.1f;

        const auto mid = Vector3::Lerp(previous_waypoint, next_waypoint, 0.5f);

        Vector3 first = XMVector3Project(previous_waypoint, 0, 0, camera.view_size().width, camera.view_size().height, 0, 1.0f, camera.projection(), camera.view(), Matrix::Identity);
        Vector3 second = XMVector3Project(previous_waypoint + to, 0, 0, camera.view_size().width, camera.view_size().height, 0, 1.0f, camera.projection(), camera.view(), Matrix::Identity);
        bool flip = first.x < second.x;
        float scale = 0.35f;

        Vector3 forward = camera.forward();
        auto scaling = Matrix::CreateScale(scale * (flip ? -1.0f : 1.0f), scale, scale);
        auto billboard = Matrix::CreateBillboard(mid, camera.rendering_position(), camera.up(), &forward) * Matrix::CreateTranslation(0, -0.1f, 0);
        auto world = scaling * billboard;
        _action->render(world, transparency_buffer, Action::Run);

        const float spacing_scale = 0.01f;
        const float expansion_scale = 75.0f;

        if (selected)
        {
            const std::vector<Action> all_actions { Action::Run, Action::Walk, Action::Sprint };

            auto page_size = 8;
            auto pages = all_actions.size() / page_size + 1;
            auto current_page = 0;
            auto base = current_page * page_size;
            auto on_page = std::min<uint32_t>(8, all_actions.size() - base);
            
            for (int i = 0; i < on_page; ++i)
            {
                float scale2 = 0.2f;
                auto scaling2 = Matrix::CreateScale(scale2 * (flip ? -1.0f : 1.0f), scale2, scale2);

                // Render another one...
                float angle = DirectX::g_XMTwoPi[0] / -static_cast<float>(on_page) * static_cast<float>(i);
                auto rotation = Matrix::CreateRotationZ(angle);

                auto transform = rotation * camera.view().Invert();

                float length = std::min(0.4f, _time * 3.0f);
                auto offset = Vector3::TransformNormal(Vector3(0, length, 0), transform);

                auto subpos = mid + offset + Vector3(0, -0.1f, 0);
                _sub_nodes[i] = subpos;

                auto billboard2 = Matrix::CreateBillboard(subpos, camera.rendering_position(), camera.up(), &forward);
                auto world2 = scaling2 * billboard2;
                _action->render(world2, transparency_buffer, all_actions[base + i]);
            }
        }
    }

    void ActionMenu::select()
    {
        _time = 0.0f;
    }

    void ActionMenu::update(float elapsed)
    {
        _time += elapsed;
    }

    PickResult ActionMenu::pick(const Vector3& position, const Vector3& direction) const
    {
        PickResult result;
        result.hit = false;
        float distance = 0;

        for (int i = 0; i < 8; ++i)
        {
            auto subsphere = BoundingSphere(_sub_nodes[i], 0.1f);
            if (subsphere.Intersects(position, direction, distance) && (!result.hit || distance < result.distance))
            {
                result.distance = distance;
                result.hit = true;
                result.index = i;
                result.position = _sub_nodes[i];
                result.type = PickResult::Type::ActionSubNode;
            }
        }

        return result;
    }
}