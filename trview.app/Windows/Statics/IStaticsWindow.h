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
        virtual void update(float dt) = 0;
        virtual void set_number(int32_t number) = 0;
        virtual void set_statics(const std::vector<std::weak_ptr<IStaticMesh>>& statics) = 0;

        Event<> on_window_closed;
    };
}
