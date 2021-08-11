module;
#include <string>
#include <windows.h>
export module trview.common.windows.window;

import trview.common.size;

namespace trview
{
    export class Window
    {
    public:
        Window(HWND window);
        Size size() const;
        HWND window() const;
        operator HWND () const;

        /// Set the title of the window.
        /// @param title The new window title.
        void set_title(const std::string& title);
    private:
        HWND        _window;
        uint32_t    _width;
        uint32_t    _height;
    };
}
