#pragma once

#include "../../Plugins/IPlugins.h"
#include "../IWindow.h"
#include <trview.common/Windows/IShell.h>
#include <trview.common/Windows/IDialogs.h>
#include <trview.common/Messages/IMessageSystem.h>
#include "../../Settings/UserSettings.h"

namespace trview
{
    class PluginsWindow final : public IWindow, public std::enable_shared_from_this<IRecipient>
    {
    public:
        struct Names
        {
            static inline const std::string plugins_list = "Plugins";
        };

        explicit PluginsWindow(const std::weak_ptr<IPlugins>& plugins, const std::shared_ptr<IShell>& shell, const std::shared_ptr<IDialogs>& dialogs, const std::weak_ptr<IMessageSystem>& messaging);
        virtual ~PluginsWindow() = default;
        void render() override;
        void set_number(int32_t number) override;
        void update(float dt) override;
        void receive_message(const Message& message) override;
        std::string type() const override;
        std::string title() const override;
    private:
        bool render_plugins_window();

        std::string _id{ "Plugins 0" };
        std::weak_ptr<IPlugins> _plugins;
        std::shared_ptr<IShell> _shell;
        std::shared_ptr<IDialogs> _dialogs;
        std::optional<UserSettings> _settings;
        std::weak_ptr<IMessageSystem> _messaging;
    };
}

