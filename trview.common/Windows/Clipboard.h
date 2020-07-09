#pragma once

#include <string>

namespace trview
{
    class Window;

    std::wstring read_clipboard(const Window& window);
    void write_clipboard(const Window& window, const std::wstring& text);
}
