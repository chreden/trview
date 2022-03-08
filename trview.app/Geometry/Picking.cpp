#include "Picking.h"
#include <trview.app/Camera/ICamera.h>

#include <trview.common/Point.h>
#include <trview.common/Window.h>

using namespace DirectX;
using namespace DirectX::SimpleMath;

namespace trview
{
    IPicking::~IPicking()
    {
    }

    Picking::Picking(const Window& window)
        : _window(window)
    {
    }

    void Picking::pick(const ICamera& camera)
    {
        const auto window_size = _window.size();
        const Point mouse_pos = client_cursor_position(_window);
        if (mouse_pos.x < 0 || mouse_pos.x > window_size.width ||
            mouse_pos.y < 0 || mouse_pos.y > window_size.height)
        {
            PickResult empty;
            empty.stop = true;
            on_pick({}, empty);
            return;
        }

        Vector3 position = camera.position();
        const auto world = Matrix::CreateTranslation(position);
        const auto projection = camera.projection();
        const auto view = camera.view();

        Vector3 direction = XMVector3Unproject(Vector3(mouse_pos.x, mouse_pos.y, 1), 0, 0,
            window_size.width, window_size.height, 0, 1.0f, projection, view, world);
        direction.Normalize();

        position += XMVector3Unproject(Vector3(mouse_pos.x, mouse_pos.y, 0.1f), 0, 0,
            window_size.width, window_size.height, 0.1f, 10000.0f, projection, view, world);

        // Call the registered pickers.
        PickInfo info{ camera.view_size(), mouse_pos, position, direction };
        PickResult result{};
        pick_sources(info, result);

        on_pick(info, result);
    }
}
