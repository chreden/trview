#pragma once

#include "../../Plugins/IPlugins.h"
#include "IPluginsWindow.h"
#include <trview.common/Windows/IShell.h>
#include <trview.common/Windows/IDialogs.h>

namespace trview
{
    class PluginsWindow final : public IPluginsWindow
    {
    public:
        struct Names
        {
            static inline const std::string plugins_list = "Plugins";
        };

        explicit PluginsWindow(const std::weak_ptr<IPlugins>& plugins, const std::shared_ptr<IShell>& shell, const std::shared_ptr<IDialogs>& dialogs);
        virtual ~PluginsWindow() = default;
        void render() override;
        void set_number(int32_t number) override;
        void set_settings(const UserSettings& settings) override;
        void update(float dt) override;
    private:
        bool render_plugins_window();

        std::string _id{ "Plugins 0" };
        std::weak_ptr<IPlugins> _plugins;
        std::shared_ptr<IShell> _shell;
        std::shared_ptr<IDialogs> _dialogs;
        UserSettings _settings;
    };
}

