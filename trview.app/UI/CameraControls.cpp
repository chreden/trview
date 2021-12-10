#include "CameraControls.h"
#include <trview.ui/Checkbox.h>
#include <trview.ui/Button.h>
#include "../Resources/resource.h"

namespace trview
{
    const std::string CameraControls::Names::reset = "Reset";
    const std::string CameraControls::Names::orbit = "Orbit";
    const std::string CameraControls::Names::free = "Free";
    const std::string CameraControls::Names::axis = "Axis";
    const std::string CameraControls::Names::ortho = "Ortho";

    CameraControls::CameraControls(ui::Control& parent, const std::shared_ptr<ui::ILoader>& source)
    {
        using namespace ui;

        auto camera_window = parent.add_child(source->load_from_resource(IDR_UI_CAMERA_CONTROLS));

        auto reset = camera_window->find<Button>(Names::reset);
        reset->on_click += on_reset;

        _orbit = camera_window->find<Checkbox>(Names::orbit);
        _token_store += _orbit->on_state_changed += [&](auto) { change_mode(CameraMode::Orbit); };

        _free = camera_window->find<Checkbox>(Names::free);
        _token_store += _free->on_state_changed += [&](auto) { change_mode(CameraMode::Free); };

        _axis = camera_window->find<Checkbox>(Names::axis);
        _token_store += _axis->on_state_changed += [&](auto) { change_mode(CameraMode::Axis); };

        _ortho = camera_window->find<Checkbox>(Names::ortho);
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
