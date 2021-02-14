#pragma once

#include <trview.app/Windows/Viewer.h>
#include <trview.common/Window.h>
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

        /// Attempt to open the specified level file.
        /// @param filename The level file to open.
        void open(const std::string& filename);

        virtual void process_message(UINT message, WPARAM wParam, LPARAM lParam) override;

        int run();
    private:
        Viewer _viewer;
        HINSTANCE _instance{ nullptr };
        UserSettings _settings;
        RecentFiles _recent_files;
        TokenStore _token_store;
    };
}
