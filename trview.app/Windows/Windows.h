#pragma once

#include <trview.common/TokenStore.h>
#include <trview.common/Windows/IShortcuts.h>
#include "../Routing/IWaypoint.h"

#include "IWindows.h"
#include "IWindow.h"
#include "WindowManager.h"

namespace trview
{
    struct IDiffWindowManager;
    struct IItemsWindowManager;
    struct IRoomsWindowManager;
    struct IRouteWindowManager;
    struct IPackWindowManager;

    class Windows final : public IWindows, public WindowManager<IWindow>, public MessageHandler
    {
    public:
        explicit Windows(
            const Window& window,
            const IWindow::Source& about_window_source,
            const IWindow::Source& camera_sink_window_source,
            const IWindow::Source& console_window_source,
            const IWindow::Source& diff_window_manager,
            std::shared_ptr<IItemsWindowManager> items_window_manager,
            const IWindow::Source& lights_window_source,
            const IWindow::Source& log_window_source,
            std::unique_ptr<IPackWindowManager> pack_window_manager,
            const IWindow::Source& plugins_window_source,
            std::shared_ptr<IRoomsWindowManager> rooms_window_manager,
            std::unique_ptr<IRouteWindowManager> route_window_manager,
            const IWindow::Source& sounds_window_source,
            const IWindow::Source& statics_window_source,
            const IWindow::Source& textures_window_source,
            const IWindow::Source& triggers_window_source,
            const std::shared_ptr<IShortcuts>& shortcuts);
        virtual ~Windows() = default;
        bool is_route_window_open() const override;
        void update(float elapsed) override;
        std::optional<int> process_message(UINT message, WPARAM wParam, LPARAM lParam) override;
        void render() override;
        void select(const std::weak_ptr<IWaypoint>& waypoint) override;
        void set_level(const std::weak_ptr<ILevel>& level) override;
        void set_route(const std::weak_ptr<IRoute>& route) override;
        void setup(const UserSettings& settings) override;
    private:
        void add_waypoint(const DirectX::SimpleMath::Vector3& position, const DirectX::SimpleMath::Vector3& normal, uint32_t room, IWaypoint::Type type, uint32_t index);

        IWindow::Source _about_window_source;
        IWindow::Source _camera_sink_window_source;
        IWindow::Source _console_window_source;
        IWindow::Source _diff_window_source;
        std::shared_ptr<IItemsWindowManager> _items_windows;
        IWindow::Source _lights_window_source;
        IWindow::Source _log_window_source;
        std::unique_ptr<IPackWindowManager> _pack_windows;
        IWindow::Source _plugins_window_source;
        std::shared_ptr<IRoomsWindowManager> _rooms_windows;
        std::weak_ptr<IRoute> _route;
        std::unique_ptr<IRouteWindowManager> _route_window;
        IWindow::Source _sounds_window_source;
        IWindow::Source _statics_window_source;
        IWindow::Source _textures_window_source;
        IWindow::Source _triggers_window_source;
    };
}
