#pragma once

#include <unordered_map>
#include <trview.common/Windows/IShortcuts.h>

#include "IWindows.h"
#include "IWindow.h"
#include "WindowManager.h"

namespace trview
{
    struct IRouteWindowManager;

    class Windows final : public IWindows, public WindowManager<IWindow>, public MessageHandler
    {
    public:
        explicit Windows(const Window& window, const std::shared_ptr<IShortcuts>& shortcuts);
        virtual ~Windows() = default;
        std::weak_ptr<IWindow> create(const std::string& type) override;
        void update(float elapsed) override;
        std::optional<int> process_message(UINT message, WPARAM wParam, LPARAM lParam) override;
        void register_window(const std::string& type, const Creator& creator) override;
        void render() override;
        void setup(const UserSettings& settings) override;
        std::vector<std::weak_ptr<IWindow>> windows(const std::string& type) const override;
    private:
        std::unordered_map<std::string, Creator> _creators;
        IWindow::Source _triggers_window_source;
    };
}
