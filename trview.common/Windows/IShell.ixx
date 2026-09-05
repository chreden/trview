export module trview.common:IShell;

import std;

namespace trview
{
    export struct IShell
    {
        virtual ~IShell() = 0;
        virtual void open(const std::wstring& path) = 0;
    };
}
