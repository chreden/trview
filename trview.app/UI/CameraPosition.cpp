#include "CameraPosition.h"
#include <trlevel/trlevel.h>
#include <trview.ui/Label.h>
#include <sstream>

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

        auto x_line = std::make_unique<StackPanel>(Point(), Size(100, 20), Colour::Transparent, Size(), StackPanel::Direction::Horizontal);
        x_line->add_child(std::make_unique<Label>(Point(), Size(10, 20), Colour::Transparent, L"X: ", 8));
        _x = x_line->add_child(std::make_unique<TextArea>(Point(), Size(80, 20), Colour::Transparent, Colour::White));
        _x->set_text(L"X coordinate");
        _x->set_mode(TextArea::Mode::SingleLine);
        _token_store += _x->on_selected += [&]() { _x->set_text(L""); };
        display->add_child(std::move(x_line));

        auto y_line = std::make_unique<StackPanel>(Point(), Size(100, 20), Colour::Transparent, Size(), StackPanel::Direction::Horizontal);
        y_line->add_child(std::make_unique<Label>(Point(), Size(10, 20), Colour::Transparent, L"Y: ", 8));
        _y = y_line->add_child(std::make_unique<TextArea>(Point(), Size(80, 20), Colour::Transparent, Colour::White));
        _y->set_text(L"Y coordinate");
        _y->set_mode(TextArea::Mode::SingleLine);
        _token_store += _y->on_selected += [&]() { _y->set_text(L""); };
        display->add_child(std::move(y_line));

        auto z_line = std::make_unique<StackPanel>(Point(), Size(100, 20), Colour::Transparent, Size(), StackPanel::Direction::Horizontal);
        z_line->add_child(std::make_unique<Label>(Point(), Size(10, 20), Colour::Transparent, L"Z: ", 8));
        _z = z_line->add_child(std::make_unique<TextArea>(Point(), Size(80, 20), Colour::Transparent, Colour::White));
        _z->set_text(L"Z coordinate");
        _z->set_mode(TextArea::Mode::SingleLine);
        _token_store += _z->on_selected += [&]() { _z->set_text(L""); };
        display->add_child(std::move(z_line));

        _display = parent.add_child(std::move(display));

        auto update_position = [&](Size size)
        {
            _display->set_position(Point(_display->position().x, size.height - 10 - _display->size().height));
        };

        _token_store += parent.on_size_changed += update_position;
        update_position(parent.size());

        // Bind manual camera position entry controls.
        _token_store += _x->on_enter += [&](const std::wstring& text) 
        {
            std::wstringstream stream;
            stream << text;
            float coordinate = 0;
            stream >> coordinate;

            _x->on_focus_clear_requested();

            on_x(coordinate / trlevel::Scale_X);
        };
        _token_store += _y->on_enter += [&](const std::wstring& text)
        {
            _y->on_focus_clear_requested(); 
        };
        _token_store += _z->on_enter += [&](const std::wstring& text)
        {
            _z->on_focus_clear_requested(); 
        };
    }

    void CameraPosition::set_position(const DirectX::SimpleMath::Vector3& position)
    {
        if (!_x->focused())
        {
            _x->set_text(convert_number(position.x * trlevel::Scale_X));
        }
        if (!_y->focused())
        {
            _y->set_text(convert_number(position.y * trlevel::Scale_Y));
        }
        if (!_z->focused())
        {
            _z->set_text(convert_number(position.z * trlevel::Scale_Z));
        }
    }
}
