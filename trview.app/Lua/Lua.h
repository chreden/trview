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
        void set_directory(const std::string& directory) override;
    private:
        void create_state();

        lua_State* L{ nullptr };
        IRoute::Source _route_source;
        IRandomizerRoute::Source _randomizer_route_source;
        IWaypoint::Source _waypoint_source;
        std::shared_ptr<IDialogs> _dialogs;
        std::shared_ptr<IFiles> _files;
        std::string _directory;
    };

    namespace lua
    {
        int push_string(lua_State* L, const std::string& text);

        template <typename Func>
        int push_list_p(lua_State* L, std::ranges::input_range auto&& range, Func&& func);

        template <typename Func>
        int push_list(lua_State* L, std::ranges::input_range auto&& range, Func&& func);

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

        template <typename T>
        int create(lua_State* L, const std::shared_ptr<T>& self, lua_CFunction index, lua_CFunction new_index);

        template <typename T>
        std::shared_ptr<T> get_self(lua_State* L, int index = 1);

        template <typename T>
        T* get_self_raw(lua_State* L, int index = 1);

        template <typename T>
        void set_self(lua_State* L, const std::shared_ptr<T>& self);

        /// <summary>
        /// GC function for cleaning up a shared_ptr.
        /// </summary>
        /// <typeparam name="T">Pointed to type</typeparam>
        /// <param name="L">Lua state</param>
        /// <returns>Stack change.</returns>
        template <typename T>
        int gc(lua_State* L);
    }
}

#include "Lua.inl"