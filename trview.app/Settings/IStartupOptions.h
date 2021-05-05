#pragma once

#include <string>

namespace trview
{
    struct IStartupOptions
    {
        using CommandLine = std::wstring;
        virtual ~IStartupOptions() = 0;
        virtual std::string filename() const = 0;
        virtual bool feature(const std::string& flag) const = 0;
    };
}
