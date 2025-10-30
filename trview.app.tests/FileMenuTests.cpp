#include <trview.app/Menus/FileMenu.h>
#include <trview.common/Mocks/Windows/IShortcuts.h>
#include <trview.common/Mocks/Windows/IDialogs.h>
#include <trview.common/Mocks/IFiles.h>
#include <trview.app/Resources/resource.h>
#include <trview.common/Mocks/Messages/IMessageSystem.h>

using namespace trlevel::mocks;
using namespace trview;
using namespace trview::tests;
using namespace trview::mocks;
using testing::Return;

namespace
{
    Event<> f5_shortcut;
    Event<> ctrl_o_shortcut;
    Event<> f6_shortcut;
    Event<> f7_shortcut;

    auto register_test_module()
    {
        struct test_module
        {
            trview::Window window{ create_test_window(L"FileMenuTests") };
            std::shared_ptr<MockShortcuts> shortcuts{ mock_shared<MockShortcuts>() };
            std::shared_ptr<MockDialogs> dialogs{ mock_shared<MockDialogs>() };
            std::shared_ptr<IFiles> files{ mock_shared<MockFiles>() };
            FileMenu::LevelNameSource level_name_source{ [](auto&&...) { return std::nullopt; } };
            std::shared_ptr<IMessageSystem> messaging{ mock_shared<MockMessageSystem>() };

            std::shared_ptr<FileMenu> build()
            {
                ON_CALL(*shortcuts, add_shortcut(false, VK_F5)).WillByDefault([&](auto, auto) -> Event<>&{ return f5_shortcut; });
                ON_CALL(*shortcuts, add_shortcut(true, 'O')).WillByDefault([&](auto, auto) -> Event<>&{ return ctrl_o_shortcut; });
                ON_CALL(*shortcuts, add_shortcut(false, VK_F6)).WillByDefault([&](auto, auto) -> Event<>&{ return f6_shortcut; });
                ON_CALL(*shortcuts, add_shortcut(false, VK_F7)).WillByDefault([&](auto, auto) -> Event<>&{ return f7_shortcut; });
                return std::make_shared<FileMenu>(window, shortcuts, dialogs, files, level_name_source, messaging);
            }

            test_module& with_window(const trview::Window& window)
            {
                this->window = window;
                return *this;
            }

            test_module& with_dialogs(const std::shared_ptr<MockDialogs>& dialogs)
            {
                this->dialogs = dialogs;
                return *this;
            }

            test_module& with_shortcuts(const std::shared_ptr<MockShortcuts>& shortcuts)
            {
                this->shortcuts = shortcuts;
                return *this;
            }

            test_module& with_files(const std::shared_ptr<MockFiles>& files)
            {
                this->files = files;
                return *this;
            }
        };

        return test_module {};
    }
}

/// Tests that opening the first file triggers the event.
TEST(FileMenu, OpenFileRaisedOnRecentFile)
{
    auto menu = register_test_module().build();

    const std::list<std::string> files{ "test.tr2", "test2.tr2" };
    menu->set_recent_files(files);

    uint32_t times_called{ 0 };
    std::string raised_file;
    auto token = menu->on_file_open += [&](const auto& file)
    {
        ++times_called;
        raised_file = file;
    };

    menu->process_message(WM_COMMAND, MAKEWPARAM(5000, 0), 0);
    ASSERT_EQ(1u, times_called);
    ASSERT_EQ(std::string("test2.tr2"), files.back());
}

TEST(FileMenu, FileOpenUsingShortcut)
{
    auto dialogs = mock_shared<MockDialogs>();
    EXPECT_CALL(*dialogs, open_file).Times(1).WillRepeatedly(Return(IDialogs::FileResult{ .filename = "filename" }));
    auto menu = register_test_module().with_dialogs(dialogs).build();
    std::optional<std::string> raised;
    auto token = menu->on_file_open += [&](auto&& filename) { raised = filename; };
    ctrl_o_shortcut();
    ASSERT_TRUE(raised.has_value());
    ASSERT_EQ(raised.value(), "filename");
}

TEST(FileMenu, FileOpenOpensFile)
{
    auto dialogs = mock_shared<MockDialogs>();
    EXPECT_CALL(*dialogs, open_file).Times(1).WillRepeatedly(Return(IDialogs::FileResult{ .filename = "filename" }));
    auto menu = register_test_module().with_dialogs(dialogs).build();
    std::optional<std::string> raised;
    auto token = menu->on_file_open += [&](auto&& filename) { raised = filename; };
    menu->process_message(WM_COMMAND, MAKEWPARAM(ID_FILE_OPEN, 0), 0);
    ASSERT_TRUE(raised.has_value());
    ASSERT_EQ(raised.value(), "filename");
}

