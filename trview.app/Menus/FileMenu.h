#pragma once

#include <trview.common/TokenStore.h>
#include <trview.common/MessageHandler.h>
#include <trview.common/IFiles.h>
#include "IFileMenu.h"

namespace trview
{
    class FileMenu final : public IFileMenu, public MessageHandler
    {
    public:
        static const inline std::string default_file_pattern{ "\\*.TR2*,\\*.TR4*,\\*.TRC*,\\*.PHD,\\*.PSX,\\*.OBJ" };

        using LevelNameSource = std::function<std::optional<std::string>(const std::string&, const std::shared_ptr<trlevel::IPack>&)>;

        explicit FileMenu(
            const Window& window,
            const std::shared_ptr<IShortcuts>& shortcuts,
            const std::shared_ptr<IDialogs>& dialogs,
            const std::shared_ptr<IFiles>& files,
            const LevelNameSource& level_name_source);
        virtual ~FileMenu() = default;
        std::vector<std::string> local_levels() const override;
        std::optional<int> process_message(UINT message, WPARAM wParam, LPARAM lParam) override;
        void open_file(const std::string& filename, const std::weak_ptr<trlevel::IPack>& pack) override;
        void render() override;
        void set_recent_files(const std::list<std::string>& files) override;
        void switch_to(const std::string& filename) override;
    private:
        void choose_file();
        void next_directory_file();
        void previous_directory_file();

        struct File
        {
            std::string path;
            std::string friendly_name;
            std::optional<std::string> level_name;
        };

        TokenStore _token_store;
        std::shared_ptr<IDialogs> _dialogs;
        HMENU             _directory_listing_menu;
        std::vector<std::string> _recent_files;
        std::vector<File> _file_switcher_list;
        std::string _opened_file;
        std::shared_ptr<IFiles> _files;
        std::optional<std::string> _initial_directory;
        LevelNameSource _level_name_source;
    };
}
