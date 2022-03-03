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
            IMapRenderer::Source map_renderer_source{ [](auto&&...) { return mock_unique<MockMapRenderer>(); } };
            std::shared_ptr<IClipboard> clipboard{ mock_shared<MockClipboard>() };

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

TEST(RoomsWindow, ClickStatShowsBubbleAndCopies)
{
    auto clipboard = mock_shared<MockClipboard>();
    EXPECT_CALL(*clipboard, write).Times(1);
    auto window = register_test_module().with_clipboard(clipboard).build();

    auto room = mock_shared<MockRoom>();
    window->set_rooms({ room });
    window->set_current_room(0);

    TestImgui imgui([&]() { window->render(); });

    ASSERT_EQ(imgui.find_window("##Tooltip_00"), nullptr);

    const auto id = imgui.id("Rooms 0")
        .push_child(RoomsWindow::Names::details_panel)
        .push_override(RoomsWindow::Names::bottom)
        .push(RoomsWindow::Names::properties)
        .id("X");
    imgui.click_element_with_hover(id,
        imgui.id("Rooms 0").push_child(RoomsWindow::Names::details_panel).push_override(RoomsWindow::Names::bottom).push_child(RoomsWindow::Names::properties));

    ASSERT_NE(imgui.find_window("##Tooltip_00"), nullptr);
}


TEST(RoomsWindow, LevelVersionChangesFlags)
{
    auto room = mock_shared<MockRoom>();
    EXPECT_CALL(*room, flag).Times(testing::AtLeast(1)).WillRepeatedly(testing::Return(true));

    auto window = register_test_module().build();
    window->set_level_version(trlevel::LevelVersion::Tomb1);
    window->set_rooms({ room });
    window->set_current_room(0);

    TestImgui imgui([&]() { window->render(); });

    ASSERT_TRUE(imgui.element_present(
            imgui.id("Rooms 0")
            .push_child(RoomsWindow::Names::details_panel)
            .push_override(RoomsWindow::Names::bottom)
            .push(RoomsWindow::Names::properties)
            .id("Bit 7")));
    ASSERT_FALSE(imgui.element_present(
        imgui.id("Rooms 0")
        .push_child(RoomsWindow::Names::details_panel)
        .push_override(RoomsWindow::Names::bottom)
        .push(RoomsWindow::Names::properties)
        .id("Quicksand / 7")));

    window->set_level_version(trlevel::LevelVersion::Tomb3);
    window->set_rooms({ room });
    window->set_current_room(0);

    imgui.reset();
    imgui.render();

    ASSERT_FALSE(imgui.element_present(
        imgui.id("Rooms 0")
        .push_child(RoomsWindow::Names::details_panel)
        .push_override(RoomsWindow::Names::bottom)
        .push(RoomsWindow::Names::properties)
        .id("Bit 7")));
    ASSERT_TRUE(imgui.element_present(
        imgui.id("Rooms 0")
        .push_child(RoomsWindow::Names::details_panel)
        .push_override(RoomsWindow::Names::bottom)
        .push(RoomsWindow::Names::properties)
        .id("Quicksand / 7")));
}
