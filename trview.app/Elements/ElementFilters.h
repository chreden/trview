#pragma once

namespace trview
{
    class Filters2;

    void add_camera_sink_filters(Filters& filters, const std::weak_ptr<ILevel>& level);
    void add_item_filters(Filters& filters, const std::set<std::string>& available_types, const std::set<std::string>& available_categories);
    void add_light_filters(Filters& filters, const std::weak_ptr<ILevel>& level);
    void add_room_filters(Filters& filters, const std::weak_ptr<ILevel>& level);
    void add_sounds_filters(Filters& filters, const std::weak_ptr<ILevel>& level);
    void add_static_mesh_filters(Filters& filters, const std::weak_ptr<ILevel>& level);
    void add_trigger_filters(Filters& filters, const std::weak_ptr<ILevel>& level);
    
}
