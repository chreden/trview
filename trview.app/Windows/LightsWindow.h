#pragma once

#include <trview.common/TokenStore.h>
#include <trview.common/Windows/IClipboard.h>
#include "../Elements/ILight.h"
#include "../Filters/Filters.h"
#include "../Track/Track.h"
#include "AutoHider.h"
#include "IWindow.h"

#include "../Settings/UserSettings.h"

#include <trview.common/Messages/IMessageSystem.h>

namespace trview
{
    class LightsWindow final : public IWindow, public std::enable_shared_from_this<IRecipient>
    {
    public:
        struct Names
        {
            static inline const std::string light_list_panel = "Light List";
            static inline const std::string lights_listbox = "Lights";
            static inline const std::string stats_listbox = "Stats";
            static inline const std::string sync_light = "Sync";
            static inline const std::string track_room = "Track Room";
            static inline const std::string details_panel = "Light Details";
        };

        explicit LightsWindow(const std::shared_ptr<IClipboard>& clipboard, const std::weak_ptr<IMessageSystem>& messaging);
        virtual ~LightsWindow() = default;
        void clear_selected_light();
        void render() override;
        void update(float delta) override;
        void set_lights(const std::vector<std::weak_ptr<ILight>>& lights);
        void set_selected_light(const std::weak_ptr<ILight>& light);
        void set_level_version(trlevel::LevelVersion version);
        void set_number(int32_t number) override;
        void set_current_room(const std::weak_ptr<IRoom>& room);
        void receive_message(const Message& message) override;
        void initialise();
        std::string type() const override;
        std::string title() const override;
    private:
        void set_sync_light(bool value);
        void set_local_selected_light(const std::weak_ptr<ILight>& light);
        void render_lights_list();
        void render_light_details();
        bool render_lights_window();
        void setup_filters();

        std::vector<std::weak_ptr<ILight>> _all_lights;
        std::shared_ptr<IClipboard> _clipboard;
        bool _sync_light{ true };
        trlevel::LevelVersion _level_version{ trlevel::LevelVersion::Tomb5 };
        std::optional<float> _tooltip_timer;
        std::string _id{ "Lights 0" };
        std::weak_ptr<ILight> _selected_light;
        std::weak_ptr<ILight> _global_selected_light;
        std::weak_ptr<IRoom> _current_room;
        std::unordered_map<std::string, std::string> _tips;
        Filters _filters;
        Track<Type::Room> _track;
        AutoHider _auto_hider;
        std::optional<UserSettings> _settings;
        TokenStore _token_store;
        bool _columns_set{ false };
        std::weak_ptr<IMessageSystem> _messaging;
        std::weak_ptr<ILevel> _level;
    };
}
