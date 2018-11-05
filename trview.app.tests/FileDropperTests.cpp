#include "CppUnitTest.h"

#include <trview.app/FileDropper.h>
#include <trview.tests.common/Window.h>
#include <shellapi.h>
#include <ShlObj.h>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace Microsoft
{
    namespace VisualStudio
    {
        namespace CppUnitTestFramework
        {
            std::wstring ToString(LONG_PTR ptr)
            {
                return std::to_wstring(ptr);
            }
        }
    }
}

namespace trview
{
    namespace tests
    {
        TEST_CLASS(FileDropperTests)
        {
            // Tests that sending a dropfile message to the dropper raises the event.
            TEST_METHOD(DropFile)
            {
                HWND window = create_test_window(L"TRViewFileDropperTests");
                FileDropper dropper(window);

                uint32_t times_called = 0;
                std::string file_opened;
                auto token = dropper.on_file_dropped += [&](const auto& file)
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

                dropper.process_message(window, WM_DROPFILES, (WPARAM)global, 0);

                GlobalFree(global);

                Assert::AreEqual(1u, times_called);
                Assert::AreEqual(std::string("test_filename"), file_opened);
            }

            // Tests that the class enables drag and drop
            TEST_METHOD(EnableDragDrop)
            {
                HWND window = create_test_window(L"TRViewFileDropperTests");
                FileDropper dropper(window);
                LONG_PTR style = GetWindowLongPtr(window, GWL_EXSTYLE);
                Assert::AreEqual<LONG_PTR>(WS_EX_ACCEPTFILES, style & WS_EX_ACCEPTFILES);
            }
        };
    }
}