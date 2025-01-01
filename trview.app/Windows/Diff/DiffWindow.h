#pragma once

#include <future>
#include <trview.common/Windows/IDialogs.h>

#include "../../Elements/ILevel.h"
#include "IDiffWindow.h"
#include "../ColumnSizer.h"

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
        void render() override;
        void set_level(const std::weak_ptr<ILevel>& level) override;
        void set_number(int32_t number) override;

        struct Diff
        {
            enum class State
            {
                Unresolved,
                Resolved
            };

            enum class Type
            {
                None,
                Add,
                Update,
                Move,
                Delete
            };

            struct Item
            {
                State state{ State::Unresolved };
                Type type{ Type::None };
                std::weak_ptr<IItem> left;
                std::weak_ptr<IItem> right;
            };

            std::vector<Item> items;
        };
    private:
        struct LoadOperation
        {
            std::shared_ptr<ILevel>     level;
            std::string                 filename;
            std::optional<std::string>  error;
            Diff                        diff;
        };

        void render_diff_details();
        bool render_diff_window();
        void start_load(const std::string& filename);
        std::shared_ptr<ILevel> load_level(const std::string& filename);
        bool loading();
        void calculate_column_widths();

        std::string _id{ "Diff 0" };
        std::string _progress;
        ILevel::Source _level_source;
        std::future<LoadOperation> _load;
        std::optional<LoadOperation> _diff;
        std::shared_ptr<IDialogs> _dialogs;
        std::weak_ptr<ILevel> _level;
        ColumnSizer _column_sizer;
    };
}
