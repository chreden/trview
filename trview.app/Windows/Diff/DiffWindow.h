#pragma once

#include <future>
#include <trview.common/Windows/IDialogs.h>

#include "../../Elements/ILevel.h"
#include "IDiffWindow.h"

namespace trview
{
    class DiffWindow final : public IDiffWindow
    {
    public:
        struct Names
        {
        };

        explicit DiffWindow(const std::shared_ptr<IDialogs>& dialogs, const ILevel::Source& level_source);
        virtual ~DiffWindow() = default;
        virtual void render() override;
        virtual void set_number(int32_t number) override;
    private:
        struct LoadOperation
        {
            std::shared_ptr<ILevel>     level;
            std::string                 filename;
            std::optional<std::string>  error;
        };

        bool render_diff_window();
        void start_load(const std::string& filename);
        std::shared_ptr<ILevel> load_level(const std::string& filename);

        std::string _id{ "Diff 0" };
        std::string _progress;
        ILevel::Source _level_source;
        std::future<LoadOperation> _load;
        std::shared_ptr<IDialogs> _dialogs;
    };
}
