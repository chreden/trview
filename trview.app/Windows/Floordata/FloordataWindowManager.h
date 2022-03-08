#pragma once

#include <trview.common/MessageHandler.h>
#include "IFloordataWindowManager.h"
#include "../WindowManager.h"
#include "../../Elements/IRoom.h"

namespace trview
{
    class FloordataWindowManager final : public IFloordataWindowManager, public WindowManager<IFloordataWindow>, public MessageHandler
    {
    public:
        explicit FloordataWindowManager(const Window& window, const std::shared_ptr<IShortcuts>& shortcuts, const IFloordataWindow::Source& floordata_window_source);
        virtual ~FloordataWindowManager() = default;
        virtual std::weak_ptr<IFloordataWindow> create_window() override;
        virtual std::optional<int> process_message(UINT message, WPARAM wParam, LPARAM lParam) override;
        virtual void render() override;
        virtual void set_room(uint32_t room) override;
        virtual void set_floordata(const std::vector<uint16_t>& data) override;
        virtual void set_items(const std::vector<Item>& items) override;
        virtual void set_rooms(const std::vector<std::weak_ptr<IRoom>>& rooms) override;
        virtual void update(float delta) override;
    private:
        IFloordataWindow::Source _floordata_window_source;

        std::vector<uint16_t> _floordata;
        std::vector<std::weak_ptr<IRoom>> _rooms;
        std::vector<Item> _items;
        uint32_t _current_room{ 0u };
    };
}
