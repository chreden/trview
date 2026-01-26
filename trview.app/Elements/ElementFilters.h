#pragma once

namespace trview
{
    class Filters2;

    void add_item_filters(Filters2& filters, const std::set<std::string>& available_types, const std::set<std::string>& available_categories);
    void add_room_filters(Filters2& filters, const std::weak_ptr<ILevel>& level);
    void add_trigger_filters(Filters2& filters, const std::weak_ptr<ILevel>& level);
}
