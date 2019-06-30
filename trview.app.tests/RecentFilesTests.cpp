#include "CppUnitTest.h"

#include <trview.app/Menus/RecentFiles.h>
#include <trview.tests.common/Window.h>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace trview
{
    namespace tests
    {
        TEST_CLASS(RecentFilesTests)
        {
            /// Tests that opening the first file triggers the event.
            TEST_METHOD(OpenFile)
            {
                RecentFiles recent_files(create_test_window(L"TRViewRecentFilesTests"));

                const std::list<std::string> files{ "test.tr2", "test2.tr2" };
                recent_files.set_recent_files(files);

                uint32_t times_called{ 0 };
                std::string raised_file;
                auto token = recent_files.on_file_open += [&](const auto& file)
                {
                    ++times_called;
                    raised_file = file;
                };

                recent_files.process_message(WM_COMMAND, MAKEWPARAM(5000, 0), 0);
                Assert::AreEqual(1u, times_called);
                Assert::AreEqual(std::string("test2.tr2"), files.back());
            }
        };
    }
}
