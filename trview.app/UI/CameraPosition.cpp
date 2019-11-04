#include "CameraPosition.h"
#include <trlevel/trlevel.h>
#include <trview.ui/Label.h>
#include <trview.common/Strings.h>

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

        _x = create_coordinate_entry(*display, _position.x, L"X");
        _y = create_coordinate_entry(*display, _position.y, L"Y");
        _z = create_coordinate_entry(*display, _position.z, L"Z");
        _display = parent.add_child(std::move(display));

        auto update_position = [&](Size size)
        {
            _display->set_position(Point(_display->position().x, size.height - 10 - _display->size().height));
        };

        _token_store += parent.on_size_changed += update_position;
        update_position(parent.size());

        // Bind manual camera position entry controls.
        _token_store += _x->on_tab += [&](const std::wstring& text) 
        { 
            update_coordinate(_position.x, text);
            _x->on_focus_clear_requested();
            _y->on_focus_requested(); 
        };
        _token_store += _y->on_tab += [&](const std::wstring& text)
        {
            update_coordinate(_position.y, text);
            _y->on_focus_clear_requested();
            _z->on_focus_requested();
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

    TextArea* CameraPosition::create_coordinate_entry(Control& parent, float& coordinate, const std::wstring& name)
    {
        auto line = std::make_unique<StackPanel>(Size(100, 20), Colour::Transparent, Size(), StackPanel::Direction::Horizontal);
        auto line_area = std::make_unique<StackPanel>(Size(10, 20), Colour::Transparent);
        line_area->add_child(std::make_unique<Window>(Point(), Size(10, 1), Colour::Transparent));
        line_area->add_child(std::make_unique<Label>(Point(), Size(10, 20), Colour::Transparent, name + L": ", 8));
        line->add_child(std::move(line_area));
        auto entry = line->add_child(std::make_unique<TextArea>(Point(), Size(80, 20), Colour::Transparent, Colour::White));
        entry->set_mode(TextArea::Mode::SingleLine);
        entry->set_text(convert_number(0));
        entry->set_name(to_utf8(name));
        _token_store += entry->on_focused += [=]() { entry->set_text(L""); };
        _token_store += entry->on_escape += [=]()
        {
            entry->on_focus_clear_requested();
            entry->set_text(convert_number(coordinate)); 
        };
        _token_store += entry->on_enter += [this, &coordinate, entry](const std::wstring& text)
        {
            update_coordinate(coordinate, text);
            entry->on_focus_clear_requested();
        };
        parent.add_child(std::move(line));
        return entry;
    }

    void CameraPosition::update_coordinate(float& coordinate, const std::wstring& text)
    {
        try
        {
            coordinate = std::stof(text) / trlevel::Scale_X;
            on_position_changed(_position);
        }
        catch (...)
        {
            // Conversion failed.
        }
    }
}
