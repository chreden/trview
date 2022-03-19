#pragma once

#include "IRoomsWindow.h"

#include <trview.common/Windows/IClipboard.h>
#include <trview.common/TokenStore.h>

#include "../UI/Tooltip.h"
#include "../Elements/Item.h"
#include "../UI/IMapRenderer.h"
#include "../Filters/Filters.h"


namespace trview
{
    class RoomsWindow final : public IRoomsWindow
    {
    public:
        struct Names
        {
            static const std::string sync_room;
            static const std::string track_item;
            static const std::string track_trigger;
            static const std::string rooms_listbox;
            static const std::string triggers_listbox;
            static const std::string stats_listbox;
            static const std::string minimap;
            static const std::string neighbours_listbox;
            static const std::string items_listbox;
            static inline const std::string details_panel = "Room Details";
            static inline const std::string properties = "Properties";
            static inline const std::string bottom = "##bottom";
        };

        /// Create a rooms window as a child of the specified window.
        /// @param device The graphics device
        /// @param renderer_source The function to call to get a renderer.
        explicit RoomsWindow(const IMapRenderer::Source& map_renderer_source, const std::shared_ptr<IClipboard>& clipboard);
        virtual ~RoomsWindow() = default;
        virtual void clear_selected_trigger() override;
        virtual void render() override;
        virtual void set_current_room(uint32_t room) override;
        virtual void set_items(const std::vector<Item>& items) override;
        virtual void set_level_version(trlevel::LevelVersion version) override;
        virtual void set_map_colours(const MapColours& colours) override;
        virtual void set_rooms(const std::vector<std::weak_ptr<IRoom>>& rooms) override;
        virtual void set_selected_item(const Item& item) override;
        virtual void set_selected_trigger(const std::weak_ptr<ITrigger>& trigger) override;
        virtual void set_triggers(const std::vector<std::weak_ptr<ITrigger>>& triggers) override;
        virtual void update(float delta) override;
        virtual void set_number(int32_t number) override;
    private:
        void set_sync_room(bool value);
        void set_track_item(bool value);
        void set_track_trigger(bool value);
        void render_rooms_list();
        void render_room_details();
        bool render_rooms_window();
        void load_room_details(uint32_t room_number);

        std::vector<std::weak_ptr<IRoom>> _all_rooms;
        std::vector<Item> _all_items;
        std::vector<std::weak_ptr<ITrigger>> _all_triggers;

        bool _sync_room{ true };
        bool _track_item{ false };
        bool _track_trigger{ false };
        
        std::optional<Item> _global_selected_item;
        std::optional<Item> _local_selected_item;
        bool _scroll_to_item{ false };

        std::weak_ptr<ITrigger> _global_selected_trigger;
        std::weak_ptr<ITrigger> _local_selected_trigger;
        bool _scroll_to_trigger{ false };

        uint32_t _current_room{ 0u };
        uint32_t _selected_room{ 0u };

        TokenStore _token_store;
        std::unique_ptr<IMapRenderer> _map_renderer;
        std::shared_ptr<IClipboard> _clipboard;
        trlevel::LevelVersion _level_version;
        std::string _id{ "Rooms 0" };
        Tooltip _map_tooltip;
        bool _scroll_to_room{ false };
        std::optional<float> _tooltip_timer;

        // Map texture saved so that it is guaranteed to survive until ImGui renders.
        graphics::Texture _map_texture;

        Filters<IRoom> _filters;
    };
}
