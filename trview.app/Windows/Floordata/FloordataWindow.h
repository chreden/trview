#pragma once

#include "IFloordataWindow.h"

namespace trview
{
    struct ISector;

    class FloordataWindow final : public IFloordataWindow
    {
    public:
        struct Names
        {
            static const inline std::string simple_mode = "Simple";
            static const inline std::string track_room = "Track Room";
        };

        virtual ~FloordataWindow() = default;
        virtual void render() override;
        virtual void set_current_room(uint32_t room) override;
        virtual void set_floordata(const std::vector<uint16_t>& data) override;
        virtual void set_items(const std::vector<Item>& items) override;
        virtual void set_number(int32_t number) override;
        virtual void set_rooms(const std::vector<std::weak_ptr<IRoom>>& rooms) override;
        virtual void update(float delta) override;
    private:
        bool render_floordata_window();
        void render_simple();
        void render_advanced();

        std::string _id{ "Floordata 0" };
        std::vector<uint16_t> _floordata;
        std::vector<std::weak_ptr<IRoom>> _rooms;
        std::vector<Item> _items;
        std::unordered_map<uint32_t, std::vector<std::weak_ptr<ISector>>> _floordata_map;
        std::unordered_map<uint32_t, std::weak_ptr<ISector>> _simple_map;
        uint32_t _selected_floordata{ 0u };
        bool _simple_mode{ false };
        bool _track_room{ false };
        uint32_t _current_room{ 0u };
    };
}
