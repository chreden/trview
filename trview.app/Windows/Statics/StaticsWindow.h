#pragma once

#include <vector>

#include <trview.common/TokenStore.h>
#include <trview.common/Windows/IClipboard.h>
#include <trview.common/Messages/IMessageSystem.h>

#include "../../Elements/IRoom.h"
#include "../../Elements/IStaticMesh.h"
#include "../../Filters/Filters.h"
#include "../../Settings/UserSettings.h"
#include "../../Track/Track.h"
#include "../AutoHider.h"
#include "../IWindow.h"

namespace trview
{
    class StaticsWindow final : public IWindow, public std::enable_shared_from_this<IRecipient>
    {
    public:
        struct Names
        {
            static inline const std::string sync_item = "Sync";
            static inline const std::string statics_list = "##staticslist";
            static inline const std::string statics_list_panel = "Statics List";
            static inline const std::string details_panel = "Static Details";
            static inline const std::string static_stats = "##staticstats";
        };

        explicit StaticsWindow(const std::shared_ptr<IClipboard>& clipboard, const std::weak_ptr<IMessageSystem>& messaging);
        virtual ~StaticsWindow() = default;
        void initialise();
        void render() override;
        std::weak_ptr<IStaticMesh> selected_static() const;
        void set_current_room(const std::weak_ptr<IRoom>& room);
        void set_number(int32_t number) override;
        void set_selected_static(const std::weak_ptr<IStaticMesh>& static_mesh);
        void set_statics(const std::vector<std::weak_ptr<IStaticMesh>>& statics);
        void update(float dt) override;
        void receive_message(const Message& message) override;
        std::string type() const override;
        std::string title() const override;
    private:
        bool render_statics_window();
        void render_statics_list();
        void render_static_details();
        void setup_filters();
        void set_local_selected_static_mesh(std::weak_ptr<IStaticMesh> static_mesh);
        void set_sync_static(bool value);

        std::string _id{ "Statics 0" };
        std::vector<std::weak_ptr<IStaticMesh>> _all_statics;
        Filters _filters;
        std::weak_ptr<IStaticMesh> _selected_static_mesh;
        bool _sync_static{ true };
        Track<Type::Room> _track;
        std::weak_ptr<IRoom> _current_room;
        std::weak_ptr<IStaticMesh> _global_selected_static;
        std::shared_ptr<IClipboard> _clipboard;
        AutoHider _auto_hider;
        TokenStore _token_store;
        std::optional<UserSettings> _settings;
        bool _columns_set{ false };
        std::weak_ptr<IMessageSystem> _messaging;
        std::weak_ptr<ILevel> _level;
    };
}

