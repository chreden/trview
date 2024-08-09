#pragma once

#include <external/lua/src/lua.h>
#include <external/lua/src/lauxlib.h>

#include <trview.common/IFiles.h>
#include <trview.common/Windows/IDialogs.h>

#include "../../Elements/ILevel.h"
#include "../../Routing/IRoute.h"
#include "../../Routing/IRandomizerRoute.h"
#include "../../Settings/UserSettings.h"
#include "../Scriptable/IScriptable.h"

namespace trview
{
    struct IApplication;

    namespace lua
    {
        void trview_register(lua_State* L,
            IApplication* application,
            const IRoute::Source& route_source,
            const IRandomizerRoute::Source& randomizer_route_source,
            const IWaypoint::Source& waypoint_source,
            const IScriptable::Source& scriptable_source,
            const std::shared_ptr<IDialogs>& dialogs,
            const std::shared_ptr<IFiles>& files);
        void set_settings(const UserSettings& settings);
    }
}
