#include "CppUnitTest.h"

#include <string>
#include <trview.common/Event.h>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace trview
{
    TEST_CLASS(EventTests)
    {
        /// Test that chaining an event to another event works correctly.
        TEST_METHOD(Chaining)
        {
            int value = 0;
            int times_called = 0;

            Event<int> first, second;
            first += second;
            second += [&](int parameter)
            {
                ++times_called;
                value = parameter;
            };
            first(100);

            Assert::AreEqual(1, times_called);
            Assert::AreEqual(100, value);
        }

        /// Test that a function is called when the event is raised.
        TEST_METHOD(Function)
        {
            int times_called = 0;
            int value = 0;

            Event<int> event;
            event += [&](auto v) {++times_called; value = v; };
            event(125);

            Assert::AreEqual(1, times_called);
            Assert::AreEqual(125, value);
        }

        TEST_METHOD(MultipleArguments)
        {
            int times_called = 0;
            int value_one = 0;
            std::string value_two;

            Event<int, std::string> event;
            event += [&](int v, std::string v2) { ++times_called; value_one = v; value_two = v2; };
            event(100, "Test");

            Assert::AreEqual(1, times_called);
            Assert::AreEqual(100, value_one);
            Assert::AreEqual(std::string("Test"), value_two);
        }

        TEST_METHOD(NoArguments)
        {
            int times_called = 0;

            Event<void> event;
            event += [&]() {++times_called; };
            event();

            Assert::AreEqual(1, times_called);
        }
    };
}
