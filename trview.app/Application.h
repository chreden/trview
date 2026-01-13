#pragma once

#include <future>

#include <trview.common/Window.h>
#include <trview.common/Timer.h>
#include <trview.common/TokenStore.h>

#include "Elements/ITypeInfoLookup.h"
#include <trview.app/Menus/IFileMenu.h>
#include <trview.app/Menus/IUpdateChecker.h>
#include <trview.app/Menus/ViewMenu.h>
#include <trview.app/Routing/IRoute.h>
#include "Routing/IRandomizerRoute.h"
#include <trview.app/Settings/ISettingsLoader.h>
#include <trview.app/Settings/IStartupOptions.h>
#include <trview.app/Windows/IViewer.h>
#include <trview.common/Windows/IDialogs.h>
#include <trview.common/Windows/IShortcuts.h>
#include "UI/IImGuiBackend.h"
#include "Plugins/IPlugins.h"
#include "UI/Fonts/IFonts.h"
#include <trview.common/Messages/IRecipient.h>
#include <trview.common/Messages/IMessageSystem.h>

struct ImFont;

namespace trview
{
    struct IWindows;

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
        virtual std::weak_ptr<IViewer> viewer() const = 0;
        Event<> on_closing;
    };

    class Application final : public IApplication, public MessageHandler, public IRecipient
    {
    public:
        enum class LoadMode
        {
            Async,
            Sync
        };

        explicit Application(
            const Window& application_window,
            std::unique_ptr<IUpdateChecker> update_checker,
            std::shared_ptr<ISettingsLoader> settings_loader,
            const std::shared_ptr<IFileMenu>& file_menu,
            const std::shared_ptr<IViewer>& viewer,
            const IRoute::Source& route_source,
            std::shared_ptr<IShortcuts> shortcuts,
            const ILevel::Source& level_source,
            std::shared_ptr<IStartupOptions> startup_options,
            std::shared_ptr<IDialogs> dialogs,
            std::shared_ptr<IFiles> files,
            std::shared_ptr<IImGuiBackend> imgui_backend,
            std::shared_ptr<IPlugins> plugins,
            const IRandomizerRoute::Source& randomizer_route_source,
            std::shared_ptr<IFonts> fonts,
            const std::shared_ptr<IWindows>& windows,
            LoadMode load_mode,
            const std::shared_ptr<IMessageSystem>& messaging);
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
        std::weak_ptr<IViewer> viewer() const override;
        void receive_message(const Message& message) override;
    private:
        // Window setup functions.
        void setup_view_menu();
        void setup_viewer(const IStartupOptions& startup_options);
        void setup_shortcuts();
        // Entity manipulation
        void add_waypoint(const DirectX::SimpleMath::Vector3& position, const DirectX::SimpleMath::Vector3& normal, std::weak_ptr<IRoom> room, IWaypoint::Type type, uint32_t index);
        void add_waypoint(const DirectX::SimpleMath::Vector3& position, const DirectX::SimpleMath::Vector3& normal, uint32_t room, IWaypoint::Type type, uint32_t index);
        void remove_waypoint(uint32_t index);
        void select_next_waypoint();
        void select_previous_waypoint();
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
        void check_load();
        void end_diff(const std::weak_ptr<ILevel>& level);

        TokenStore _token_store;

        // Window message related components.
        std::shared_ptr<ISettingsLoader> _settings_loader;
        UserSettings _settings;
        std::shared_ptr<IFileMenu> _file_menu;
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
        std::shared_ptr<IViewer> _viewer;
        std::shared_ptr<IWindows> _windows;
        Timer _timer;
        std::optional<std::pair<std::string, FontSetting>> _new_font;

        std::shared_ptr<IImGuiBackend> _imgui_backend;
        std::string _imgui_ini_filename;
        bool _recent_route_prompted{ false };

        std::shared_ptr<IPlugins> _plugins;

        IRandomizerRoute::Source _randomizer_route_source;
        std::shared_ptr<IFonts> _fonts;

        struct LoadOperation
        {
            std::shared_ptr<ILevel>     level;
            std::string                 filename;
            ILevel::OpenMode            open_mode;
            std::optional<std::string>  error;
        };

        std::future<LoadOperation> _load;
        LoadMode _load_mode;
        std::string _progress;
        std::optional<std::string> _route_directory;

        std::shared_ptr<IMessageSystem> _messaging;
    };

    std::shared_ptr<IApplication> create_application(HINSTANCE hInstance, int command_show, const std::wstring& command_line);
}
