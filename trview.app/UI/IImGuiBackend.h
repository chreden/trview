#pragma once

namespace trview
{
    struct IImGuiBackend
    {
        virtual ~IImGuiBackend() = 0;
        virtual void initialise() = 0;
        virtual bool is_setup() const = 0;
        virtual void new_frame() = 0;
        virtual void rebuild_fonts() = 0;
        virtual void render() = 0;
        virtual void reset_layout() = 0;
        virtual void shutdown() = 0;
        virtual bool window_procedure(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) = 0;
    };
}
