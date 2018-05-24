#include "CppUnitTest.h"

#include <trview.common/Event.h>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace trview
{
    TEST_CLASS(EventTests)
    {
        TEST_METHOD(Chaining)
        {
            bool called = false;
            Event<int> first, second;
            first += second;
            second += [&](int) { called = true; };
            first(100);
            Assert::IsTrue(called);
        }
    };
}
