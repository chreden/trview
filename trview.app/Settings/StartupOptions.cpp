#include "StartupOptions.h"
#include <trview.common/Strings.h>

namespace trview
{
    namespace
    {
        std::string build_filename(const LPWSTR* const arguments, int number_of_arguments, int start)
        {
            if (number_of_arguments < start)
            {
                return std::string();
            }

            std::wstring filename = arguments[start];
            for (int i = start + 1; i < number_of_arguments; ++i)
            {
                filename += L" " + std::wstring(arguments[i]);
            }
            return to_utf8(filename);
        }
    }

    IStartupOptions::~IStartupOptions()
    {
    }

    StartupOptions::StartupOptions(const CommandLine& command_line)
    {
        int number_of_arguments = 0;
        const LPWSTR* const arguments = CommandLineToArgvW(command_line.c_str(), &number_of_arguments);

        // Open with paths won't be passed quoted, so if the path contains spaces then stick the arguments
        // back together and just try that.
        if (number_of_arguments > 1 && !std::wstring(arguments[1]).starts_with(L'-'))
        {
            _filename = build_filename(arguments, number_of_arguments, 1);
            return;
        }

        for (int i = 1; i < number_of_arguments; ++i)
        {
            std::wstring arg = arguments[i];
            if (arg.front() == L'-')
            {
                _flags.insert(to_utf8(arg.substr(1)));
            }
            else
            {
                _filename = build_filename(arguments, number_of_arguments, i);
                return;
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

