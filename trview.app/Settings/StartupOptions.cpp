#include "StartupOptions.h"
#include <trview.common/Strings.h>

namespace trview
{
    StartupOptions::StartupOptions(const CommandLine& command_line)
    {
        int number_of_arguments = 0;
        const LPWSTR* const arguments = CommandLineToArgvW(command_line.c_str(), &number_of_arguments);
        if (number_of_arguments > 1)
        {
            _filename = trview::to_utf8(arguments[1]);
        }
    }

    std::string StartupOptions::filename() const
    {
        return _filename;
    }

    bool StartupOptions::feature(const std::wstring& flag) const
    {
        return false;
    }
}

