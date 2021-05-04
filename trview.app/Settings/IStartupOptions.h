#pragma once

#include <string>

namespace trview
{
    struct IStartupOptions
    {
        using CommandLine = std::wstring;
        virtual ~IStartupOptions() = 0;
        virtual std::wstring filename() const = 0;
        virtual bool feature(const std::wstring& flag) const = 0;
    };
}
