#pragma once

#include <vector>
#include <memory>
#include <trview.app/Elements/ILight.h>
#include "ILightsWindow.h"

namespace trview
{
    struct ILightsWindowManager
    {
        virtual ~ILightsWindowManager() = 0;
        virtual void set_lights(const std::vector<std::weak_ptr<ILight>>& lights) = 0;
        virtual std::weak_ptr<ILightsWindow> create_window() = 0;
        virtual void render() = 0;
        virtual void update(float delta) = 0;
        virtual void set_selected_light(const std::weak_ptr<ILight>& light) = 0;
        virtual void set_level_version(trlevel::LevelVersion version) = 0;
        virtual void set_room(const std::weak_ptr<IRoom>& room) = 0;

        Event<std::weak_ptr<ILight>> on_light_selected;
        Event<std::weak_ptr<ILight>, bool> on_light_visibility;
    };
}
