#include "StartupOptions.h"

namespace trview
{
    StartupOptions::StartupOptions(const CommandLine& command_line)
    {
    }

    std::wstring StartupOptions::filename() const
    {
        return {};
    }

    bool StartupOptions::feature(const std::wstring& flag) const
    {
        return false;
    }
}

