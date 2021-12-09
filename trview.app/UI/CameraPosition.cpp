#include "CameraPosition.h"
#include <trlevel/ILevel.h>
#include <trview.ui/Label.h>
#include <trview.common/Strings.h>
#include "../Resources/resource.h"

using namespace trview::ui;
using namespace trview::graphics;
using namespace DirectX::SimpleMath;

namespace trview
{
    namespace
    {
        std::wstring convert_number(float value)
        {
            std::wstringstream stream;
            stream.precision(4);
            stream << std::fixed << (value >= 0 ? L" " : L"") << value;
            return stream.str();
        }
    }

    const std::string CameraPosition::Names::x{ "X" };
    const std::string CameraPosition::Names::y{ "Y" };
    const std::string CameraPosition::Names::z{ "Z" };
    const std::string CameraPosition::Names::yaw{ "Yaw" };
    const std::string CameraPosition::Names::pitch{ "Pitch" };

    CameraPosition::CameraPosition(Control& parent, const ui::ILoader& ui_source)
    {
        auto panel = parent.add_child(ui_source.load_from_resource(IDR_UI_CAMERA_POSITION));
        _yaw = panel->find<TextArea>(Names::yaw);
        _pitch = panel->find<TextArea>(Names::pitch);
        _x = panel->find<TextArea>(Names::x);
        _y = panel->find<TextArea>(Names::y);
        _z = panel->find<TextArea>(Names::z);

        bind_coordinate_entry(_yaw, _rotation_yaw, true);
        bind_coordinate_entry(_pitch, _rotation_pitch, true);
        bind_coordinate_entry(_x, _position.x, false);
        bind_coordinate_entry(_y, _position.y, false);
        bind_coordinate_entry(_z, _position.z, false);

        const auto update_position = [=](Size size)
        {
            panel->set_position(Point(panel->position().x, size.height - 10 - panel->size().height));
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
            _yaw->set_text(convert_number(_display_degrees ? DirectX::XMConvertToDegrees(yaw) : yaw));
        }
        if (!_pitch->focused())
        {
            _pitch->set_text(convert_number(_display_degrees ? DirectX::XMConvertToDegrees(pitch) : pitch));
        }
    }

    void CameraPosition::bind_coordinate_entry(ui::TextArea* entry, float& coordinate, bool is_rotation)
    {
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
    }

    void CameraPosition::update_position_coordinate(float& coordinate, const std::wstring& text)
    {
        try
        {
            const auto value = std::stof(text);
            if (!std::isfinite(value))
            {
                return;
            }
            coordinate = value / trlevel::Scale_X;
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
            const auto value = std::stof(text);
            if (!std::isfinite(value))
            {
                return;
            }
            coordinate = _display_degrees ? DirectX::XMConvertToRadians(value) : value;
            on_rotation_changed(_rotation_yaw, _rotation_pitch);
        }
        catch (...)
        {
            // Conversion failed.
        }
    }

    void CameraPosition::set_display_degrees(bool value)
    {
        _display_degrees = value;
        set_rotation(_rotation_yaw, _rotation_pitch);
    }
}
