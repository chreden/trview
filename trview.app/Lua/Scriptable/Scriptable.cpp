#include "Scriptable.h"
#include "../Lua.h"
#include "../Vector3.h"
#include "../../Camera/ICamera.h"

using namespace DirectX::SimpleMath;

namespace trview
{
    namespace lua
    {
        namespace
        {
            IScriptable::Source scriptable_source;

            int scriptable_index(lua_State* L)
            {
                auto scriptable = get_self<IScriptable>(L);
                const std::string key = lua_tostring(L, 2);

                if (key == "click")
                {
                    scriptable->click();
                }
                else if (key == "data")
                {
                    int ref = scriptable->data();
                    lua_rawgeti(L, LUA_REGISTRYINDEX, ref);
                    return 1;
                }
                else if (key == "notes")
                {
                    lua_pushstring(L, scriptable->notes().c_str());
                    return 1;
                }
                else if (key == "position")
                {
                    return create_vector3(L, scriptable->position() * trlevel::Scale);
                }

                return 0;
            }

            int scriptable_newindex(lua_State* L)
            {
                auto scriptable = get_self<IScriptable>(L);
                const std::string key = lua_tostring(L, 2);

                if (key == "data")
                {
                    scriptable->set_data(luaL_ref(L, LUA_REGISTRYINDEX));
                }
                else if (key == "notes")
                {
                    scriptable->set_notes(lua_tostring(L, -1));
                }
                else if (key == "on_click")
                {
                    scriptable->set_on_click(luaL_ref(L, LUA_REGISTRYINDEX));
                }
                else if (key == "on_tooltip")
                {
                    scriptable->set_on_tooltip(luaL_ref(L, LUA_REGISTRYINDEX));
                }
                else if (key == "position")
                {
                    scriptable->set_position(to_vector3(L, -1) / trlevel::Scale);
                }

                return 0;
            }

            int create_scriptable(lua_State* L, const std::shared_ptr<IScriptable>& scriptable)
            {
                return create(L, scriptable, scriptable_index, scriptable_newindex);
            }

            int scriptable_new(lua_State* L)
            {
                return create_scriptable(L, scriptable_source(L));
            }
        }

        void scriptable_register(lua_State* L, const IScriptable::Source& source)
        {
            scriptable_source = source;
            lua_newtable(L);
            lua_pushcfunction(L, scriptable_new);
            lua_setfield(L, -2, "new");
            lua_setglobal(L, "Scriptable");
        }

        std::shared_ptr<IScriptable> to_scriptable(lua_State* L, int index)
        {
            return get_self<IScriptable>(L, index);
        }
    }

    IScriptable::~IScriptable()
    {
    }

    Scriptable::Scriptable(lua_State* L, const std::shared_ptr<IMesh>& mesh, const graphics::Texture& texture)
        : _state(L), _mesh(mesh), _texture(texture)
    {
    }

    void Scriptable::click()
    {
        if (_on_click == LUA_NOREF)
        {
            return;
        }

        if (lua_rawgeti(_state, LUA_REGISTRYINDEX, _on_click) == LUA_TNIL)
        {
            lua_pop(_state, 1);
        }
        else
        {
            lua::create_scriptable(_state, shared_from_this());
            lua_pcall(_state, 1, 0, 0);
        }
    }

    int Scriptable::data() const
    {
        return _data;
    }

    void Scriptable::set_data(int ref)
    {
        _data = ref;
    }

    void Scriptable::set_on_click(int ref)
    {
        _on_click = ref;
    }

    std::shared_ptr<IMesh> Scriptable::mesh() const
    {
        return _mesh;
    }

    std::string Scriptable::notes() const
    {
        return _notes;
    }

    DirectX::SimpleMath::Vector3 Scriptable::position() const
    {
        return _position;
    }

    void Scriptable::render(const ICamera& camera)
    {
        auto world = Matrix::CreateScale(0.25f) * Matrix::CreateTranslation(position());
        auto wvp = world * camera.view_projection();
        auto light_direction = Vector3::TransformNormal(camera.position() - position(), world.Invert());
        light_direction.Normalize();
        mesh()->render(wvp, _texture, Colour::White, 1.0f, light_direction);

        const auto window_size = camera.view_size();
        set_screen_position(XMVector3Project(position(), 0, 0, window_size.width, window_size.height, 0, 1.0f, camera.projection(), camera.view(), Matrix::Identity));
    }

    DirectX::SimpleMath::Vector3 Scriptable::screen_position() const
    {
        return _screen_position;
    }

    void Scriptable::set_position(const DirectX::SimpleMath::Vector3& position) 
    {
        _position = position;
        on_changed();
    }

    void Scriptable::set_notes(const std::string& notes)
    {
        _notes = notes;
        on_changed();
    }

    void Scriptable::set_on_tooltip(int ref)
    {
        _on_tooltip = ref;
    }

    void Scriptable::set_screen_position(const DirectX::SimpleMath::Vector3& position)
    {
        _screen_position = position;
    }

    std::string Scriptable::tooltip() const
    {
        if (_on_tooltip == LUA_NOREF)
        {
            return "Scriptable";
        }

        if (lua_rawgeti(_state, LUA_REGISTRYINDEX, _on_tooltip) == LUA_TNIL)
        {
            lua_pop(_state, 1);
            return "Scriptable";
        }
        else
        {
            lua::create_scriptable(_state, std::const_pointer_cast<IScriptable>(shared_from_this()));
            lua_pcall(_state, 1, 1, 0);
            return lua_tostring(_state, -1);
        }
    }
}