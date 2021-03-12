#pragma once

#include <trview.common/Window.h>

#include <trlevel/ILevelLoader.h>

#include <trview.app/Elements/Level.h>
#include <trview.app/Elements/ITypeNameLookup.h>
#include <trview.app/Menus/IFileDropper.h>
#include <trview.app/Menus/ILevelSwitcher.h>
#include <trview.app/Menus/IRecentFiles.h>
#include <trview.app/Menus/IUpdateChecker.h>
#include <trview.app/Menus/ViewMenu.h>
#include <trview.app/Routing/Route.h>
#include <trview.app/Settings/ISettingsLoader.h>
#include <trview.app/Windows/ItemsWindowManager.h>
#include <trview.app/Windows/RoomsWindowManager.h>
#include <trview.app/Windows/RouteWindowManager.h>
#include <trview.app/Windows/TriggersWindowManager.h>
#include <trview.app/Windows/IViewer.h>
#include <trview.app/Lua/Lua.h>
#include <trview.app/Graphics/ITextureStorage.h>

#include <trview.graphics/Device.h>
#include <trview.graphics/IShaderStorage.h>

namespace trview
{
    class Application final : public MessageHandler
    {
    public:
        explicit Application(
            const Window& application_window,
            std::unique_ptr<IUpdateChecker> update_checker,
            std::unique_ptr<ISettingsLoader> settings_loader,
            std::unique_ptr<IFileDropper> file_dropper,
            std::unique_ptr<trlevel::ILevelLoader> level_loader,
            std::unique_ptr<ILevelSwitcher> level_switcher,
            std::unique_ptr<IRecentFiles> recent_files,
            std::unique_ptr<IViewer> viewer,
            std::unique_ptr<graphics::IShaderStorage> shader_storage,
            std::unique_ptr<graphics::IFontFactory> font_factory,
            std::unique_ptr<ITextureStorage> texture_storage,
            std::unique_ptr<graphics::Device> device,
            std::unique_ptr<IRoute> route,
            std::unique_ptr<IShortcuts> shortcuts,
            const std::wstring& command_line);
        virtual ~Application();

        /// Event raised when a level file is successfully opened.
        /// @remarks The filename is passed as a parameter to the listener functions.
        Event<std::string> on_file_loaded;

        /// Attempt to open the specified level file.
        /// @param filename The level file to open.
        void open(const std::string& filename);

        virtual void process_message(UINT message, WPARAM wParam, LPARAM lParam) override;

        int run();
    private:
        // Window setup functions.
        void setup_view_menu();
        void setup_viewer(const std::wstring& command_line);
        void setup_items_windows();
        void setup_triggers_windows();
        void setup_rooms_windows();
        void setup_route_window();
        void setup_shortcuts();

        // Entity manipulation
        void add_waypoint(const DirectX::SimpleMath::Vector3& position, uint32_t room, Waypoint::Type type, uint32_t index);
        void remove_waypoint(uint32_t index);
        void select_item(const Item& item);
        void select_room(uint32_t room);
        void select_trigger(const Trigger* const trigger);
        void select_waypoint(uint32_t index);
        void select_next_waypoint();
        void select_previous_waypoint();
        void set_item_visibility(const Item& item, bool visible);
        void set_trigger_visibility(Trigger* const trigger, bool visible);

        // Rendering
        void render();

        // Lua
        void register_lua();

        TokenStore _token_store;

        // Window message related components.
        std::unique_ptr<ISettingsLoader> _settings_loader;
        UserSettings _settings;
        std::unique_ptr<IFileDropper> _file_dropper;
        std::unique_ptr<trlevel::ILevelLoader> _level_loader;
        std::unique_ptr<ILevelSwitcher> _level_switcher;
        std::unique_ptr<IRecentFiles> _recent_files;
        std::unique_ptr<IUpdateChecker> _update_checker;
        ViewMenu _view_menu;
        std::unique_ptr<IShortcuts> _shortcuts;
        HINSTANCE _instance{ nullptr };

        // Rendering
        std::unique_ptr<graphics::Device> _device;
        std::unique_ptr<graphics::IShaderStorage> _shader_storage;
        std::unique_ptr<graphics::IFontFactory> _font_factory;
        std::unique_ptr<ITextureStorage> _texture_storage;

        // Level data components
        std::unique_ptr<ITypeNameLookup> _type_name_lookup;
        std::unique_ptr<Level> _level;

        // Routing and tools.
        std::unique_ptr<IRoute> _route;

        // Windows
        std::unique_ptr<IViewer> _viewer;
        std::unique_ptr<ItemsWindowManager> _items_windows;
        std::unique_ptr<TriggersWindowManager> _triggers_windows;
        std::unique_ptr<RouteWindowManager> _route_window;
        std::unique_ptr<RoomsWindowManager> _rooms_windows;
    };

    Application create_application(HINSTANCE instance, const std::wstring& command_line, int command_show);
}
