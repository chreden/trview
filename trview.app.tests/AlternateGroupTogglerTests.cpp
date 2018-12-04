#include "CppUnitTest.h"
#include <trview.app/AlternateGroupToggler.h>
#include <trview.tests.common/Window.h>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace trview
{
    namespace tests
    {
        TEST_CLASS(AlternateGroupTogglerTests)
        {
            /// Tests that when a number key is pressed an alternate group is toggled.
            TEST_METHOD(NumberKeys)
            {
                auto window = create_test_window(L"AlternateGroupToggler");

                for (uint32_t i = 0; i < 10; ++i)
                {
                    AlternateGroupToggler toggler(window);

                    bool called = false;
                    uint32_t called_group = 0;

                    auto token = toggler.on_alternate_group += [&](uint32_t group)
                    {
                        called = true;
                        called_group = group;
                    };

                    toggler.process_message(window, WM_CHAR, L'0' + i, 0);

                    Assert::IsTrue(called, L"Callback was not raised");
                    Assert::AreEqual(called_group, i);
                }
            }

            /// Tests that if a non-numeric character is sent, no event is raised.
            TEST_METHOD(Characters)
            {
                auto window = create_test_window(L"AlternateGroupToggler");
                AlternateGroupToggler toggler(window);

                bool called = false;
                uint32_t called_group = 0;

                auto token = toggler.on_alternate_group += [&](uint32_t group)
                {
                    called = true;
                    called_group = group;
                };

                toggler.process_message(window, WM_CHAR, L'A', 0);

                Assert::IsFalse(called, L"Callback should not be raised");
                Assert::AreEqual(called_group, 0u);
            }
        };
    }
}