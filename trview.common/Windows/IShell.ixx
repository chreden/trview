export module trview.common:IShell;

import std;

namespace trview
{
    struct IShell
    {
        virtual ~IShell() = 0;
        virtual void open(const std::wstring& path) = 0;
    };
}
