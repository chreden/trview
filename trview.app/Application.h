#pragma once

#include <trview.common/Window.h>
#include <trview.common/Timer.h>

#include <trlevel/ILevel.h>

#include <trview.app/Elements/ITypeNameLookup.h>
#include <trview.app/Menus/IFileMenu.h>
#include <trview.app/Menus/IUpdateChecker.h>
#include <trview.app/Menus/ViewMenu.h>
#include <trview.app/Routing/Route.h>
#include <trview.app/Settings/ISettingsLoader.h>
#include <trview.app/Settings/IStartupOptions.h>
#include <trview.app/Windows/IItemsWindowManager.h>
#include <trview.app/Windows/IRoomsWindowManager.h>
#include <trview.app/Windows/IRouteWindowManager.h>
#include <trview.app/Windows/ITriggersWindowManager.h>
#include <trview.app/Windows/ILightsWindowManager.h>
#include <trview.app/Windows/IViewer.h>
#include <trview.app/Lua/Lua.h>
#include <trview.common/Windows/IDialogs.h>
#include <trview.common/Windows/IShortcuts.h>
#include "Windows/Log/ILogWindowManager.h"
#include "UI/IImGuiBackend.h"

struct ImFont;

namespace trview
{
    struct IApplication
    {
        virtual ~IApplication() = 0;
        virtual int run() = 0;
        Event<> on_closing;
    };

    class Application final : public IApplication, public MessageHandler
    {
    public:
        explicit Application(
            const Window& application_window,
            std::unique_ptr<IUpdateChecker> update_checker,
            std::shared_ptr<ISettingsLoader> settings_loader,
            const trlevel::ILevel::Source& trlevel_source,
            std::unique_ptr<IFileMenu> file_menu,
            std::unique_ptr<IViewer> viewer,
            const IRoute::Source& route_source,
            std::shared_ptr<IShortcuts> shortcuts,
            std::unique_ptr<IItemsWindowManager> items_window_manager,
            std::unique_ptr<ITriggersWindowManager> triggers_window_manager,
            std::unique_ptr<IRouteWindowManager> route_window_manager,
            std::unique_ptr<IRoomsWindowManager> rooms_window_manager,
            const ILevel::Source& level_source,
            std::shared_ptr<IStartupOptions> startup_options,
            std::shared_ptr<IDialogs> dialogs,
            std::shared_ptr<IFiles> files,
            std::unique_ptr<IImGuiBackend> imgui_backend,
            std::unique_ptr<ILightsWindowManager> lights_window_manager,
            std::unique_ptr<ILogWindowManager> log_window_manager);
        virtual ~Application();
        /// Attempt to open the specified level file.
        /// @param filename The level file to open.
        void open(const std::string& filename, ILevel::OpenMode open_mode);
        virtual std::optional<int> process_message(UINT message, WPARAM wParam, LPARAM lParam) override;
        virtual int run() override;
        void render();
    private:
        // Window setup functions.
        void setup_view_menu();
        void setup_viewer(const IStartupOptions& startup_options);
        void setup_items_windows();
        void setup_triggers_windows();
        void setup_rooms_windows();
        void setup_route_window();
        void setup_lights_windows();
        void setup_shortcuts();
        // Entity manipulation
        void add_waypoint(const DirectX::SimpleMath::Vector3& position, const DirectX::SimpleMath::Vector3& normal, uint32_t room, IWaypoint::Type type, uint32_t index);
        void remove_waypoint(uint32_t index);
        void select_item(const Item& item);
        void select_room(uint32_t room);
        /// <summary>
        /// Select a trigger in the application. If the trigger is empty, nothing happens.
        /// </summary>
        /// <param name="trigger">The trigger.</param>
        void select_trigger(const std::weak_ptr<ITrigger>& trigger);
        void select_waypoint(uint32_t index);
        void select_next_waypoint();
        void select_previous_waypoint();
        void select_light(const std::weak_ptr<ILight>& light);
        void set_item_visibility(const Item& item, bool visible);
        void set_trigger_visibility(const std::weak_ptr<ITrigger>& trigger, bool visible);
        void set_light_visibility(const std::weak_ptr<ILight>& light, bool visible);
        void set_room_visibility(const std::weak_ptr<IRoom>& room, bool visible);
        // Lua
        void register_lua();
        bool should_discard_changes();
        void reload();

        TokenStore _token_store;

        // Window message related components.
        std::shared_ptr<ISettingsLoader> _settings_loader;
        UserSettings _settings;
        trlevel::ILevel::Source _trlevel_source;
        std::unique_ptr<IFileMenu> _file_menu;
        std::unique_ptr<IUpdateChecker> _update_checker;
        ViewMenu _view_menu;
        std::shared_ptr<IShortcuts> _shortcuts;
        std::shared_ptr<IDialogs> _dialogs;
        std::shared_ptr<IFiles> _files;
        HINSTANCE _instance{ nullptr };

        // Level data components
        std::unique_ptr<ITypeNameLookup> _type_name_lookup;
        std::unique_ptr<ILevel> _level;
        ILevel::Source _level_source;

        // Routing and tools.
        IRoute::Source _route_source;
        std::shared_ptr<IRoute> _route;

        // Windows
        std::unique_ptr<IViewer> _viewer;
        std::unique_ptr<IItemsWindowManager> _items_windows;
        std::unique_ptr<ITriggersWindowManager> _triggers_windows;
        std::unique_ptr<IRouteWindowManager> _route_window;
        std::unique_ptr<IRoomsWindowManager> _rooms_windows;
        std::unique_ptr<ILightsWindowManager> _lights_windows;
        Timer _timer;
        bool _imgui_setup{ false };
        ImFont* _font;

        std::unique_ptr<IImGuiBackend> _imgui_backend;
        std::string _imgui_ini_filename;
        std::unique_ptr<ILogWindowManager> _log_windows;
    };

    Window create_window(HINSTANCE hInstance, int command_show);
    std::unique_ptr<IApplication> create_application(const Window& window, const std::wstring& command_line);
}
