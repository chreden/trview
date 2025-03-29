#pragma once

#include "IFileMenu.h"

#include <trview.common/Windows/IDialogs.h>
#include <trview.common/IFiles.h>

#include <optional>

namespace trview
{
    class ImGuiFileMenu final : public IFileMenu
    {
    public:
        explicit ImGuiFileMenu(const std::shared_ptr<IDialogs>& dialogs, const std::shared_ptr<IFiles>& files);
        virtual ~ImGuiFileMenu() = default;
        std::vector<std::string> local_levels() const override;
        void open_file(const std::string& filename) override;
        void render() override;
        void set_recent_files(const std::list<std::string>& files) override;
        void switch_to(const std::string& filename) override;
    private:
        static const inline std::string default_file_pattern{ "\\*.TR2*,\\*.TR4*,\\*.TRC*,\\*.PHD,\\*.PSX,\\*.OBJ" };

        std::shared_ptr<IDialogs> _dialogs;
        std::shared_ptr<IFiles> _files;
        std::vector<IFiles::File> _file_switcher;
        std::vector<std::string> _recent_files;
        std::optional<std::string> _initial_directory;
    };
}
