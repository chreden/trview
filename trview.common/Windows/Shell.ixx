export module trview.common:Shell;

import std;
import :IShell;

namespace trview
{
    class Shell final : public IShell
    {
    public:
        virtual ~Shell() = default;
        virtual void open(const std::wstring& path) override;
    };
}
