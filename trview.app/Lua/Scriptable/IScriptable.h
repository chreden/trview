#pragma once

#include <functional>
#include <memory>

#include <trview.common/Event.h>

#include "../../Geometry/IMesh.h"

struct lua_State;

namespace trview
{
    struct ICamera;

    struct IScriptable
    {
        using Source = std::function<std::shared_ptr<IScriptable>(lua_State*)>;
        virtual ~IScriptable() = 0;
        virtual void click() = 0;
        virtual int data() const = 0;
        virtual std::shared_ptr<IMesh> mesh() const = 0;
        virtual std::string notes() const = 0;
        virtual DirectX::SimpleMath::Vector3 position() const = 0;
        virtual void render(const ICamera& camera) = 0;
        virtual DirectX::SimpleMath::Vector3 screen_position() const = 0;
        virtual void set_data(int ref) = 0;
        virtual void set_notes(const std::string& notes) = 0;
        virtual void set_on_click(int ref) = 0;
        virtual void set_position(const DirectX::SimpleMath::Vector3& position) = 0;
        virtual void set_on_tooltip(int ref) = 0;
        virtual void set_screen_position(const DirectX::SimpleMath::Vector3& position) = 0;
        virtual std::string tooltip() const = 0;

        Event<> on_changed;
    };

    namespace lua
    {
        void scriptable_register(lua_State* L, const IScriptable::Source& source);
        std::shared_ptr<IScriptable> to_scriptable(lua_State* L, int index);
    }
}
