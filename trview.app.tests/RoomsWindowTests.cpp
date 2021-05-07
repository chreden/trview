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
#include <external/boost/di.hpp>
#include <trview.app/Mocks/Elements/ITrigger.h>

using namespace trview;
using namespace trview::tests;
using namespace trview::graphics;
using namespace trview::graphics::mocks;
using namespace trview::ui::mocks;
using namespace trview::ui::render::mocks;
using namespace trview::mocks;
using namespace trview::input::mocks;
using testing::ReturnRef;

namespace
{
    MockMouse mouse;

    auto register_test_module()
    {
        using namespace boost;
        return di::make_injector(
            di::bind<IDeviceWindow::Source>.to([](auto&&) { return [](auto&&...) { return std::make_unique<MockDeviceWindow>(); }; }),
            di::bind<ui::render::IRenderer::Source>.to([](auto&&) { return [](auto&&...) { return std::make_unique<MockRenderer>(); }; }),
            di::bind<ui::render::IMapRenderer::Source>.to([](auto&&) { return [](auto&&...) { return std::make_unique<MockMapRenderer>(); }; }),
            di::bind<ui::IInput::Source>.to([&](auto&&) { return [&](auto&&...) 
                { 
                    auto input = std::make_unique<MockInput>();
                    EXPECT_CALL(*input, mouse).WillRepeatedly(ReturnRef(mouse));
                    return std::move(input);
                };
            }),
            di::bind<Window>.to(create_test_window(L"ItemsWindowTests")),
            di::bind<RoomsWindow>()
        );
    }
}

TEST(RoomsWindow, ClearSelectedTriggerClearsSelection)
{
    auto window = register_test_module().create<std::unique_ptr<RoomsWindow>>();
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
    auto window = register_test_module().create<std::unique_ptr<RoomsWindow>>();
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