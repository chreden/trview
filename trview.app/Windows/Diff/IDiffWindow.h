#pragma once

#include <trview.common/Event.h>

namespace trview
{
    struct ICameraSink;
    struct ILevel;
    struct ILight;
    struct ISoundSource;
    struct ITrigger;
    struct IRoom;
    struct ISector;
    struct IStaticMesh;

    struct IDiffWindow
    {
        using Source = std::function<std::shared_ptr<IDiffWindow>()>;

        virtual ~IDiffWindow() = 0;
        virtual void render() = 0;
        virtual void set_number(int32_t number) = 0;
        /// <summary>
        /// Event raised when the window is closed.
        /// </summary>
        Event<> on_window_closed;

        Event<std::weak_ptr<ILevel>> on_diff_ended;
    };
}