#pragma once

namespace trview
{
    struct ILauWindow;

    struct ILauWindowManager
    {
        virtual ~ILauWindowManager() = 0;
        virtual void render(bool vsync) = 0;
        virtual std::weak_ptr<ILauWindow> create_window() = 0;
    };
}
