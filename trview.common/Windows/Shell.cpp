#include "Shell.h"
#include <shellapi.h>

namespace trview
{
    IShell::~IShell()
    {
    }

    void Shell::open(const std::wstring& path)
    {
        ShellExecute(0, 0, path.c_str(), 0, 0, SW_SHOW);
    }
}
