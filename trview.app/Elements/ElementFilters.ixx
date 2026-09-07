export module trview.app:ElementFilters;

import std;

import :Filters;
import :ILevel;

namespace trview
{
    export void add_camera_sink_filters(Filters& filters);
    export void add_flyby_filters(Filters& filters);
    export void add_flyby_node_filters(Filters& filters, const std::weak_ptr<ILevel>& level);
    export void add_item_filters(Filters& filters, const std::weak_ptr<ILevel>& level);
    export void add_light_filters(Filters& filters, const std::weak_ptr<ILevel>& level);
    export void add_room_filters(Filters& filters, const std::weak_ptr<ILevel>& level);
    export void add_sector_filters(Filters& filters, const std::weak_ptr<ILevel>& level);
    export void add_sounds_filters(Filters& filters, const std::weak_ptr<ILevel>& level);
    export void add_static_mesh_filters(Filters& filters, const std::weak_ptr<ILevel>& level);
    export void add_trigger_filters(Filters& filters, const std::weak_ptr<ILevel>& level);
    export void add_all_filters(Filters& filters, const std::weak_ptr<ILevel>& level);
}
