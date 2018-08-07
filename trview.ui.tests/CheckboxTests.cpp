#include "CppUnitTest.h"

#include <trview.ui/Checkbox.h>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace trview
{
    namespace ui
    {
        namespace tests
        {
            TEST_CLASS(CheckboxTests)
            {
            public:
                // Tests that the state change event is not raised when the set_state function
                // is called rather than the user clicking on the control.
                TEST_METHOD(StateChangeEventNotRaised)
                {
                    Checkbox checkbox(Point(), Size(20, 20), graphics::Texture(), graphics::Texture());
                    bool raised = false;
                    auto token = checkbox.on_state_changed += [&](bool)
                    {
                        raised = true;
                    };

                    Assert::IsFalse(checkbox.state());
                    checkbox.set_state(true);
                    Assert::IsFalse(raised);
                    Assert::IsTrue(checkbox.state());
                }

                // Tests that the state change event is raised when the user clicks on the control.
                TEST_METHOD(StateChangeEventRaised)
                {
                    Checkbox checkbox(Point(), Size(20, 20), graphics::Texture(), graphics::Texture());
                    
                    bool raised = false;
                    uint32_t times = 0;
                    bool raised_state = false;
                    auto token = checkbox.on_state_changed += [&raised, &times, &raised_state](bool state)
                    {
                        raised = true;
                        raised_state = state;
                        ++times;
                    };

                    checkbox.mouse_down(Point());
                    Assert::IsTrue(raised);
                    Assert::AreEqual(1u, times);
                    Assert::AreEqual(true, raised_state);
                    Assert::AreEqual(raised_state, checkbox.state());
                }

                // Tests that the state of the checkbox can cycle between checked and unchecked and raises
                // an event each time the state changes.
                TEST_METHOD(StateChangeCycle)
                {
                    Checkbox checkbox(Point(), Size(20, 20), graphics::Texture(), graphics::Texture());

                    std::vector<bool> states;
                    auto token = checkbox.on_state_changed += [&states](bool state)
                    {
                        states.push_back(state);
                    };

                    checkbox.mouse_down(Point());
                    checkbox.mouse_down(Point());
                    Assert::AreEqual((std::size_t)2u, states.size());
                    Assert::AreEqual(true, static_cast<bool>(states[0]));
                    Assert::AreEqual(false, static_cast<bool>(states[1]));
                }
            };
        }
    }
}