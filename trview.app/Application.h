#pragma once

#include <trview.app/Windows/Viewer.h>
#include <trview.common/Window.h>
#include <trview.app/Menus/FileDropper.h>
#include <trview.app/Menus/LevelSwitcher.h>
#include <trview.app/Menus/RecentFiles.h>

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
        Viewer _viewer;
        UserSettings _settings;
        FileDropper _file_dropper;
        LevelSwitcher _level_switcher;
        RecentFiles _recent_files;
        TokenStore _token_store;
        HINSTANCE _instance{ nullptr };
    };
}
