#pragma once

#include <memory>
#include <trview.common/Event.h>

namespace trview
{
    struct ICameraSink;
    struct IDiffWindow;
    struct ILevel;
    struct ILight;
    struct ISoundSource;
    struct IStaticMesh;
    struct ITrigger;
    struct IRoom;

    struct IDiffWindowManager
    {
        virtual ~IDiffWindowManager() = 0;
        virtual std::weak_ptr<IDiffWindow> create_window() = 0;
        virtual void render() = 0;
        virtual void set_level(const std::weak_ptr<ILevel>& level) = 0;

        Event<std::weak_ptr<ILevel>> on_diff_ended;
    };
}
