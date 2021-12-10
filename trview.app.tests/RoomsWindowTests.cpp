#include <trview.app/Windows/RoomsWindow.h>
#include <trview.ui.render/Mocks/IRenderer.h>
#include <trview.ui/Button.h>
#include <trview.ui/Listbox.h>
#include <trview.app/Elements/Types.h>
#include <trview.graphics/mocks/IDeviceWindow.h>
#include <trview.ui.render/Mocks/IMapRenderer.h>
#include <trview.app/Mocks/Elements/IRoom.h>
#include <trview.ui/Mocks/Input/IInput.h>
#include <trview.input/Mocks/IMouse.h>
#include <trview.app/Mocks/Elements/ITrigger.h>
#include <trview.common/Mocks/Windows/IClipboard.h>
#include <trview.app/Mocks/UI/IBubble.h>
#include <trview.ui/JsonLoader.h>
#include <trview.common/Mocks/Windows/IShell.h>

using namespace trview;
using namespace trview::tests;
using namespace trview::graphics;
using namespace trview::graphics::mocks;
using namespace trview::ui;
using namespace trview::ui::mocks;
using namespace trview::ui::render;
using namespace trview::ui::render::mocks;
using namespace trview::mocks;
using namespace trview::input::mocks;

namespace
{
    auto register_test_module()
    {
        struct test_module
        {
            IDeviceWindow::Source device_window_source{ [](auto&&...) { return std::make_unique<MockDeviceWindow>(); } };
            IRenderer::Source renderer_source{ [](auto&&...) { return std::make_unique<MockRenderer>(); } };
            IMapRenderer::Source map_renderer_source{ [](auto&&...) { return std::make_unique<MockMapRenderer>(); } };
            IInput::Source input_source{ [&](auto&&...)
                    {
                        auto input = std::make_unique<MockInput>();
                        EXPECT_CALL(*input, mouse).WillRepeatedly(testing::Return(std::make_shared<MockMouse>()));
                        return std::move(input);
                    } };
            trview::Window window{ create_test_window(L"RoomsWindowTests") };
            std::shared_ptr<IClipboard> clipboard{ std::make_shared<MockClipboard>() };
            IBubble::Source bubble_source{ [](auto&&...) { return std::make_unique<MockBubble>(); } };

            test_module& with_bubble_source(const IBubble::Source& source)
            {
                this->bubble_source = source;
                return *this;
            }

            std::unique_ptr<RoomsWindow> build()
            {
                return std::make_unique<RoomsWindow>(device_window_source, renderer_source, map_renderer_source, input_source, clipboard, bubble_source, window, std::make_shared<JsonLoader>(std::make_shared<MockShell>()));
            }
        };
        return test_module{};
    }
}

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


TEST(RoomsWindow, ClickStatShowsBubble)
{
    auto bubble = std::make_unique<MockBubble>();
    EXPECT_CALL(*bubble, show(testing::A<const Point&>())).Times(1);

    auto window = register_test_module().with_bubble_source([&](auto&&...) { return std::move(bubble); }).build();

    auto room = std::make_shared<MockRoom>();
    window->set_rooms({ room });
    window->set_current_room(0);

    auto stats = window->root_control()->find<Listbox>(RoomsWindow::Names::stats_listbox);
    ASSERT_NE(stats, nullptr);

    auto first_stat = stats->find<ui::Control>(Listbox::Names::row_name_format + "0");
    ASSERT_NE(first_stat, nullptr);

    auto value = first_stat->find<ui::Button>(ui::Listbox::Row::Names::cell_name_format + "Value");
    ASSERT_NE(value, nullptr);
    value->clicked(Point());
}

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

