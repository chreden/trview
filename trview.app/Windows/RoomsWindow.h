#pragma once

#include "IWindow.h"

#include <trview.common/Windows/IClipboard.h>
#include <trview.common/TokenStore.h>
#include <trview.common/Messages/IMessageSystem.h>

#include "../Settings/UserSettings.h"

#include "../Elements/IItem.h"
#include "../UI/IMapRenderer.h"
#include "../Filters/Filters.h"
#include "../Track/Track.h"
#include "AutoHider.h"

namespace trview
{
    class RoomsWindow final : public IWindow, public std::enable_shared_from_this<IRecipient>
    {
    public:
        struct Names
        {
            static inline const std::string details_panel = "Room Details";
            static inline const std::string properties = "Properties";
            static inline const std::string properties_tab = "Properties##Properties Tab";
            static inline const std::string rooms_panel = "Rooms List";
            static inline const std::string rooms_list = "##roomslist";
            static inline const std::string simple_mode = "Raw";
        };

        /// Create a rooms window as a child of the specified window.
        /// @param device The graphics device
        /// @param renderer_source The function to call to get a renderer.
        explicit RoomsWindow(const IMapRenderer::Source& map_renderer_source, const std::shared_ptr<IClipboard>& clipboard, const std::weak_ptr<IMessageSystem>& messaging);
        virtual ~RoomsWindow() = default;
        void clear_selected_trigger();
        void render() override;
        void set_current_room(const std::weak_ptr<IRoom>& room);
        void set_items(const std::vector<std::weak_ptr<IItem>>& items);
        void set_level_version(trlevel::LevelVersion version);
        void set_rooms(const std::vector<std::weak_ptr<IRoom>>& rooms);
        void set_selected_item(const std::weak_ptr<IItem>& item);
        void set_selected_trigger(const std::weak_ptr<ITrigger>& trigger);
        void update(float delta) override;
        void set_number(int32_t number) override;
        void set_floordata(const std::vector<uint16_t>& data);
        void set_selected_light(const std::weak_ptr<ILight>& light);
        void set_selected_camera_sink(const std::weak_ptr<ICameraSink>& camera_sink);
        void clear_selected_light();
        void clear_selected_camera_sink();
        void set_ng_plus(bool value);
        void set_trng(bool value);
        std::string name() const;
        void set_filters(std::vector<Filters::Filter> filters);
        void set_selected_sector(const std::weak_ptr<ISector>& sector);
        void receive_message(const Message& message) override;
        void initialise();
        std::string type() const override;
        std::string title() const override;
    private:
        void set_sync_room(bool value);
        void render_rooms_list();
        void render_room_details();
        bool render_rooms_window();
        void load_room_details(std::weak_ptr<IRoom> room);
        void generate_filters();
        void render_properties_tab(const std::shared_ptr<IRoom>& room);
        void render_neighbours_tab(const std::shared_ptr<IRoom>& room);
        void render_items_tab(const std::shared_ptr<IRoom>& room);
        void render_triggers_tab();
        void render_sector_tab();
        void render_camera_sink_tab();
        void render_lights_tab();
        void set_triggers(const std::vector<std::weak_ptr<ITrigger>>& triggers);
        void set_lights(const std::vector<std::weak_ptr<ILight>>& lights);
        void set_camera_sinks(const std::vector<std::weak_ptr<ICameraSink>>& camera_sinks);
        void select_room(std::weak_ptr<IRoom> room);
        void render_statics_tab();
        void set_static_meshes(const std::vector<std::weak_ptr<IStaticMesh>>& static_meshes);
        std::optional<Filters> convert_to_sector_filters() const;
        void apply_sector_filters();


        std::vector<std::weak_ptr<IRoom>> _all_rooms;
        std::vector<std::weak_ptr<IItem>> _all_items;
        std::vector<std::weak_ptr<ITrigger>> _triggers;
        std::vector<std::weak_ptr<ILight>> _lights;
        std::vector<std::weak_ptr<ICameraSink>> _camera_sinks;
        std::vector<std::weak_ptr<IStaticMesh>> _static_meshes;

        bool _sync_room{ true };

        std::weak_ptr<IItem> _global_selected_item;
        std::weak_ptr<IItem> _local_selected_item;
        bool _scroll_to_item{ false };

        std::weak_ptr<ITrigger> _global_selected_trigger;
        std::weak_ptr<ITrigger> _local_selected_trigger;
        bool _scroll_to_trigger{ false };

        std::weak_ptr<ILight> _global_selected_light;
        std::weak_ptr<ILight> _local_selected_light;
        bool _scroll_to_light{ false };

        std::weak_ptr<ICameraSink> _global_selected_camera_sink;
        std::weak_ptr<ICameraSink> _local_selected_camera_sink;
        bool _scroll_to_camera_sink{ false };

        std::weak_ptr<IStaticMesh> _local_selected_static_mesh;
        bool _scroll_to_static_mesh{ false };

        std::weak_ptr<ISector> _global_selected_sector;
        std::weak_ptr<ISector> _local_selected_sector;

        std::weak_ptr<IRoom> _current_room;
        std::weak_ptr<IRoom> _selected_room;

        TokenStore _token_store;
        std::shared_ptr<IMapRenderer> _map_renderer;
        std::shared_ptr<IClipboard> _clipboard;
        trlevel::LevelVersion _level_version{ trlevel::LevelVersion::Tomb1 };
        trlevel::PlatformAndVersion _platform_and_version;
        std::string _id{ "Rooms 0" };
        std::optional<float> _tooltip_timer;

        Filters _filters;
        bool _force_sort{ false };
        std::vector<uint16_t> _floordata;
        bool _simple_mode{ true };
        uint32_t _selected_floordata{ 0 };
        Track<Type::Item, Type::Trigger, Type::Light, Type::CameraSink, Type::Sector> _track;

        bool _ng_plus{ false };
        AutoHider _auto_hider;
        bool _trng{ false };

        std::optional<UserSettings> _settings;
        bool _columns_set{ false };
        std::weak_ptr<IMessageSystem> _messaging;

        std::weak_ptr<ILevel> _level;
    };
}
