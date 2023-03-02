#pragma once

#include "IPlugin.h"
#include <trview.common/IFiles.h>

namespace trview
{
    struct Plugin final : public IPlugin
    {
        explicit Plugin(const std::shared_ptr<IFiles>& files, const std::string& path);
        virtual ~Plugin() = default;
    };
}
