#pragma once

#include <cstdint>
#include <functional>
#include <memory>
#include <trview.common/Event.h>

#include "../../Elements/IStaticMesh.h"

namespace trview
{
    struct IStaticsWindow
    {
        using Source = std::function<std::shared_ptr<IStaticsWindow>()>;
        virtual ~IStaticsWindow() = 0;
        virtual void render() = 0;
        virtual std::weak_ptr<IStaticMesh> selected_static() const = 0;
        virtual void set_current_room(const std::weak_ptr<IRoom>& room) = 0;
        virtual void set_number(int32_t number) = 0;
        virtual void set_selected_static(const std::weak_ptr<IStaticMesh>& static_mesh) = 0;
        virtual void set_statics(const std::vector<std::weak_ptr<IStaticMesh>>& statics) = 0;
        virtual void update(float dt) = 0;

        Event<> on_window_closed;
        Event<std::weak_ptr<IStaticMesh>> on_static_selected;
    };
}
