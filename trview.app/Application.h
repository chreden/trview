#pragma once

#include <trview.common/Window.h>

#include <trview.app/Elements/Level.h>
#include <trview.app/Elements/ITypeNameLookup.h>
#include <trview.app/Menus/FileDropper.h>
#include <trview.app/Menus/LevelSwitcher.h>
#include <trview.app/Menus/RecentFiles.h>
#include <trview.app/Menus/UpdateChecker.h>
#include <trview.app/Routing/Route.h>
#include <trview.app/Windows/ItemsWindowManager.h>
#include <trview.app/Windows/RoomsWindowManager.h>
#include <trview.app/Windows/RouteWindowManager.h>
#include <trview.app/Windows/TriggersWindowManager.h>
#include <trview.app/Windows/Viewer.h>

#include <trview.graphics/Device.h>
#include <trview.graphics/IShaderStorage.h>

namespace trview
{
    class Application : public MessageHandler
    {
    public:
        explicit Application(HINSTANCE hInstance, const std::wstring& command_line, int command_show);
        virtual ~Application();

        /// Event raised when the recent files list is updated.
        /// @remarks The list of filenames is passed as a parameter to the listener functions.
        Event<std::list<std::string>> on_recent_files_changed;

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
        void setup_viewer(const std::wstring& command_line);
        void setup_items_windows();
        void setup_triggers_windows();
        void setup_rooms_windows();
        void setup_route_window();

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

        TokenStore _token_store;

        // Window message related components.
        UserSettings _settings;
        FileDropper _file_dropper;
        LevelSwitcher _level_switcher;
        RecentFiles _recent_files;
        UpdateChecker _update_checker;
        Shortcuts _shortcuts;
        HINSTANCE _instance{ nullptr };

        // Rendering
        graphics::Device _device;
        std::unique_ptr<graphics::IShaderStorage> _shader_storage;
        graphics::FontFactory _font_factory;
        std::unique_ptr<ITextureStorage> _texture_storage;

        // Level data components
        std::unique_ptr<ITypeNameLookup> _type_name_lookup;
        std::unique_ptr<Level> _level;

        // Routing and tools.
        std::unique_ptr<Route> _route;

        // Windows
        std::unique_ptr<Viewer> _viewer;
        std::unique_ptr<ItemsWindowManager> _items_windows;
        std::unique_ptr<TriggersWindowManager> _triggers_windows;
        std::unique_ptr<RouteWindowManager> _route_window;
        std::unique_ptr<RoomsWindowManager> _rooms_windows;
    };
}
