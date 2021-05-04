#include "StartupOptions.h"
#include <trview.common/Strings.h>

namespace trview
{
    StartupOptions::StartupOptions(const CommandLine& command_line)
    {
        int number_of_arguments = 0;
        const LPWSTR* const arguments = CommandLineToArgvW(command_line.c_str(), &number_of_arguments);
        for (int i = 1; i < number_of_arguments; ++i)
        {
            std::wstring arg = arguments[i];
            if (arg.front() == L'-')
            {
                _flags.insert(to_utf8(arg.substr(1)));
            }
            else if(i == 1)
            {
                _filename = trview::to_utf8(arg);
            }
        }
    }

    std::string StartupOptions::filename() const
    {
        return _filename;
    }

    bool StartupOptions::feature(const std::string& flag) const
    {
        return _flags.find(flag) != _flags.end();
    }
}

