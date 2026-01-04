#pragma once

#include <future>
#include <string>
#include <trview.common/Windows/IDialogs.h>
#include <trview.common/TokenStore.h>
#include <trview.common/Messages/IMessageSystem.h>

#include "../../Menus/IFileMenu.h"
#include "../../Elements/ILevel.h"
#include "../IWindow.h"
#include "../../Settings/UserSettings.h"

namespace trview
{
    class DiffWindow final : public IWindow, public std::enable_shared_from_this<IRecipient>
    {
    public:
        struct Names
        {
        };

        explicit DiffWindow(const std::shared_ptr<IDialogs>& dialogs, const ILevel::Source& level_source, const std::shared_ptr<IFileMenu>& file_menu, const std::weak_ptr<IMessageSystem>& messaging);
        virtual ~DiffWindow() = default;
        void initialise();
        void update(float delta) override;
        void render() override;
        void set_level(const std::weak_ptr<ILevel>& level);
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

        void receive_message(const Message& message) override;
        std::string type() const override;
        std::string title() const override;
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

        std::string _id{ "Diff 0" };
        std::string _progress;
        ILevel::Source _level_source;
        std::future<LoadOperation> _load;
        std::optional<LoadOperation> _diff;
        std::shared_ptr<IDialogs> _dialogs;
        std::weak_ptr<ILevel> _level;
        std::shared_ptr<IFileMenu> _file_menu;
        TokenStore _token_store;
        std::optional<UserSettings> _settings;
        bool _only_show_changes{ false };
        std::weak_ptr<IMessageSystem> _messaging;
    };
}
