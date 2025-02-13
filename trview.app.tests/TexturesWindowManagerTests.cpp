#include <trview.app/Windows/Textures/TexturesWindowManager.h>
#include <trview.app/Mocks/Windows/ITexturesWindow.h>
#include <trview.app/Resources/resource.h>

using namespace trview;
using namespace trview::mocks;
using namespace trview::tests;

namespace
{
    auto register_test_module()
    {
        struct test_module
        {
            Window window{ create_test_window(L"TexturesWindowManagerTests") };
            ITexturesWindow::Source window_source{ [](auto&&...) { return mock_shared<MockTexturesWindow>(); } };

            test_module& with_window_source(const ITexturesWindow::Source& source)
            {
                this->window_source = source;
                return *this;
            }

            std::unique_ptr<TexturesWindowManager> build()
            {
                return std::make_unique<TexturesWindowManager>(window, window_source);
            }
        };

        return test_module{};
    }
}

TEST(TexturesWindowManager, CreateWindowPassesLevelTextureStorage)
{
    auto level = mock_shared<MockLevel>();
    auto mock_window = mock_shared<MockTexturesWindow>();
    EXPECT_CALL(*mock_window, add_level).Times(1);
    auto manager = register_test_module().with_window_source([&](auto&&...) { return mock_window; }).build();
    manager->add_level(level);
    manager->create_window();
}

TEST(TexturesWindowManager, SetTextureStoragePassesToWindows)
{
    auto level = mock_shared<MockLevel>();
    auto mock_window = mock_shared<MockTexturesWindow>();
    auto manager = register_test_module().with_window_source([&](auto&&...) { return mock_window; }).build();
    manager->create_window();

    EXPECT_CALL(*mock_window, add_level).Times(1);
    manager->add_level(level);
}

TEST(TexturesWindowManager, WindowsRendered)
{
    auto mock_window = mock_shared<MockTexturesWindow>();
    EXPECT_CALL(*mock_window, render).Times(1);

    auto manager = register_test_module().with_window_source([&](auto&&...) { return mock_window; }).build();
    manager->create_window();
    manager->render();
}

TEST(TexturesWindowManager, WindowCreatedOnMenuItem)
{
    auto mock_window = mock_shared<MockTexturesWindow>();
    EXPECT_CALL(*mock_window, set_number).Times(1);

    auto manager = register_test_module().with_window_source([&](auto&&...) { return mock_window; }).build();
    manager->process_message(WM_COMMAND, MAKEWPARAM(ID_WINDOWS_TEXTURES, 0), 0);
}