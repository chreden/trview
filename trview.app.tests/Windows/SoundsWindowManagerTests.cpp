#include <trview.app/Windows/Sounds/SoundsWindowManager.h>
#include <trview.app/Mocks/Windows/ISoundsWindow.h>
#include <trview.app/Mocks/Elements/ISoundSource.h>
#include <trview.app/Resources/resource.h>

#include <trview.tests.common/Event.h>

using namespace trview;
using namespace trview::tests;
using namespace trview::mocks;

namespace
{
    auto register_test_module()
    {
        struct test_module
        {
            Window window{ create_test_window(L"SoundsWindowManagerTests") };
            ISoundsWindow::Source window_source{ [](auto&&...) { return mock_shared<MockSoundsWindow>(); } };

            test_module& with_window_source(const ISoundsWindow::Source& source)
            {
                this->window_source = source;
                return *this;
            }

            test_module()
            {
            }

            std::unique_ptr<SoundsWindowManager> build()
            {
                return std::make_unique<SoundsWindowManager>(window, window_source);
            }
        };

        return test_module{};
    }
}

TEST(SoundsWindowManager, EventsForwarded)
{
    auto window = mock_shared<MockSoundsWindow>();
    auto manager = register_test_module().with_window_source([&]() { return window; }).build();

    bool scene_changed = false;
    auto t1 = manager->on_scene_changed += capture_called(scene_changed);
    std::shared_ptr<ISoundSource> raised;
    auto t2 = manager->on_sound_source_selected += capture(raised);

    manager->create_window();

    auto sound_source = mock_shared<MockSoundSource>();
    window->on_scene_changed();
    window->on_sound_source_selected(sound_source);

    ASSERT_EQ(scene_changed, true);
    ASSERT_EQ(raised, sound_source);
}

TEST(SoundsWindowManager, RendersAllWindows)
{
    auto window = mock_shared<MockSoundsWindow>();
    EXPECT_CALL(*window, render).Times(1);

    auto manager = register_test_module().with_window_source([&]() { return window; }).build();
    manager->create_window();
    manager->render();
}

TEST(SoundsWindowManager, WindowCreated)
{
    auto window = mock_shared<MockSoundsWindow>();
    EXPECT_CALL(*window, set_number(1)).Times(1);
    EXPECT_CALL(*window, add_level).Times(1);
    EXPECT_CALL(*window, set_selected_sound_source).Times(1);

    auto level = mock_shared<MockLevel>();

    auto manager = register_test_module()
        .with_window_source([&]() { return window; })
        .build();

    manager->add_level(level);

    auto created = manager->create_window().lock();
    ASSERT_EQ(created, window);
}

TEST(SoundsWindowManager, WindowCreatedOnCommand)
{
    auto window = mock_shared<MockSoundsWindow>();
    bool raised = false;
    auto manager = register_test_module()
        .with_window_source([&]() { raised = true; return window; })
        .build();

    manager->process_message(WM_COMMAND, MAKEWPARAM(ID_WINDOWS_SOUNDS, 0), 0);

    ASSERT_TRUE(raised);
}
