#pragma once

#include "IPlugin.h"
#include <trview.common/IFiles.h>

namespace trview
{
    struct Plugin final : public IPlugin
    {
    public:
        explicit Plugin(const std::shared_ptr<IFiles>& files, const std::string& path);
        virtual ~Plugin() = default;
        std::string name() const override;
        std::string author() const override;
        std::string description() const override;
    private:
        std::string _name{ "Unknown" };
        std::string _author{ "Unknown" };
        std::string _description;
    };
}
