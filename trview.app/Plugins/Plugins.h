#pragma once

#include "IPlugins.h"
#include "IPlugin.h"
#include "../Settings/UserSettings.h"
#include <trview.common/IFiles.h>
#include <trview.common/Messages/IRecipient.h>

namespace trview
{
    class Plugins final : public IPlugins, public IRecipient
    {
    public:
        explicit Plugins(const std::shared_ptr<IFiles>& files,
            const std::shared_ptr<IPlugin>& default_plugin,
            const IPlugin::Source& plugin_source,
            const UserSettings& settings);
        virtual ~Plugins() = default;
        std::vector<std::weak_ptr<IPlugin>> plugins() const override;
        void initialise(IApplication* application) override;
        void render_ui() override;
        void reload() override;
        void receive_message(const Message& message) override;
    private:
        std::vector<std::shared_ptr<IPlugin>> _plugins;
        std::shared_ptr<IFiles> _files;
        UserSettings _settings;
        IPlugin::Source _plugin_source;
        IApplication* _application;
    };
}
