#pragma once

#include "../../Plugins/IPlugins.h"
#include "IPluginsWindow.h"
#include <trview.common/Windows/IShell.h>

namespace trview
{
    class PluginsWindow final : public IPluginsWindow
    {
    public:
        explicit PluginsWindow(const std::weak_ptr<IPlugins>& plugins, const std::shared_ptr<IShell>& shell);
        virtual ~PluginsWindow() = default;
        void render() override;
        void set_number(int32_t number) override;
        void update(float dt) override;
    private:
        bool render_plugins_window();

        std::string _id{ "Plugins 0" };
        std::weak_ptr<IPlugins> _plugins;
        std::shared_ptr<IShell> _shell;
    };
}

