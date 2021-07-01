#include "CameraPosition.h"
#include <trlevel/ILevel.h>
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
        _position_display = parent.add_child(std::make_unique<StackPanel>(Point(10, parent.size().height - 90), Size(200, 90), Colour(0.5f, 0.0f, 0.0f, 0.0f)));
        _position_display->set_margin(Size(5, 5));

        _x = create_coordinate_entry(*_position_display, _position.x, L"X");
        _y = create_coordinate_entry(*_position_display, _position.y, L"Y");
        _z = create_coordinate_entry(*_position_display, _position.z, L"Z");

        _rotation_display = parent.add_child(std::make_unique<StackPanel>(Point(10, parent.size().height - 90 - 5 - 60), Size(200, 60), Colour(0.5f, 0.0f, 0.0f, 0.0f)));
        _rotation_display->set_margin(Size(5, 5));
        _yaw = create_coordinate_entry(*_rotation_display, _rotation_yaw, L"Yaw", true);
        _pitch = create_coordinate_entry(*_rotation_display, _rotation_pitch, L"Pitch", true);

        auto update_position = [&](Size size)
        {
            _position_display->set_position(Point(_position_display->position().x, size.height - 10 - _position_display->size().height));
            _rotation_display->set_position(Point(_position_display->position().x, size.height - 10 - _position_display->size().height - 5 - _rotation_display->size().height));
        };

        _token_store += parent.on_size_changed += update_position;
        update_position(parent.size());

        // Bind manual camera position entry controls.
        _token_store += _x->on_tab += [&](const std::wstring& text) 
        { 
            update_position_coordinate(_position.x, text);
            _x->on_focus_clear_requested();
            _y->on_focus_requested(); 
        };
        _token_store += _y->on_tab += [&](const std::wstring& text)
        {
            update_position_coordinate(_position.y, text);
            _y->on_focus_clear_requested();
            _z->on_focus_requested();
        };
        _token_store += _yaw->on_tab += [&](const std::wstring& text)
        {
            update_rotation_coordinate(_rotation_yaw, text);
            _yaw->on_focus_clear_requested();
            _pitch->on_focus_requested();
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

    void CameraPosition::set_rotation(float yaw, float pitch)
    {
        _rotation_yaw = yaw;
        _rotation_pitch = pitch;

        if (!_yaw->focused())
        {
            _yaw->set_text(convert_number(yaw));
        }
        if (!_pitch->focused())
        {
            _pitch->set_text(convert_number(pitch));
        }
    }

    TextArea* CameraPosition::create_coordinate_entry(Control& parent, float& coordinate, const std::wstring& name, bool is_rotation)
    {
        const auto label_width = is_rotation ? 30 : 10;
        auto line = parent.add_child(std::make_unique<StackPanel>(Size(100, 20), Colour::Transparent, Size(), StackPanel::Direction::Horizontal));
        auto line_area = line->add_child(std::make_unique<StackPanel>(Size(10, 20), Colour::Transparent));
        line_area->add_child(std::make_unique<ui::Window>(Size(10, 1), Colour::Transparent));
        line_area->add_child(std::make_unique<Label>(Size(label_width, 20), Colour::Transparent, name + L": ", 8));
        auto entry = line->add_child(std::make_unique<TextArea>(Size(90 - (line_area->size().width - 10), 20), Colour::Transparent, Colour::White));
        entry->set_mode(TextArea::Mode::SingleLine);
        entry->set_text(convert_number(0));
        entry->set_name(to_utf8(name));
        _token_store += entry->on_focused += [=]() { entry->highlight_all(); };
        _token_store += entry->on_click += [=]() { entry->highlight_all(); };
        _token_store += entry->on_focus_lost += [=]() { entry->clear_highlight(); };
        _token_store += entry->on_escape += [=]()
        {
            entry->on_focus_clear_requested();
            entry->set_text(convert_number(coordinate)); 
        };
        _token_store += entry->on_enter += [this, &coordinate, entry, is_rotation](const std::wstring& text)
        {
            if (is_rotation)
            {
                update_rotation_coordinate(coordinate, text);
            }
            else
            {
                update_position_coordinate(coordinate, text);
            }
            entry->on_focus_clear_requested();
        };
        return entry;
    }

    void CameraPosition::update_position_coordinate(float& coordinate, const std::wstring& text)
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

    void CameraPosition::update_rotation_coordinate(float& coordinate, const std::wstring& text)
    {
        try
        {
            coordinate = std::stof(text);
            on_rotation_changed(_rotation_yaw, _rotation_pitch);
        }
        catch (...)
        {
            // Conversion failed.
        }
    }
}
