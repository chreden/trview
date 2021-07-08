#include "CameraControls.h"

#include <trview.ui/GroupBox.h>
#include <trview.ui/Checkbox.h>
#include <trview.ui/Button.h>
#include <trview.ui/Slider.h>
#include <trview.ui/Label.h>
#include <trview.ui/Layouts/GridLayout.h>

namespace trview
{
    CameraControls::CameraControls(ui::Control& parent)
    {
        using namespace ui;

        auto camera_window = parent.add_child(std::make_unique<GroupBox>(Size(150, 92), Colour::Transparent, Colour::Grey, L"Camera"));
        auto grid = camera_window->add_child(std::make_unique<ui::Window>(Size(150, 63), Colour::Transparent));
        grid->set_layout(std::make_unique<GridLayout>(2, 3));

        // Make a button with a label next to it, until this kind of control exists.
        auto create_labelled_button = [](Event<>& on_click, const std::wstring& text)
        {
            auto panel = std::make_unique<StackPanel>(Size(56, 16), Colour::Transparent, Size(), StackPanel::Direction::Horizontal, SizeMode::Manual);
            auto button = panel->add_child(std::make_unique<Button>(Size(16, 16)));
            button->on_click += on_click;
            panel->add_child(std::make_unique<Label>(Size(40, 16), Colour::Transparent, text, 8, graphics::TextAlignment::Left, graphics::ParagraphAlignment::Centre));
            return std::move(panel);
        };

        grid->add_child(create_labelled_button(on_reset, L"Reset"));

        _orbit = grid->add_child(std::make_unique<Checkbox>(Colour::Transparent, L"Orbit"));
        _token_store += _orbit->on_state_changed += [&](auto) { change_mode(CameraMode::Orbit); };

        _free = grid->add_child(std::make_unique<Checkbox>(Colour::Transparent, L"Free"));
        _token_store += _free->on_state_changed += [&](auto) { change_mode(CameraMode::Free); };

        _axis = grid->add_child(std::make_unique<Checkbox>(Colour::Transparent, L"Axis"));
        _token_store += _axis->on_state_changed += [&](auto) { change_mode(CameraMode::Axis); };

        _ortho = grid->add_child(std::make_unique<Checkbox>(Colour::Transparent, L"Ortho"));
        _token_store += _ortho->on_state_changed += [&](auto ortho_enabled) { change_projection(ortho_enabled ? ProjectionMode::Orthographic : ProjectionMode::Perspective); };
    }

    // Set the current camera mode and raise the on_mode_selected event.
    // mode: The new camera mode.
    void CameraControls::change_mode(CameraMode mode)
    {
        set_mode(mode);
        on_mode_selected(mode);
    }

    void CameraControls::change_projection(ProjectionMode mode)
    {
        set_projection_mode(mode);
        on_projection_mode_selected(mode);
    }

    // Set the current camera mode. This will not raise the on_mode_selected event.
    // mode: The camera mode to change to.
    void CameraControls::set_mode(CameraMode mode)
    {
        _orbit->set_state(mode == CameraMode::Orbit);
        _free->set_state(mode == CameraMode::Free);
        _axis->set_state(mode == CameraMode::Axis);
    }

    void CameraControls::set_projection_mode(ProjectionMode mode)
    {
        _ortho->set_state(mode == ProjectionMode::Orthographic);
    }
}
