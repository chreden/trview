#pragma once

#include <trview.common/Windows/IShortcuts.h>
#include <trview.common/MessageHandler.h>
#include "ILightsWindow.h"
#include "ILightsWindowManager.h"
#include "WindowManager.h"

namespace trview
{
    class LightsWindowManager final : public ILightsWindowManager, WindowManager<ILightsWindow>, public MessageHandler
    {
    public:
        LightsWindowManager(const Window& window, const std::shared_ptr<IShortcuts>& shortcuts, const ILightsWindow::Source& lights_window_source);
        virtual ~LightsWindowManager() = default;
        void add_level(const std::weak_ptr<ILevel>& level) override;
        std::weak_ptr<ILightsWindow> create_window() override;
        std::optional<int> process_message(UINT message, WPARAM wParam, LPARAM lParam) override;
        void render() override;
        void set_selected_light(const std::weak_ptr<ILight>& light) override;
        void set_room(const std::weak_ptr<IRoom>& room) override;
        void update(float delta) override;
    private:
        struct Selection
        {
            std::weak_ptr<ILevel> level;
            std::weak_ptr<IRoom> room;
            std::weak_ptr<ILight> light;
        };

        std::vector<Selection> _levels;
        ILightsWindow::Source _lights_window_source;
    };
}
