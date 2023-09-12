#pragma once

#include "IRoomsWindow.h"

#include <trview.common/Windows/IClipboard.h>
#include <trview.common/TokenStore.h>

#include "../UI/Tooltip.h"
#include "../Elements/IItem.h"
#include "../UI/IMapRenderer.h"
#include "../Filters/Filters.h"
#include "../Track/Track.h"

namespace trview
{
    class RoomsWindow final : public IRoomsWindow
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
        explicit RoomsWindow(const IMapRenderer::Source& map_renderer_source, const std::shared_ptr<IClipboard>& clipboard);
        virtual ~RoomsWindow() = default;
        virtual void clear_selected_trigger() override;
        virtual void render() override;
        virtual void set_current_room(uint32_t room) override;
        virtual void set_items(const std::vector<std::weak_ptr<IItem>>& items) override;
        virtual void set_level_version(trlevel::LevelVersion version) override;
        virtual void set_map_colours(const MapColours& colours) override;
        virtual void set_rooms(const std::vector<std::weak_ptr<IRoom>>& rooms) override;
        virtual void set_selected_item(const std::weak_ptr<IItem>& item) override;
        virtual void set_selected_trigger(const std::weak_ptr<ITrigger>& trigger) override;
        virtual void update(float delta) override;
        virtual void set_number(int32_t number) override;
        virtual void set_floordata(const std::vector<uint16_t>& data) override;
        virtual void set_selected_light(const std::weak_ptr<ILight>& light) override;
        virtual void set_selected_camera_sink(const std::weak_ptr<ICameraSink>& camera_sink) override;
        virtual void clear_selected_light() override;
        virtual void clear_selected_camera_sink() override;
    private:
        void set_sync_room(bool value);
        void render_rooms_list();
        void render_room_details();
        bool render_rooms_window();
        void load_room_details(uint32_t room_number);
        void generate_filters();
        void render_properties_tab(const std::shared_ptr<IRoom>& room);
        void render_neighbours_tab(const std::shared_ptr<IRoom>& room);
        void render_items_tab(const std::shared_ptr<IRoom>& room);
        void render_triggers_tab();
        void render_floordata_tab(const std::shared_ptr<IRoom>& room);
        void set_selected_sector(const std::shared_ptr<ISector>& sector);
        void render_camera_sink_tab();
        void render_lights_tab();
        void set_triggers(const std::vector<std::weak_ptr<ITrigger>>& triggers);
        void set_lights(const std::vector<std::weak_ptr<ILight>>& lights);
        void set_camera_sinks(const std::vector<std::weak_ptr<ICameraSink>>& camera_sinks);
        void select_room(uint32_t room);
        void render_statics_tab();
        void set_static_meshes(const std::vector<std::weak_ptr<IStaticMesh>>& static_meshes);

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

        uint32_t _current_room{ 0u };
        uint32_t _selected_room{ 0u };

        TokenStore _token_store;
        std::unique_ptr<IMapRenderer> _map_renderer;
        std::shared_ptr<IClipboard> _clipboard;
        trlevel::LevelVersion _level_version{ trlevel::LevelVersion::Tomb1 };
        std::string _id{ "Rooms 0" };
        Tooltip _map_tooltip;
        bool _scroll_to_room{ false };
        std::optional<float> _tooltip_timer;

        // Map texture saved so that it is guaranteed to survive until ImGui renders.
        graphics::Texture _map_texture;

        Filters<IRoom> _filters;
        bool _force_sort{ false };
        std::vector<uint16_t> _floordata;
        bool _simple_mode{ true };
        bool _in_floordata_mode{ false };
        std::weak_ptr<ISector> _selected_sector;
        uint32_t _selected_floordata{ 0 };
        Track<Type::Item, Type::Trigger, Type::Light, Type::CameraSink> _track;
    };
}
