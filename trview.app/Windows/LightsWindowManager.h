#pragma once

#include "ILightsWindow.h"
#include "ILightsWindowManager.h"
#include <trview.common/Windows/IShortcuts.h>
#include <trview.common/MessageHandler.h>
#include <trview.common/TokenStore.h>

namespace trview
{
    class LightsWindowManager final : public ILightsWindowManager, public MessageHandler
    {
    public:
        LightsWindowManager(const Window& window, const std::shared_ptr<IShortcuts>& shortcuts, const ILightsWindow::Source& lights_window_source);
        virtual ~LightsWindowManager() = default;
        virtual void set_lights(const std::vector<std::weak_ptr<ILight>>& lights) override;
        virtual void set_light_visible(const std::weak_ptr<ILight>& light, bool state) override;
        virtual void set_level_version(trlevel::LevelVersion version) override;
        virtual void render() override;
        virtual void update(float delta) override;
        virtual void set_selected_light(const std::weak_ptr<ILight>& light) override;
        virtual std::optional<int> process_message(UINT message, WPARAM wParam, LPARAM lParam) override;
        virtual std::weak_ptr<ILightsWindow> create_window() override;
        virtual void set_room(uint32_t room) override;
    private:
        TokenStore _token_store;
        std::vector<std::shared_ptr<ILightsWindow>> _windows;
        std::vector<std::weak_ptr<ILightsWindow>> _closing_windows;
        std::vector<std::weak_ptr<ILight>> _lights;
        ILightsWindow::Source _lights_window_source;
        std::weak_ptr<ILight> _selected_light;
        trlevel::LevelVersion _level_version{ trlevel::LevelVersion::Tomb1 };
        uint32_t _current_room{ 0u };
        uint32_t _window_count{ 0u };
    };
}
