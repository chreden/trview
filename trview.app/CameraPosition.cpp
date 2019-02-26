#include "CameraPosition.h"
#include <trlevel/trlevel.h>

using namespace trview::ui;
using namespace trview::graphics;
using namespace DirectX::SimpleMath;

namespace trview
{
    namespace
    {
        std::wstring convert_number(float value)
        {
            return (value >= 0 ? L" " : L"") + std::to_wstring(value);
        }
    }

    CameraPosition::CameraPosition(Control& parent)
    {
        auto display = std::make_unique<StackPanel>(Point(10, parent.size().height - 90), Size(200, 90), Colour(0.5f, 0.0f, 0.0f, 0.0f));
        display->set_margin(Size(5, 5));

        _x = display->add_child(std::make_unique<Label>(Point(), Size(100, 20), Colour::Transparent, L"X coordinate", 8));
        _y = display->add_child(std::make_unique<Label>(Point(), Size(100, 20), Colour::Transparent, L"Y coordinate", 8));
        _z = display->add_child(std::make_unique<Label>(Point(), Size(100, 20), Colour::Transparent, L"Z coordinate", 8));
        _display = parent.add_child(std::move(display));

        auto update_position = [&](Size size)
        {
            _display->set_position(Point(_display->position().x, size.height - 10 - _display->size().height));
        };

        _token_store += parent.on_size_changed += update_position;
        update_position(parent.size());
    }

    void CameraPosition::set_position(const DirectX::SimpleMath::Vector3& position)
    {
        _x->set_text(L"X: " + convert_number(position.x * trlevel::Scale_X));
        _y->set_text(L"Y: " + convert_number(position.y * trlevel::Scale_Y));
        _z->set_text(L"Z: " + convert_number(position.z * trlevel::Scale_Z));
    }
}
