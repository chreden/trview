#include "CppUnitTest.h"

#include <trview.ui/Button.h>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace trview
{
    namespace ui
    {
        namespace tests
        {
            TEST_CLASS(ButtonTests)
            {
            public:
                // Tests that the click event is raised when the button is clicked on.
                TEST_METHOD(ClickEventRaised)
                {
                    Button button(Point(), Size(20, 20), graphics::Texture(), graphics::Texture());
                    bool raised = false;
                    button.on_click += [&raised]() { raised = true; };
                    button.mouse_down(Point());
                    Assert::IsTrue(raised);
                }
            };
        }
    }
}
