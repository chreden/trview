#include "Picking.h"
#include "ICamera.h"

#include <trview.common/Point.h>
#include <trview.common/Window.h>
#include <trview.ui/Label.h>
#include <SimpleMath.h>

using namespace DirectX;
using namespace DirectX::SimpleMath;

namespace trview
{
    Picking::Picking(ui::Control& parent)
    {
        auto picking = std::make_unique<ui::Label>(Point(500, 0), Size(38, 30), Colour(0.2f, 0.2f, 0.2f), L"0", 8, graphics::TextAlignment::Centre, graphics::ParagraphAlignment::Centre);
        picking->set_visible(false);
        picking->set_handles_input(false);
        _label = parent.add_child(std::move(picking));
    }

    void Picking::pick(const Window& window, const ICamera& camera)
    {
        const Vector3 position = camera.position();
        const auto world = Matrix::CreateTranslation(position);
        const auto projection = camera.projection();
        const auto view = camera.view();
        const auto window_size = window.size();

        const Point mouse_pos = client_cursor_position(window);
        Vector3 direction = XMVector3Unproject(Vector3(mouse_pos.x, mouse_pos.y, 1), 0, 0,
            window_size.width, window_size.height, 0, 1.0f, projection, view, world);
        direction.Normalize();

        // Call the registered pickers.
        PickInfo info{ camera.view_size(), mouse_pos, position, direction };
        PickResult result{};
        on_pick(info, result);

        // Show the tooltip.
        _label->set_visible(result.hit && _show);
        if (result.hit)
        {
            Vector3 screen_pos = XMVector3Project(result.position, 0, 0, window_size.width, window_size.height, 0, 1.0f, projection, view, XMMatrixIdentity());
            _label->set_position(Point(screen_pos.x - _label->size().width, screen_pos.y - _label->size().height));
            _label->set_text(pick_to_string(result));
            _label->set_text_colour(result.type == PickResult::Type::Room ? Colour(1.0f, 1.0f, 1.0f) : result.type == PickResult::Type::Trigger ? Colour(1.0f, 0.0f, 1.0f) : Colour(0.0f, 1.0f, 0.0f));
        }
    }
}