TEST(FileMenu, FileOpenDoesNotOpenFileWhenCancelled)
{
    auto dialogs = mock_shared<MockDialogs>();
    EXPECT_CALL(*dialogs, open_file).Times(1);
    auto menu = register_test_module().with_dialogs(dialogs).build();
    bool called = false;
    auto token = menu->on_file_open += [&](auto&&...) { called = true; };
    menu->process_message(WM_COMMAND, MAKEWPARAM(ID_FILE_OPEN, 0), 0);
    ASSERT_FALSE(called);
}

TEST(FileMenu, DropFile)
{
    auto menu = register_test_module().build();

    uint32_t times_called = 0;
    std::string file_opened;
    auto token = menu->on_file_open += [&](const auto& file)
    {
        ++times_called;
        file_opened = file;
    };

    std::string filename("test_filename");
    // +2 for the double null terminated list of names.
    HGLOBAL global = GlobalAlloc(GHND, sizeof(DROPFILES) + filename.size() + 2);
    DROPFILES* dropfiles = static_cast<DROPFILES*>(GlobalLock(global));
    dropfiles->pFiles = sizeof(DROPFILES);
    strcpy_s(reinterpret_cast<char*>(dropfiles + 1), filename.size() + 1, filename.c_str());
    GlobalUnlock(global);

    menu->process_message(WM_DROPFILES, (WPARAM)global, 0);

    GlobalFree(global);

    ASSERT_EQ(1u, times_called);
    ASSERT_EQ(std::string("test_filename"), file_opened);
}

// Tests that the class enables drag and drop
TEST(FileMenu, EnableDragDrop)
{
    Window window = create_test_window(L"TRViewFileMenuTests");
    auto menu = register_test_module().with_window(window).build();
    LONG_PTR style = GetWindowLongPtr(window, GWL_EXSTYLE);
    ASSERT_EQ(WS_EX_ACCEPTFILES, style & WS_EX_ACCEPTFILES);
}

TEST(FileMenu, Reload)
{
    auto menu = register_test_module().build();
    bool raised;
    auto token = menu->on_reload += [&]() { raised = true; };
    menu->process_message(WM_COMMAND, MAKEWPARAM(ID_FILE_RELOAD, 0), 0);
    ASSERT_TRUE(raised);
}

TEST(FileMenu, ReloadUsingShortcut)
{
    auto menu = register_test_module().build();
    bool raised;
    auto token = menu->on_reload += [&]() { raised = true; };
    f5_shortcut();
    ASSERT_TRUE(raised);
}

TEST(FileMenu, PreviousFile)
{
    auto files = mock_shared<MockFiles>();
    std::vector<IFiles::File> filenames{ { "file1", "file1", 0 }, { "file2", "file2", 0 } };
    EXPECT_CALL(*files, get_files).Times(1).WillOnce(Return(filenames));

    auto menu = register_test_module().with_files(files).build();

    std::optional<std::string> raised;
    auto token = menu->on_file_open += [&](const auto& f)
    {
        raised = f;
    };

    menu->open_file("file2", {});

    f6_shortcut();
    ASSERT_TRUE(raised);
    ASSERT_EQ(raised.value(), "file1");
}

TEST(FileMenu, NextFile)
{
    auto files = mock_shared<MockFiles>();
    std::vector<IFiles::File> filenames{ { "file1", "file1", 0 }, { "file2", "file2", 0 } };
    EXPECT_CALL(*files, get_files).Times(1).WillOnce(Return(filenames));

    auto menu = register_test_module().with_files(files).build();

    std::optional<std::string> raised;
    auto token = menu->on_file_open += [&](const auto& f)
    {
        raised = f;
    };

    menu->open_file("file1", {});

    f7_shortcut();
    ASSERT_TRUE(raised);
    ASSERT_EQ(raised.value(), "file2");
}

TEST(FileMenu, SwitchTo)
{
    auto files = mock_shared<MockFiles>();
    std::vector<IFiles::File> filenames{ { "file1", "file1", 0 }, { "file2", "file2", 0 } };
    EXPECT_CALL(*files, get_files).Times(1).WillOnce(Return(filenames));

    auto menu = register_test_module().with_files(files).build();

    std::optional<std::string> raised;
    auto token = menu->on_file_open += [&](const auto& f)
    {
        raised = f;
    };

    menu->open_file("file1", {});
    menu->switch_to("c:\\dir\\file2");
    ASSERT_TRUE(raised);
    ASSERT_EQ(raised.value(), "file2");
}
