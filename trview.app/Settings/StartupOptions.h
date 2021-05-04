#pragma once

#include "IStartupOptions.h"

namespace trview
{
    class StartupOptions final : public IStartupOptions
    {
    public:
        explicit StartupOptions(const CommandLine& command_line);
        virtual ~StartupOptions() = default;
        virtual std::string filename() const override;
        virtual bool feature(const std::wstring& flag) const override;
    private:
        std::string _filename;
    };
}
