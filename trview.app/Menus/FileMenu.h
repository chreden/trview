#pragma once

#include <trview.common/TokenStore.h>
#include <trview.common/MessageHandler.h>
#include <trview.common/IFiles.h>
#include <trview.common/Messages/IMessageSystem.h>
#include "IFileMenu.h"
#include "../Elements/Level/ILevelNameLookup.h"

namespace trview
{
    class FileMenu final : public IFileMenu, public MessageHandler, public IRecipient, public std::enable_shared_from_this<IRecipient>
    {
    public:
        static const inline std::string default_file_pattern{ "\\*.TR2*,\\*.TR4*,\\*.TRC*,\\*.PHD,\\*.PSX,\\*.OBJ,\\*.TOM,\\*.SAT" };

        using LevelNameSource = std::function<std::optional<ILevelNameLookup::Name>(const std::string&, const std::shared_ptr<trlevel::IPack>&)>;

        explicit FileMenu(
            const Window& window,
            const std::shared_ptr<IShortcuts>& shortcuts,
            const std::shared_ptr<IDialogs>& dialogs,
            const std::shared_ptr<IFiles>& files,
            const LevelNameSource& level_name_source,
            const std::weak_ptr<IMessageSystem>& messaging);
        virtual ~FileMenu() = default;
        std::vector<std::string> local_levels() const override;
        std::optional<int> process_message(UINT message, WPARAM wParam, LPARAM lParam) override;
        void open_file(const std::string& filename, const std::weak_ptr<trlevel::IPack>& pack) override;
        void render() override;
        void set_recent_files(const std::list<std::string>& files);
        void set_sorting_mode(LevelSortingMode mode);
        void switch_to(const std::string& filename) override;
        void receive_message(const Message& message) override;
    private:
        void choose_file();
        void next_directory_file();
        void previous_directory_file();
        void sort_level_switcher();
        void update_level_switcher();

        struct File
        {
            std::string path;
            std::string friendly_name;
            std::optional<ILevelNameLookup::Name> level_name;
        };

        TokenStore _token_store;
        std::weak_ptr<IMessageSystem> _messaging;
        std::shared_ptr<IDialogs> _dialogs;
        HMENU             _directory_listing_menu;
        std::vector<std::string> _recent_files;
        std::vector<File> _file_switcher_list;
        std::string _opened_file;
        std::shared_ptr<IFiles> _files;
        std::optional<std::string> _initial_directory;
        LevelNameSource _level_name_source;
        LevelSortingMode _sorting_mode{ LevelSortingMode::FilenameOnly };
    };
}
