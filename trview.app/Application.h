#pragma once

#include <trview.common/Window.h>
#include <trview.common/Timer.h>

#include <trlevel/ILevel.h>

#include "Elements/ITypeInfoLookup.h"
#include <trview.app/Menus/IFileMenu.h>
#include <trview.app/Menus/IUpdateChecker.h>
#include <trview.app/Menus/ViewMenu.h>
#include <trview.app/Routing/Route.h>
#include "Routing/IRandomizerRoute.h"
#include <trview.app/Settings/ISettingsLoader.h>
#include <trview.app/Settings/IStartupOptions.h>
#include <trview.app/Windows/IItemsWindowManager.h>
#include <trview.app/Windows/IRoomsWindowManager.h>
#include <trview.app/Windows/IRouteWindowManager.h>
#include <trview.app/Windows/ITriggersWindowManager.h>
#include <trview.app/Windows/ILightsWindowManager.h>
#include <trview.app/Windows/IViewer.h>
#include <trview.common/Windows/IDialogs.h>
#include <trview.common/Windows/IShortcuts.h>
#include "Windows/Log/ILogWindowManager.h"
#include "Windows/Textures/ITexturesWindowManager.h"
#include "UI/IImGuiBackend.h"
#include "Windows/CameraSink/ICameraSinkWindowManager.h"
#include "Windows/Console/IConsoleManager.h"
#include "Plugins/IPlugins.h"
#include "Windows/Plugins/IPluginsWindowManager.h"
#include "UI/Fonts/IFonts.h"

struct ImFont;

namespace trview
{
    struct IApplication
    {
        virtual ~IApplication() = 0;
        virtual int run() = 0;
        virtual std::weak_ptr<ILevel> current_level() const = 0;
        virtual std::shared_ptr<ILevel> load(const std::string& filename) = 0;
        virtual std::vector<std::string> local_levels() const = 0;
        virtual std::shared_ptr<IRoute> route() const = 0;
        virtual void set_current_level(const std::shared_ptr<ILevel>& level, ILevel::OpenMode open_mode, bool prompt_user) = 0;
        virtual void set_route(const std::shared_ptr<IRoute>& route) = 0;
        virtual UserSettings settings() const = 0;
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
            std::unique_ptr<ILogWindowManager> log_window_manager,
            std::unique_ptr<ITexturesWindowManager> textures_window_manager,
            std::unique_ptr<ICameraSinkWindowManager> camera_sink_window_manager,
            std::unique_ptr<IConsoleManager> console_manager,
            std::shared_ptr<IPlugins> plugins,
            std::unique_ptr<IPluginsWindowManager> plugins_window_manager,
            const IRandomizerRoute::Source& randomizer_route_source,
            std::shared_ptr<IFonts> fonts);
        virtual ~Application();
        /// Attempt to open the specified level file.
        /// @param filename The level file to open.
        void open(const std::string& filename, ILevel::OpenMode open_mode);
        virtual std::optional<int> process_message(UINT message, WPARAM wParam, LPARAM lParam) override;
        virtual int run() override;
        void render();
        std::weak_ptr<ILevel> current_level() const override;
        std::shared_ptr<ILevel> load(const std::string& filename) override;
        std::vector<std::string> local_levels() const override;
        std::shared_ptr<IRoute> route() const override;
        void set_current_level(const std::shared_ptr<ILevel>& level, ILevel::OpenMode open_mode, bool prompt_user) override;
        void set_route(const std::shared_ptr<IRoute>& route) override;
        UserSettings settings() const override;
    private:
        // Window setup functions.
        void setup_view_menu();
        void setup_viewer(const IStartupOptions& startup_options);
        void setup_items_windows();
        void setup_triggers_windows();
        void setup_rooms_windows();
        void setup_route_window();
        void setup_lights_windows();
        void setup_camera_sink_windows();
        void setup_shortcuts();
        // Entity manipulation
        void add_waypoint(const DirectX::SimpleMath::Vector3& position, const DirectX::SimpleMath::Vector3& normal, std::weak_ptr<IRoom> room, IWaypoint::Type type, uint32_t index);
        void add_waypoint(const DirectX::SimpleMath::Vector3& position, const DirectX::SimpleMath::Vector3& normal, uint32_t room, IWaypoint::Type type, uint32_t index);
        void remove_waypoint(uint32_t index);
        void select_item(std::weak_ptr<IItem> item);
        void select_room(std::weak_ptr<IRoom> room);
        /// <summary>
        /// Select a trigger in the application. If the trigger is empty, nothing happens.
        /// </summary>
        /// <param name="trigger">The trigger.</param>
        void select_trigger(const std::weak_ptr<ITrigger>& trigger);
        void select_waypoint(uint32_t index);
        void select_next_waypoint();
        void select_previous_waypoint();
        void select_light(const std::weak_ptr<ILight>& light);
        void select_camera_sink(const std::weak_ptr<ICameraSink>& camera_sink);
        void set_item_visibility(const std::weak_ptr<IItem>& item, bool visible);
        void set_trigger_visibility(const std::weak_ptr<ITrigger>& trigger, bool visible);
        void set_light_visibility(const std::weak_ptr<ILight>& light, bool visible);
        void set_room_visibility(const std::weak_ptr<IRoom>& room, bool visible);
        void set_camera_sink_visibility(const std::weak_ptr<ICameraSink>& camera_sink, bool visible);
        void select_sector(const std::weak_ptr<ISector>& sector);
        bool is_rando_route() const;
        bool should_discard_changes();
        void reload();
        void open_route();
        void reload_route();
        void save_route();
        void import_route(const std::string& path, bool is_rando);
        void save_route_as();
        void open_recent_route();
        void save_window_placement();
        void select_static_mesh(const std::weak_ptr<IStaticMesh>& static_mesh);

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
        std::unique_ptr<ITypeInfoLookup> _type_info_lookup;
        std::shared_ptr<ILevel> _level;
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
        ImFont* _font;
        std::optional<FontSetting> _new_font;

        std::unique_ptr<IImGuiBackend> _imgui_backend;
        std::string _imgui_ini_filename;
        std::unique_ptr<ILogWindowManager> _log_windows;
        bool _recent_route_prompted{ false };

        std::unique_ptr<ITexturesWindowManager> _textures_windows;
        std::unique_ptr<ICameraSinkWindowManager> _camera_sink_windows;
        std::unique_ptr<IConsoleManager> _console_manager;
        std::shared_ptr<IPlugins> _plugins;
        std::unique_ptr<IPluginsWindowManager> _plugins_windows;

        IRandomizerRoute::Source _randomizer_route_source;
        std::shared_ptr<IFonts> _fonts;
    };

    std::unique_ptr<IApplication> create_application(HINSTANCE hInstance, int command_show, const std::wstring& command_line);
}
