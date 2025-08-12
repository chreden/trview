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

    std::shared_ptr<ISoundSource> raised;
    auto t2 = manager->on_sound_source_selected += capture(raised);

    manager->create_window();

    auto sound_source = mock_shared<MockSoundSource>();
    window->on_sound_source_selected(sound_source);

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

TEST(SoundsWindowManager, SetLevelVersion)
{
    auto window = mock_shared<MockSoundsWindow>();
    EXPECT_CALL(*window, set_level_version(trlevel::LevelVersion::Unknown)).Times(1);
    EXPECT_CALL(*window, set_level_version(trlevel::LevelVersion::Tomb3)).Times(1);

    auto manager = register_test_module().with_window_source([&]() { return window; }).build();
    manager->create_window();
    manager->set_level_version(trlevel::LevelVersion::Tomb3);
}

TEST(SoundsWindowManager, SetSoundSources)
{
    auto window = mock_shared<MockSoundsWindow>();
    EXPECT_CALL(*window, set_sound_sources).Times(2);

    auto manager = register_test_module().with_window_source([&]() { return window; }).build();
    manager->create_window();
    manager->set_sound_sources({});
}

TEST(SoundsWindowManager, SetSoundStorage)
{
    auto window = mock_shared<MockSoundsWindow>();
    EXPECT_CALL(*window, set_sound_storage).Times(2);

    auto manager = register_test_module().with_window_source([&]() { return window; }).build();
    manager->create_window();
    manager->set_sound_storage({});
}

TEST(SoundsWindowManager, WindowCreated)
{
    auto window = mock_shared<MockSoundsWindow>();
    EXPECT_CALL(*window, set_number(1)).Times(1);
    EXPECT_CALL(*window, set_level_version).Times(1);
    EXPECT_CALL(*window, set_selected_sound_source).Times(1);
    EXPECT_CALL(*window, set_sound_sources).Times(1);
    EXPECT_CALL(*window, set_sound_storage).Times(1);

    auto manager = register_test_module()
        .with_window_source([&]() { return window; })
        .build();

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
