module;

#include <external/lua/src/lua.h>

export module trview.app:Lua;

import std;

import trview.common;

import :ILua;
import :IRoute;
import :IRandomizerRoute;
import :IWaypoint;
import :IScriptable;
import :IMesh;
import :Forward;

namespace trview
{
    export class Lua final : public ILua
    {
    public:
        explicit Lua(
            const IRoute::Source& route_source,
            const IRandomizerRoute::Source& randomizer_route_source,
            const IWaypoint::Source& waypoint_source,
            const IScriptable::Source& scriptable_source,
            const std::shared_ptr<IDialogs>& dialogs,
            const std::shared_ptr<IFiles>& files,
            const IMesh::Source& mesh_source);
        ~Lua();
        void do_file(const std::string& file) override;
        void execute(const std::string& command) override;
        void initialise(IApplication* application) override;
        void set_directory(const std::string& directory) override;
    private:
        void create_state();

        lua_State* L{ nullptr };
        IRoute::Source _route_source;
        IRandomizerRoute::Source _randomizer_route_source;
        IScriptable::Source _scriptable_source;
        IWaypoint::Source _waypoint_source;
        IMesh::Source _mesh_source;
        std::shared_ptr<IDialogs> _dialogs;
        std::shared_ptr<IFiles> _files;
        std::string _directory;
    };
}
