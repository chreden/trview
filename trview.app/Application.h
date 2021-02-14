#pragma once

#include <trview.common/Window.h>

#include <trview.app/Windows/Viewer.h>
#include <trview.app/Menus/FileDropper.h>
#include <trview.app/Menus/LevelSwitcher.h>
#include <trview.app/Menus/RecentFiles.h>
#include <trview.app/Menus/UpdateChecker.h>
#include <trview.app/Elements/Level.h>
#include <trview.app/Elements/ITypeNameLookup.h>

#include <trview.graphics/Device.h>
#include <trview.graphics/IShaderStorage.h>

namespace trview
{
    class Application : MessageHandler
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
        UserSettings _settings;
        FileDropper _file_dropper;
        LevelSwitcher _level_switcher;
        RecentFiles _recent_files;
        UpdateChecker _update_checker;
        TokenStore _token_store;
        graphics::Device _device;
        std::unique_ptr<Viewer> _viewer;
        std::unique_ptr<ITypeNameLookup> _type_name_lookup;
        std::unique_ptr<graphics::IShaderStorage> _shader_storage;
        std::unique_ptr<Level> _level;
        HINSTANCE _instance{ nullptr };
    };
}
