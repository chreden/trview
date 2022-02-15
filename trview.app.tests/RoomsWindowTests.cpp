#include <trview.app/Windows/RoomsWindow.h>
#include <trview.app/Elements/Types.h>
#include <trview.app/Mocks/UI/IMapRenderer.h>
#include <trview.app/Mocks/Elements/IRoom.h>
#include <trview.app/Mocks/Elements/ITrigger.h>
#include <trview.common/Mocks/Windows/IClipboard.h>
#include "TestImgui.h"

using namespace trview;
using namespace trview::tests;
using namespace trview::mocks;

namespace
{
    auto register_test_module()
    {
        struct test_module
        {
            IMapRenderer::Source map_renderer_source{ [](auto&&...) { return std::make_unique<MockMapRenderer>(); } };
            std::shared_ptr<IClipboard> clipboard{ std::make_shared<MockClipboard>() };

            std::unique_ptr<RoomsWindow> build()
            {
                return std::make_unique<RoomsWindow>(map_renderer_source, clipboard);
            }

            test_module& with_clipboard(const std::shared_ptr<IClipboard>& clipboard)
            {
                this->clipboard = clipboard;
                return *this;
            }
        };
        return test_module{};
    }
}
/*
TEST(RoomsWindow, ClearSelectedTriggerClearsSelection)
{
    auto window = register_test_module().build();
    auto room = std::make_shared<MockRoom>();
    auto trigger = std::make_shared<MockTrigger>();

    window->set_rooms({ room });
    window->set_triggers({ trigger });
    
    auto list = window->root_control()->find<ui::Listbox>(RoomsWindow::Names::rooms_listbox);
    ASSERT_NE(list, nullptr);

    auto row = list->find<ui::Control>(ui::Listbox::Names::row_name_format + "0");
    ASSERT_NE(row, nullptr);

    auto cell = row->find<ui::Button>(ui::Listbox::Row::Names::cell_name_format + "#");
    ASSERT_NE(cell, nullptr);
    cell->clicked(Point());

    auto triggers_list = window->root_control()->find<ui::Listbox>(RoomsWindow::Names::triggers_listbox);
    ASSERT_NE(triggers_list, nullptr);

    auto triggers_row = triggers_list->find<ui::Control>(ui::Listbox::Names::row_name_format + "0");
    ASSERT_NE(triggers_row, nullptr);

    auto triggers_cell = triggers_row->find<ui::Button>(ui::Listbox::Row::Names::cell_name_format + "#");
    ASSERT_NE(triggers_cell, nullptr);

    ASSERT_FALSE(triggers_list->selected_item().has_value());

    triggers_cell->clicked(Point());
    ASSERT_TRUE(triggers_list->selected_item().has_value());

    window->clear_selected_trigger();
    ASSERT_FALSE(triggers_list->selected_item().has_value());
}

TEST(RoomsWindow, SetTriggersClearsSelection)
{
    auto window = register_test_module().build();
    auto room = std::make_shared<MockRoom>();
    auto trigger = std::make_shared<MockTrigger>();

    window->set_rooms({ room });
    window->set_triggers({ trigger });

    auto list = window->root_control()->find<ui::Listbox>(RoomsWindow::Names::rooms_listbox);
    ASSERT_NE(list, nullptr);

    auto row = list->find<ui::Control>(ui::Listbox::Names::row_name_format + "0");
    ASSERT_NE(row, nullptr);

    auto cell = row->find<ui::Button>(ui::Listbox::Row::Names::cell_name_format + "#");
    ASSERT_NE(cell, nullptr);
    cell->clicked(Point());

    auto triggers_list = window->root_control()->find<ui::Listbox>(RoomsWindow::Names::triggers_listbox);
    ASSERT_NE(triggers_list, nullptr);

    auto triggers_row = triggers_list->find<ui::Control>(ui::Listbox::Names::row_name_format + "0");
    ASSERT_NE(triggers_row, nullptr);

    auto triggers_cell = triggers_row->find<ui::Button>(ui::Listbox::Row::Names::cell_name_format + "#");
    ASSERT_NE(triggers_cell, nullptr);

    ASSERT_FALSE(triggers_list->selected_item().has_value());

    triggers_cell->clicked(Point());
    ASSERT_TRUE(triggers_list->selected_item().has_value());

    window->set_triggers({});
    ASSERT_FALSE(triggers_list->selected_item().has_value());
}
*/

TEST(RoomsWindow, ClickStatShowsBubbleAndCopies)
{
    auto clipboard = std::make_shared<MockClipboard>();
    EXPECT_CALL(*clipboard, write).Times(1);
    auto window = register_test_module().with_clipboard(clipboard).build();

    auto room = std::make_shared<MockRoom>();
    window->set_rooms({ room });
    window->set_current_room(0);

    TestImgui imgui([&]() { window->render(); });

    ASSERT_EQ(imgui.find_window("##Tooltip_00"), nullptr);

    const auto id = imgui.id("Rooms 0")
        .push_child(RoomsWindow::Names::details_panel)
        .push_override(RoomsWindow::Names::bottom)
        .push(RoomsWindow::Names::properties)
        .id("X");
    imgui.click_element(id, false, true);
}

/*
TEST(RoomsWindow, LevelVersionChangesFlags)
{
    auto room = std::make_shared<MockRoom>();
    EXPECT_CALL(*room, flag).Times(testing::AtLeast(1)).WillRepeatedly(testing::Return(true));

    auto window = register_test_module().build();
    window->set_level_version(trlevel::LevelVersion::Tomb1);
    window->set_rooms({ room });
    window->set_current_room(0);

    auto stats_box = window->root_control()->find<Listbox>(RoomsWindow::Names::stats_listbox);
    const auto get_item_names = [](const auto& items)
    {
        std::vector<std::wstring> names;
        for (const auto& item : items)
        {
            names.push_back(item.value(L"Name"));
        }
        return names;
    };
    auto names = get_item_names(stats_box->items());

    ASSERT_THAT(names, testing::Contains(L"Bit 7"));
    ASSERT_THAT(names, testing::Not(testing::Contains(L"Quicksand / 7")));

    window->set_level_version(trlevel::LevelVersion::Tomb3);
    window->set_rooms({ room });
    window->set_current_room(0);
    names = get_item_names(stats_box->items());

    ASSERT_THAT(names, testing::Not(testing::Contains(L"Bit 7")));
    ASSERT_THAT(names, testing::Contains(L"Quicksand / 7"));
}
*/