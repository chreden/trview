#pragma once

#include <trview.lau/drm.h>

namespace trview
{
    struct ILauWindow;

    struct ILauWindowManager
    {
        Event<lau::Drm> on_mesh;

        virtual ~ILauWindowManager() = 0;
        virtual void render(bool vsync) = 0;
        virtual std::weak_ptr<ILauWindow> create_window() = 0;
    };
}
