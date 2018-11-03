#include "FileDropper.h"
#include <trview.common/Strings.h>

namespace trview
{
    FileDropper::FileDropper(HWND window)
        : MessageHandler(window)
    {
        // Makes this window accept dropped files.
        DragAcceptFiles(window, TRUE);
    }

    void FileDropper::process_message(HWND window, UINT message, WPARAM wParam, LPARAM lParam)
    {
        if(message == WM_DROPFILES)
        {
            wchar_t filename[MAX_PATH];
            memset(&filename, 0, sizeof(filename));
            DragQueryFile((HDROP)wParam, 0, filename, MAX_PATH);
            on_file_dropped(to_utf8(filename));
        }
    }
}
