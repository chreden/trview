#include <trview.app/Windows/RoomsWindowManager.h>
#include <trview.app/Elements/Types.h>
#include <trview.app/Mocks/Windows/IRoomsWindow.h>
#include <trview.common/Mocks/Windows/IShortcuts.h>
#include <trview.app/Mocks/Geometry/IMesh.h>
#include <trview.app/Mocks/Elements/ITrigger.h>
#include <trview.app/Mocks/Elements/ISector.h>
#include <trview.app/Mocks/Elements/IRoom.h>
#include <trview.app/Mocks/Elements/IStaticMesh.h>

using namespace trview;
using namespace trview::mocks;
using namespace trview::tests;

namespace
{
    Event<> shortcut_handler;

    auto register_test_module()
    {
        struct test_module
        {
            Window window{ create_test_window(L"RoomsWindowManagerTests") };
            std::shared_ptr<MockShortcuts> shortcuts{ mock_shared<MockShortcuts>() };
            RoomsWindow::Source window_source{ [](auto&&...) { return mock_shared<MockRoomsWindow>(); } };

            test_module& with_window_source(const RoomsWindow::Source& source)
            {
                this->window_source = source;
                return *this;
            }

            test_module& with_shortcuts(const std::shared_ptr<MockShortcuts>& shortcuts)
            {
                this->shortcuts = shortcuts;
                return *this;
            }

            test_module()
            {
                EXPECT_CALL(*shortcuts, add_shortcut).WillRepeatedly([&](auto, auto) -> Event<>&{ return shortcut_handler; });
            }

            std::unique_ptr<RoomsWindowManager> build()
            {
                return std::make_unique<RoomsWindowManager>(window, shortcuts, window_source);
            }
        };

        return test_module{};
    }
}

TEST(RoomsWindowManager, WindowsUpdated)
{
    auto mock_window = mock_shared<MockRoomsWindow>();
    EXPECT_CALL(*mock_window, update(1.0f)).Times(1);
    auto manager = register_test_module().with_window_source([&](auto&&...) { return mock_window; }).build();
    manager->create_window();
    manager->update(1.0f);
}

TEST(RoomsWindowManager, LevelVersionPassedToNewWindows)
{
    auto mock_window = mock_shared<MockRoomsWindow>();
    EXPECT_CALL(*mock_window, set_level_version(trlevel::LevelVersion::Tomb3)).Times(1);
    auto manager = register_test_module().with_window_source([&](auto&&...) { return mock_window; }).build();
    manager->set_level_version(trlevel::LevelVersion::Tomb3);
    manager->create_window();
}

TEST(RoomsWindowManager, LevelVersionPassedToWindows)
{
    auto mock_window = mock_shared<MockRoomsWindow>();
    EXPECT_CALL(*mock_window, set_level_version(trlevel::LevelVersion::Unknown)).Times(1);
    EXPECT_CALL(*mock_window, set_level_version(trlevel::LevelVersion::Tomb3)).Times(1);
    auto manager = register_test_module().with_window_source([&](auto&&...) { return mock_window; }).build();
    manager->create_window();
    manager->set_level_version(trlevel::LevelVersion::Tomb3);
}

TEST(RoomsWindowManager, SetMapColoursPassedToWindows)
{
    auto mock_window = mock_shared<MockRoomsWindow>();
    EXPECT_CALL(*mock_window, set_map_colours).Times(2);
    auto manager = register_test_module().with_window_source([&](auto&&...) { return mock_window; }).build();
    manager->create_window();
    manager->set_map_colours({});
}

TEST(RoomsWindowManager, MapColoursPassedToNewWindows)
{
    auto mock_window = mock_shared<MockRoomsWindow>();
    EXPECT_CALL(*mock_window, set_map_colours).Times(1);
    auto manager = register_test_module().with_window_source([&](auto&&...) { return mock_window; }).build();
    manager->create_window();
}

TEST(RoomsWindowManager, SceneChangedRaised)
{
    auto manager = register_test_module().build();
    auto room = mock_shared<MockRoom>();

    bool raised = false;
    auto token = manager->on_scene_changed += [&]() { raised = true; };

    auto window = manager->create_window().lock();
    ASSERT_NE(window, nullptr);

    window->on_scene_changed();
    ASSERT_TRUE(raised);
}

TEST(RoomsWindowManager, SetFloordataPassedToWindows)
{
    auto mock_window = mock_shared<MockRoomsWindow>();
    EXPECT_CALL(*mock_window, set_floordata).Times(2);
    auto manager = register_test_module().with_window_source([&](auto&&...) { return mock_window; }).build();
    manager->create_window();
    manager->set_floordata({});
}

TEST(RoomsWindowManager, FloordataPassedToNewWindows)
{
    auto mock_window = mock_shared<MockRoomsWindow>();
    EXPECT_CALL(*mock_window, set_floordata).Times(1);
    auto manager = register_test_module().with_window_source([&](auto&&...) { return mock_window; }).build();
    manager->create_window();
}

TEST(RoomsWindowManager, OnSectorHoverRaised)
{
    auto manager = register_test_module().build();
    auto sector = mock_shared<MockSector>();

    std::weak_ptr<ISector> raised;
    auto token = manager->on_sector_hover += [&](auto&& value)
    {
        raised = value;
    };

    auto window = manager->create_window().lock();
    ASSERT_NE(window, nullptr);

    window->on_sector_hover(sector);
    ASSERT_EQ(raised.lock(), sector);
}

TEST(RoomsWindowManager, OnStaticMeshSelectedForwarded)
{
    auto manager = register_test_module().build();
    auto static_mesh = mock_shared<MockStaticMesh>();

    std::shared_ptr<IStaticMesh> raised;
    auto token = manager->on_static_mesh_selected += [&](auto&& value)
    {
        raised = value.lock();
    };

    auto window = manager->create_window().lock();
    ASSERT_NE(window, nullptr);

    window->on_static_mesh_selected(static_mesh);
    ASSERT_EQ(raised, static_mesh);
}

TEST(RoomsWindowManager, SetNgPlusPassedToWindows)
{
    auto mock_window = mock_shared<MockRoomsWindow>();
    EXPECT_CALL(*mock_window, set_ng_plus).Times(2);
    auto manager = register_test_module().with_window_source([&](auto&&...) { return mock_window; }).build();
    manager->create_window();
    manager->set_ng_plus(false);
}
