#include "ElementFilters.h"

namespace trview
{
    void add_all_filters(Filters& filters, const std::weak_ptr<ILevel>& level)
    {
        add_camera_sink_filters(filters);
        add_flyby_filters(filters);
        add_flyby_node_filters(filters, level);
        add_item_filters(filters, level);
        add_light_filters(filters, level);
        add_room_filters(filters, level);
        add_sector_filters(filters, level);
        add_sounds_filters(filters, level);
        add_static_mesh_filters(filters, level);
        add_trigger_filters(filters, level);
    }
}