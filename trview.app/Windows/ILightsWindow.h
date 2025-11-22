#pragma once

#include <trview.common/Window.h>
#include <trview.common/Event.h>
#include <trview.app/Elements/ILight.h>

namespace trview
{
    struct ILightsWindow
    {
        using Source = std::function<std::shared_ptr<ILightsWindow>()>;
        virtual ~ILightsWindow() = 0;
        virtual void clear_selected_light() = 0;
        virtual void render() = 0;
        virtual void update(float delta) = 0;
        virtual void set_lights(const std::vector<std::weak_ptr<ILight>>& lights) = 0;
        virtual void set_level_version(trlevel::LevelVersion version) = 0;
        virtual void set_number(int32_t number) = 0;
        virtual void set_current_room(const std::weak_ptr<IRoom>& room) = 0;
        /// <summary>
        /// Event raised when the window is closed.
        /// </summary>
        Event<> on_window_closed;
    };
}
