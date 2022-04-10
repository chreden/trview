#pragma once

#include <trview.common/TokenStore.h>
#include <trview.common/IFiles.h>

#include "../../Settings/UserSettings.h"

#include "IPlugins.h"
#include "IPlugin.h"

namespace trview
{
    class Plugins final : public IPlugins, public MessageHandler
    {
    public:
        explicit Plugins(
            const Window& window,
            const std::shared_ptr<IShortcuts>& shortcuts,
            const IPlugin::Source& plugin_source,
            const std::shared_ptr<IFiles>& files, 
            const UserSettings& settings);
        virtual ~Plugins() = default;
        virtual void load() override;
        virtual void initialise() override;
        virtual void render_ui() override;
        virtual std::optional<int> process_message(UINT message, WPARAM wParam, LPARAM lParam) override;
    private:
        void reload();

        TokenStore _token_store;
        IPlugin::Source _plugin_source;
        UserSettings _settings;
        std::shared_ptr<IFiles> _files;
        std::vector<std::unique_ptr<IPlugin>> _plugins;
    };
}