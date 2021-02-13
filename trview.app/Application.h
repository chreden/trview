#pragma once

namespace trview
{
    class Application
    {
    public:
        Application(HINSTANCE hInstance, const std::wstring& command_line, int command_show);
        void run();
    };
}
