module;

#include <external/lua/src/lua.h>
#include <external/lua/src/lauxlib.h>

export module trview.app:Lua;

import std;

import trview.common;

import :ILua;
import :IRoute;
import :IRandomizerRoute;
import :IWaypoint;
import :IScriptable;
import :IMesh;

namespace trview
{
    struct IApplication;

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

    export namespace lua
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
        void create_userdata(lua_State* L, const T& value);

        template <typename T>
        void cleanup_userdata(lua_State* L, int index);

        template <typename T>
        T& get_userdata(lua_State* L, int index);

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

        template <typename T>
        int default_gc(lua_State* L);

        void assign_metatable(lua_State* L, int ref_index);
        int store_metatable(lua_State* L, const std::unordered_map<std::string, lua_CFunction>& map);
        void create_metatable(lua_State* L, const std::unordered_map<std::string, lua_CFunction>& map);
        bool equal_metatable(lua_State* L, int index, int metatable);

        using FunctionMap = const std::unordered_map<std::string, lua_CFunction>&;

        template <FunctionMap T>
        int default_index(lua_State* L);

        template <typename T, auto Prop>
        int prop_getter(lua_State* L);
    }
}

#include "Lua.inl"