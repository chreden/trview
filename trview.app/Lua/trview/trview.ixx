module;

#include <external/lua/src/lua.h>

export module trview.app:trview;

import trview.common;

import :ILevel;
import :IRoute;
import :IRandomizerRoute;
import :UserSettings;
import :IScriptable;
import :Forward;

namespace trview
{
    namespace lua
    {
        export void trview_register(lua_State* L,
            IApplication* application,
            const IRoute::Source& route_source,
            const IRandomizerRoute::Source& randomizer_route_source,
            const IWaypoint::Source& waypoint_source,
            const IScriptable::Source& scriptable_source,
            const std::shared_ptr<IDialogs>& dialogs,
            const std::shared_ptr<IFiles>& files,
            const IMesh::Source& mesh_source);
        export void set_settings(const UserSettings& settings);
    }
}
