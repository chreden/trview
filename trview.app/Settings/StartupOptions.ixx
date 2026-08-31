export module trview.app:StartupOptions;

import std;

import :IStartupOptions;

namespace trview
{
    export class StartupOptions final : public IStartupOptions
    {
    public:
        explicit StartupOptions(const CommandLine& command_line);
        virtual ~StartupOptions() = default;
        virtual std::string filename() const override;
        virtual bool feature(const std::string& flag) const override;
    private:
        std::string _filename;
        std::set<std::string> _flags;
    };
}
