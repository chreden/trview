#include "CppUnitTest.h"

#include <trview.common/Timer.h>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace trview
{
    namespace
    {
        struct TimeSource
        {
            uint32_t times_called;
            float time;

            float poll()
            {
                ++times_called;
                return time;
            }

            TimeSource()
                : TimeSource(0.0f)
            {
            }

            TimeSource(float initial_time)
                : time(initial_time)
            {
            }

            operator std::function<float()>()
            {
                return std::bind(&TimeSource::poll, this);
            }
        };
    }

    TEST_CLASS(TimerTests)
    {
        // Tests that when the timer is constructed, the timer has the correct
        // initial values and that the time source is polled.
        TEST_METHOD(Constructor)
        {
            TimeSource source;
            Timer timer(source);

            Assert::AreEqual(0.0f, timer.elapsed());
            Assert::AreEqual(0.0f, timer.total());
            Assert::AreEqual(1u, source.times_called);
        }

        // Tests that when there is an initial time source value of non-zero the elapsed
        // and total times are not affected.
        TEST_METHOD(InitialOffset)
        {
            TimeSource source{ 15.0f };
            Timer timer(source);

            Assert::AreEqual(0.0f, timer.elapsed());
            Assert::AreEqual(0.0f, timer.total());
            Assert::AreEqual(1u, source.times_called);
        }

        // Tests that when the time is advanced and the timer is updated the timer correctly
        // updates the elapsed time property.
        TEST_METHOD(Elapsed)
        {
            TimeSource source;
            Timer timer(source);

            Assert::AreEqual(0.0f, timer.elapsed());
            Assert::AreEqual(1u, source.times_called);

            source.time = 1.0f;
            timer.update();

            Assert::AreEqual(1.0f, timer.elapsed());
            Assert::AreEqual(2u, source.times_called);
        }

        // Tests that when the time is advanced and the timer is update the timer correctly
        // updates the total time property.
        TEST_METHOD(Total)
        {
            TimeSource source;
            Timer timer(source);

            Assert::AreEqual(0.0f, timer.total());
            Assert::AreEqual(1u, source.times_called);

            source.time = 10.0f;
            timer.update();

            Assert::AreEqual(10.0f, timer.total());
            Assert::AreEqual(2u, source.times_called);
        }

        // Tests that when the time is reset, the total and elapsed properties are reset.
        TEST_METHOD(Reset)
        {
            TimeSource source;
            Timer timer(source);

            source.time = 10.0f;
            timer.update();

            Assert::AreEqual(10.0f, timer.elapsed());
            Assert::AreEqual(10.0f, timer.total());

            timer.reset();

            Assert::AreEqual(0.0f, timer.elapsed());
            Assert::AreEqual(0.0f, timer.total());
        }
    };
}
