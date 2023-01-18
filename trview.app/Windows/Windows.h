#pragma once

#include "IItemsWindowManager.h"
#include "ILightsWindowManager.h"
#include "IRoomsWindowManager.h"
#include "ITriggersWindowManager.h"
#include "IWindows.h"

namespace trview
{
    class Windows final : public IWindows
    {
    public:
        Windows(
            std::unique_ptr<IItemsWindowManager> items_windows,
            std::unique_ptr<ILightsWindowManager> lights_windows,
            std::unique_ptr<IRoomsWindowManager> rooms_windows,
            std::unique_ptr<ITriggersWindowManager> triggers_windows);
        virtual ~Windows();
        void render() override;
        void set_item_visibility(const Item& item, bool value) override;
        void set_level(std::shared_ptr<ILevel> level) override;
        void set_room(uint32_t room) override;
        void set_selected_camera_sink(std::weak_ptr<ICameraSink> camera_sink) override;
        void set_selected_item(const Item& item) override;
        void set_selected_light(std::weak_ptr<ILight> light) override;
        void set_selected_trigger(std::weak_ptr<ITrigger> trigger) override;
        void set_settings(const UserSettings& settings) override;
        void startup(const UserSettings& settings) override;
        void update(float elapsed) override;
    private:
        TokenStore _token_store;
        std::unique_ptr<IItemsWindowManager> _items_windows;
        std::unique_ptr<ILightsWindowManager> _lights_windows;
        std::unique_ptr<IRoomsWindowManager> _rooms_windows;
        std::unique_ptr<ITriggersWindowManager> _triggers_windows;
        std::weak_ptr<ILevel> _level;
    };
}
