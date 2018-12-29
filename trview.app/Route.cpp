#include "Route.h"
#include <DirectXCollision.h>
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

    Route::Route(const graphics::Device& device)
        : _waypoint_mesh(create_cube_mesh(device.device()))
    {
    }

    void Route::add(const Vector3& position)
    {
        _waypoints.push_back(position);
    }

    PickResult Route::pick(const Vector3& position, const Vector3& direction) const
    {
        PickResult result;
        result.hit = false;

        for (uint32_t i = 0; i < _waypoints.size(); ++i)
        {
            const auto waypoint = _waypoints[i];

            auto box = BoundingBox(waypoint - Vector3(0, 0.25f, 0), Vector3(PoleThickness, 0.5f, PoleThickness) * 0.5f);

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
    }

    void Route::render(const graphics::Device& device, const ICamera& camera, const ILevelTextureStorage& texture_storage)
    {
        for (std::size_t i = 0; i < _waypoints.size(); ++i)
        {
            // Render the pole.
            using namespace DirectX::SimpleMath;
            const auto waypoint = _waypoints[i];

            auto wvp = Matrix::CreateScale(PoleThickness, 0.5f, PoleThickness) * Matrix::CreateTranslation(waypoint - Vector3(0, 0.25f, 0)) * camera.view_projection();
            _waypoint_mesh->render(device.context(), wvp, texture_storage, Color(1.0f, 0.0f, 1.0f));

            // Should render the in-between line somehow - if there is another point in the list.
            if (i < _waypoints.size() - 1)
            {
                const auto current = waypoint - Vector3(0, 0.5f, 0);
                const auto next_waypoint = _waypoints[i + 1] - Vector3(0, 0.5f, 0);
                const auto mid = Vector3::Lerp(current, next_waypoint, 0.5f);
                const auto matrix = Matrix(DirectX::XMMatrixLookAtRH(mid, next_waypoint, Vector3::Up)).Invert();
                const auto length = (next_waypoint - current).Length();
                const auto to_wvp = Matrix::CreateScale(RopeThickness, RopeThickness, length) * matrix * camera.view_projection();
                _waypoint_mesh->render(device.context(), to_wvp, texture_storage, Color(0.0f, 1.0f, 0.0f));
            }
        }
    }
}
