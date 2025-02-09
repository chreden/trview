#pragma once

#include <future>
#include <string>
#include <trview.common/Windows/IDialogs.h>
#include <trview.common/TokenStore.h>

#include "../../Menus/IFileMenu.h"
#include "../../Elements/ILevel.h"
#include "IDiffWindow.h"
#include "../ColumnSizer.h"
#include "../../Settings/UserSettings.h"

namespace trview
{
    class DiffWindow final : public IDiffWindow
    {
    public:
        struct Names
        {
        };

        explicit DiffWindow(const std::shared_ptr<IDialogs>& dialogs, const ILevel::Source& level_source, std::unique_ptr<IFileMenu> file_menu);
        virtual ~DiffWindow() = default;
        void render() override;
        void set_level(const std::weak_ptr<ILevel>& level) override;
        void set_number(int32_t number) override;
        void set_settings(const UserSettings& settings) override;

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
                Reindex,
                Delete
            };

            template <typename T>
            struct Item
            {
                State state{ State::Unresolved };
                Type type{ Type::None };
                std::weak_ptr<T> left;
                std::weak_ptr<T> right;
            };

            std::vector<Item<IItem>> items;
            std::vector<Item<ITrigger>> triggers;
            std::vector<Item<ILight>> lights;
            std::vector<Item<ICameraSink>> camera_sinks;
            std::vector<Item<IStaticMesh>> static_meshes;
            std::vector<Item<ISoundSource>> sound_sources;
            std::vector<Item<IRoom>> rooms;
            std::vector<Item<ISector>> sectors;
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
        Diff do_diff(const std::shared_ptr<ILevel>& left, const std::shared_ptr<ILevel>& right);
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
        std::unique_ptr<IFileMenu> _file_menu;
        TokenStore _token_store;
        UserSettings _settings;
        bool _only_show_changes{ false };
    };
}
