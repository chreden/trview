#pragma once

#include <trview.app/Windows/Viewer.h>

namespace trview
{
    class Application
    {
    public:
        Application(HINSTANCE hInstance, const std::wstring& command_line, int command_show);
        int run();
    private:
        std::unique_ptr<Viewer> _viewer;
        HINSTANCE _instance;
    };
}
