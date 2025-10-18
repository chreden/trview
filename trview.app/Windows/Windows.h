#pragma once

#include <trview.common/TokenStore.h>
#include "../Routing/IWaypoint.h"

#include "IWindows.h"

namespace trview
{
    struct IAboutWindowManager;
    struct ICameraSinkWindowManager;
    struct IConsoleManager;
    struct IDiffWindowManager;
    struct IItemsWindowManager;
    struct ILightsWindowManager;
    struct ILogWindowManager;
    struct IPluginsWindowManager;
    struct IRoomsWindowManager;
    struct IRouteWindowManager;
    struct ISoundsWindowManager;
    struct IStaticsWindowManager;
    struct ITexturesWindowManager;
    struct ITriggersWindowManager;
    struct IPackWindowManager;

    class Windows final : public IWindows
    {
    public:
        explicit Windows(
            std::unique_ptr<IAboutWindowManager> about_window_manager,
            std::unique_ptr<ICameraSinkWindowManager> camera_sink_windows,
            std::unique_ptr<IConsoleManager> console_manager,
            std::unique_ptr<IDiffWindowManager> diff_window_manager,
            std::shared_ptr<IItemsWindowManager> items_window_manager,
            std::unique_ptr<ILightsWindowManager> lights_window_manager,
            std::unique_ptr<ILogWindowManager> log_window_manager,
            std::unique_ptr<IPackWindowManager> pack_window_manager,
            std::unique_ptr<IPluginsWindowManager> plugins_window_manager,
            std::shared_ptr<IRoomsWindowManager> rooms_window_manager,
            std::unique_ptr<IRouteWindowManager> route_window_manager,
            std::unique_ptr<ISoundsWindowManager> sounds_window_manager,
            std::unique_ptr<IStaticsWindowManager> statics_window_manager,
            std::unique_ptr<ITexturesWindowManager> textures_window_manager,
            std::unique_ptr<ITriggersWindowManager> triggers_window_manager);
        virtual ~Windows() = default;
        bool is_route_window_open() const override;
        void update(float elapsed) override;
        void render() override;
        void select(const std::weak_ptr<IWaypoint>& waypoint) override;
        void set_level(const std::weak_ptr<ILevel>& level) override;
        void set_room(const std::weak_ptr<IRoom>& room) override;
        void set_route(const std::weak_ptr<IRoute>& route) override;
        void setup(const UserSettings& settings) override;
    private:
        void add_waypoint(const DirectX::SimpleMath::Vector3& position, const DirectX::SimpleMath::Vector3& normal, uint32_t room, IWaypoint::Type type, uint32_t index);

        TokenStore _token_store;
        TokenStore _level_token_store;
        std::unique_ptr<IAboutWindowManager> _about_windows;
        std::unique_ptr<ICameraSinkWindowManager> _camera_sink_windows;
        std::unique_ptr<IConsoleManager> _console_manager;
        std::unique_ptr<IDiffWindowManager> _diff_windows;
        std::shared_ptr<IItemsWindowManager> _items_windows;
        std::unique_ptr<ILightsWindowManager> _lights_windows;
        std::unique_ptr<ILogWindowManager> _log_windows;
        std::unique_ptr<IPackWindowManager> _pack_windows;
        std::unique_ptr<IPluginsWindowManager> _plugins_windows;
        std::shared_ptr<IRoomsWindowManager> _rooms_windows;
        std::weak_ptr<IRoute> _route;
        std::unique_ptr<IRouteWindowManager> _route_window;
        std::unique_ptr<ISoundsWindowManager> _sounds_windows;
        std::unique_ptr<IStaticsWindowManager> _statics_windows;
        std::unique_ptr<ITexturesWindowManager> _textures_windows;
        std::unique_ptr<ITriggersWindowManager> _triggers_windows;
    };
}
