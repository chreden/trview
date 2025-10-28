#pragma once

#include "IFileMenu.h"
#include "../Elements/Level/ILevelNameLookup.h"

#include <trview.common/Windows/IDialogs.h>
#include <trview.common/IFiles.h>
#include <trview.common/Messages/IRecipient.h>

#include <optional>

namespace trview
{
    class ImGuiFileMenu final : public IFileMenu, public IRecipient
    {
    public:
        using LevelNameSource = std::function<std::optional<ILevelNameLookup::Name>(const std::string&, const std::shared_ptr<trlevel::IPack>&)>;

        explicit ImGuiFileMenu(const std::shared_ptr<IDialogs>& dialogs, const std::shared_ptr<IFiles>& files, const LevelNameSource& level_name_source);
        virtual ~ImGuiFileMenu() = default;
        std::vector<std::string> local_levels() const override;
        void open_file(const std::string& filename, const std::weak_ptr<trlevel::IPack>& pack) override;
        void render() override;
        void set_recent_files(const std::list<std::string>& files);
        void set_sorting_mode(LevelSortingMode mode);
        void switch_to(const std::string& filename) override;
        void receive_message(const Message& message) override;
    private:
        static const inline std::string default_file_pattern{ "\\*.TR2*,\\*.TR4*,\\*.TRC*,\\*.PHD,\\*.PSX,\\*.OBJ,\\*.TOM,\\*.SAT" };
        void sort_level_switcher();

        struct File
        {
            std::string path;
            std::string friendly_name;
            std::optional<ILevelNameLookup::Name> level_name;
        };

        std::shared_ptr<IDialogs> _dialogs;
        std::shared_ptr<IFiles> _files;
        std::vector<File> _file_switcher;
        std::vector<std::string> _recent_files;
        std::optional<std::string> _initial_directory;
        LevelSortingMode _sorting_mode{ LevelSortingMode::Full };
        LevelNameSource _level_name_source;
    };
}
