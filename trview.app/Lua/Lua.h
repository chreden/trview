#pragma once

#include <external/lua/src/lua.h>
#include <external/lua/src/lauxlib.h>
#include <string>
#include <vector>
#include <cstdint>
#include <trview.common/Event.h>
#include <functional>
#include "ILua.h"
#include "../Routing/IRoute.h"
#include "../Routing/IRandomizerRoute.h"
#include "../Settings/UserSettings.h"

#include <trview.common/Windows/IDialogs.h>
#include <trview.common/IFiles.h>

namespace trview
{
    struct IApplication;

    class Lua final : public ILua
    {
    public:
        explicit Lua(const IRoute::Source& route_source, const IRandomizerRoute::Source& randomizer_route_source, const IWaypoint::Source& waypoint_source, const std::shared_ptr<IDialogs>& dialogs, const std::shared_ptr<IFiles>& files);
        ~Lua();
        void do_file(const std::string& file) override;
        void execute(const std::string& command) override;
        void initialise(IApplication* application) override;
    private:
        lua_State* L{ nullptr };
        IRoute::Source _route_source;
        IRandomizerRoute::Source _randomizer_route_source;
        IWaypoint::Source _waypoint_source;
        std::shared_ptr<IDialogs> _dialogs;
        std::shared_ptr<IFiles> _files;
    };

    namespace lua
    {
        int push_string(lua_State* L, const std::string& text);

        template <typename Func>
        int push_list_p(lua_State* L, std::ranges::input_range auto&& range, Func&& func);

        template <typename Func>
        int push_list(lua_State* L, std::ranges::input_range auto&& range, Func&& func);

        template <typename T>
        T* get_self(lua_State* L);

        template <typename T>
        struct EnumValue
        {
            std::string name;
            T value;
        };

        template <typename T>
        void set_enum_value(lua_State* L, const EnumValue<T>& value);
        
        template <typename T>
        void create_enum(lua_State* L, const std::string& name, const std::vector<EnumValue<T>>& values);
    }
}

#include "Lua.inl"