#pragma once

#include <string>

namespace trview
{
    struct IShell
    {
        virtual ~IShell() = 0;
        virtual void open(const std::wstring& path) = 0;
    };
}
