#define NOMINMAX
#include "Route.h"
#include <DirectXCollision.h>
#include <algorithm>
#include "ICamera.h"
#include "ILevelTextureStorage.h"

using namespace DirectX;
using namespace DirectX::SimpleMath;

namespace trview
{
    namespace
    {
        const float PoleThickness = 0.05f;
        const float RopeThickness = 0.015f;
    }

    Route::Route(const graphics::Device& device, const graphics::IShaderStorage& shader_storage)
        : _waypoint_mesh(create_cube_mesh(device.device())), _selection_renderer(device, shader_storage)
    {
    }

    void Route::add(const Vector3& position, uint32_t room)
    {
        add(position, room, Waypoint::Type::Position, 0u);
    }

    void Route::add(const DirectX::SimpleMath::Vector3& position, uint32_t room, Waypoint::Type type, uint32_t type_index)
    {
        _waypoints.emplace_back(_waypoint_mesh.get(), position, room, type, type_index);
    }

    void Route::clear()
    {
        _waypoints.clear();
        _selected_index = 0u;
    }

    void Route::insert(const DirectX::SimpleMath::Vector3& position, uint32_t room, uint32_t index)
    {
        if (index >= _waypoints.size())
        {
            return add(position, room, Waypoint::Type::Position, 0u);
        }
        insert(position, room, index, Waypoint::Type::Position, 0u);
    }

    void Route::insert(const DirectX::SimpleMath::Vector3& position, uint32_t room, uint32_t index, Waypoint::Type type, uint32_t type_index)
    {
        _waypoints.insert(_waypoints.begin() + index, Waypoint(_waypoint_mesh.get(), position, room, type, type_index));
    }

    PickResult Route::pick(const Vector3& position, const Vector3& direction) const
    {
        PickResult result;
        result.hit = false;

        for (uint32_t i = 0; i < _waypoints.size(); ++i)
        {
            const auto& waypoint = _waypoints[i];

            auto box = BoundingBox(waypoint.position() - Vector3(0, 0.25f, 0), Vector3(PoleThickness, 0.5f, PoleThickness) * 0.5f);

            float distance = 0;
            if (box.Intersects(position, direction, distance) && (!result.hit || distance < result.distance))
            {
                result.distance = distance;
                result.hit = true;
                result.index = i;
                result.position = position + direction * distance;
                result.type = PickResult::Type::Waypoint;
            }
        }

        return result;
    }

    void Route::remove(uint32_t index)
    {
        if (index >= _waypoints.size())
        {
            return;
        }
        _waypoints.erase(_waypoints.begin() + index);
        if (_selected_index > index)
        {
            --_selected_index;
        }
    }

    void Route::render(const graphics::Device& device, const ICamera& camera, const ILevelTextureStorage& texture_storage)
    {
        for (std::size_t i = 0; i < _waypoints.size(); ++i)
        {
            auto& waypoint = _waypoints[i];
            waypoint.render(device, camera, texture_storage, Color(1.0f, 0.0f, 1.0f));

            // Should render the in-between line somehow - if there is another point in the list.
            if (i < _waypoints.size() - 1)
            {
                const auto current = waypoint.position() - Vector3(0, 0.5f, 0);
                const auto next_waypoint = _waypoints[i + 1].position() - Vector3(0, 0.5f, 0);
                const auto mid = Vector3::Lerp(current, next_waypoint, 0.5f);
                const auto matrix = Matrix(DirectX::XMMatrixLookAtRH(mid, next_waypoint, Vector3::Up)).Invert();
                const auto length = (next_waypoint - current).Length();
                const auto to_wvp = Matrix::CreateScale(RopeThickness, RopeThickness, length) * matrix * camera.view_projection();
                _waypoint_mesh->render(device.context(), to_wvp, texture_storage, Color(0.0f, 1.0f, 0.0f));
            }
        }

        // Render selected waypoint...
        if (_selected_index < _waypoints.size())
        {
            _selection_renderer.render(device, camera, texture_storage, _waypoints[_selected_index], Color(1.0f, 1.0f, 1.0f));
        }
    }

    uint32_t Route::selected_waypoint() const
    {
        return _selected_index;
    }

    void Route::select_waypoint(uint32_t index)
    {
        _selected_index = index;
    }

    const Waypoint& Route::waypoint(uint32_t index) const
    {
        if (index < _waypoints.size())
        {
            return _waypoints[index];
        }
        throw std::range_error("Waypoint index out of range");
    }

    Waypoint& Route::waypoint(uint32_t index)
    {
        if (index < _waypoints.size())
        {
            return _waypoints[index];
        }
        throw std::range_error("Waypoint index out of range");
    }

    uint32_t Route::waypoints() const
    {
        return _waypoints.size();
    }
}
