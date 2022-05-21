#pragma once

#include <trview.common/TokenStore.h>
#include <trview.common/MessageHandler.h>
#include "IFileMenu.h"
#include "DirectoryListing.h"

namespace trview
{
    class FileMenu final : public IFileMenu, public MessageHandler
    {
    public:
        explicit FileMenu(const Window& window, const std::shared_ptr<IShortcuts>& shortcuts, const std::shared_ptr<IDialogs>& dialogs);
        virtual ~FileMenu() = default;
        virtual std::optional<int> process_message(UINT message, WPARAM wParam, LPARAM lParam) override;
        virtual void open_file(const std::string& filename) override;
        virtual void set_recent_files(const std::list<std::string>& files) override;
    private:
        void choose_file();

        TokenStore _token_store;
        std::shared_ptr<IDialogs> _dialogs;
        HMENU             _directory_listing_menu;
        std::vector<std::string> _recent_files;
        std::vector<File> _file_switcher_list;
    };
}
