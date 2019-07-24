#include "CameraPosition.h"
#include <trlevel/trlevel.h>
#include <trview.ui/Label.h>

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

        ui::TextArea* create_coordinate_entry(TokenStore& token_store, Control& parent, const std::wstring& name)
        {
            auto line = std::make_unique<StackPanel>(Point(), Size(100, 20), Colour::Transparent, Size(), StackPanel::Direction::Horizontal);
            line->add_child(std::make_unique<Label>(Point(), Size(10, 20), Colour::Transparent, name + L": ", 8));
            auto entry = line->add_child(std::make_unique<TextArea>(Point(), Size(80, 20), Colour::Transparent, Colour::White));
            entry->set_mode(TextArea::Mode::SingleLine);
            token_store += entry->on_selected += [=]() { entry->set_text(L""); };
            parent.add_child(std::move(line));
            return entry;
        }
    }

    CameraPosition::CameraPosition(Control& parent)
    {
        auto display = std::make_unique<StackPanel>(Point(10, parent.size().height - 90), Size(200, 90), Colour(0.5f, 0.0f, 0.0f, 0.0f));
        display->set_margin(Size(5, 5));

        _x = create_coordinate_entry(_token_store, *display, L"X");
        _y = create_coordinate_entry(_token_store, *display, L"Y");
        _z = create_coordinate_entry(_token_store, *display, L"Z");
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
            try
            {
                _position.x = std::stof(text) / trlevel::Scale_X;
                on_position_changed(_position);
            }
            catch (...)
            {
                // Conversion failed.
            }
            _x->on_focus_clear_requested();
        };
        _token_store += _y->on_enter += [&](const std::wstring& text)
        {
            try
            {
                _position.y = std::stof(text) / trlevel::Scale_Y;
                on_position_changed(_position);
            }
            catch (...)
            {
                // Conversion failed.
            }
            _y->on_focus_clear_requested(); 
        };
        _token_store += _z->on_enter += [&](const std::wstring& text)
        {
            try
            {
                _position.z = std::stof(text) / trlevel::Scale_Z;
                on_position_changed(_position);
            }
            catch (...)
            {
                // Conversion failed.
            }
            _z->on_focus_clear_requested(); 
        };
    }

    void CameraPosition::set_position(const DirectX::SimpleMath::Vector3& position)
    {
        _position = position;

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
