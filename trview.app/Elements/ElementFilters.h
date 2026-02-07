#pragma once

#include <memory>

namespace trview
{
    class Filters;
    struct ILevel;

    void add_camera_sink_filters(Filters& filters);
    void add_flyby_filters(Filters& filters);
    void add_flyby_node_filters(Filters& filters, const std::weak_ptr<ILevel>& level);
    void add_item_filters(Filters& filters, const std::weak_ptr<ILevel>& level);
    void add_light_filters(Filters& filters, const std::weak_ptr<ILevel>& level);
    void add_room_filters(Filters& filters, const std::weak_ptr<ILevel>& level);
    void add_sector_filters(Filters& filters, const std::weak_ptr<ILevel>& level);
    void add_sounds_filters(Filters& filters, const std::weak_ptr<ILevel>& level);
    void add_static_mesh_filters(Filters& filters, const std::weak_ptr<ILevel>& level);
    void add_trigger_filters(Filters& filters, const std::weak_ptr<ILevel>& level);
    void add_all_filters(Filters& filters, const std::weak_ptr<ILevel>& level);
}
