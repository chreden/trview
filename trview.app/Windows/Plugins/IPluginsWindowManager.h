#pragma once

#include "IPluginsWindow.h"

namespace trview
{
    struct IPluginsWindowManager
    {
        virtual ~IPluginsWindowManager() = 0;
        virtual void render() = 0;
        virtual std::weak_ptr<IPluginsWindow> create_window() = 0;
        virtual void set_settings(const UserSettings& settings) = 0;
        virtual void update(float dt) = 0;

        Event<UserSettings> on_settings;
    };
}