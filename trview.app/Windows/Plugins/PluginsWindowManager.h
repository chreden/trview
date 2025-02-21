#pragma once

#include <trview.common/MessageHandler.h>
#include "../WindowManager.h"
#include "IPluginsWindowManager.h"

namespace trview
{
    class PluginsWindowManager final : public IPluginsWindowManager, public WindowManager<IPluginsWindow>, public MessageHandler
    {
    public:
        explicit PluginsWindowManager(const Window& window, const std::shared_ptr<IShortcuts>& shortcuts, const IPluginsWindow::Source& plugins_window_source);
        virtual ~PluginsWindowManager() = default;
        std::optional<int> process_message(UINT message, WPARAM wParam, LPARAM lParam) override;
        void render() override;
        std::weak_ptr<IPluginsWindow> create_window() override;
        void update(float delta) override;
        void set_settings(const UserSettings& settings) override;
    private:
        IPluginsWindow::Source _source;
        UserSettings _settings;
    };
}
