#pragma once

#include <trview.common/IFiles.h>
#include <trview.common/Windows/IDialogs.h>

#include "../../Routing/IRoute.h"
#include "../../Routing/IRandomizerRoute.h"
#include "../../Settings/UserSettings.h"

struct lua_State;

namespace trview
{
    namespace lua
    {
        int create_route(lua_State* L, const std::shared_ptr<IRoute>& route);
        std::shared_ptr<IRoute> to_route(lua_State* L, int index);
        void route_register(lua_State* L, const IRoute::Source& source, const IRandomizerRoute::Source& randomizer_source, const std::shared_ptr<IDialogs>& dialogs, const std::shared_ptr<IFiles>& files);
        void route_set_settings(const UserSettings& new_settings);
    }
}
