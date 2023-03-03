#pragma once

#include "IPlugins.h"
#include "IPlugin.h"
#include "../Settings/UserSettings.h"
#include <trview.common/IFiles.h>

namespace trview
{
    class Plugins final : public IPlugins
    {
    public:
        explicit Plugins(const std::shared_ptr<IFiles>& files,
            const IPlugin::Source& plugin_source,
            const UserSettings& settings);
        virtual ~Plugins() = default;
        std::vector<std::weak_ptr<IPlugin>> plugins() const override;
        void initialise(IApplication* application) override;
    private:
        std::vector<std::shared_ptr<IPlugin>> _plugins;
    };
}
