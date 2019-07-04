#include "gtest/gtest.h"
#include <trview.common/Timer.h>

using namespace trview;

namespace
{
    // Class used to provide a way of setting the time and keeping track
    // of the number of times that the timer calls the time source function.
    struct TimeSource
    {
        uint32_t times_called{ 0u };
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

/// Tests that when the timer is constructed, the timer has the correct
/// initial values and that the time source is polled.
TEST(Timer, Constructor)
{
    TimeSource source;
    Timer timer(source);

    ASSERT_EQ(0.0f, timer.elapsed());
    ASSERT_EQ(0.0f, timer.total());
    ASSERT_EQ(1u, source.times_called);
}

/// Tests that when there is an initial time source value of non-zero the elapsed
/// and total times are not affected.
TEST(Timer, InitialOffset)
{
    TimeSource source{ 15.0f };
    Timer timer(source);

    ASSERT_EQ(0.0f, timer.elapsed());
    ASSERT_EQ(0.0f, timer.total());
    ASSERT_EQ(1u, source.times_called);
}

/// Tests that when the time is advanced and the timer is updated the timer correctly
/// updates the elapsed time property.
TEST(Timer, Elapsed)
{
    TimeSource source;
    Timer timer(source);

    ASSERT_EQ(0.0f, timer.elapsed());
    ASSERT_EQ(1u, source.times_called);

    source.time = 1.0f;
    timer.update();

    ASSERT_EQ(1.0f, timer.elapsed());
    ASSERT_EQ(2u, source.times_called);
}

/// Tests that when the time is advanced and the timer is update the timer correctly
/// updates the total time property.
TEST(Timer, Total)
{
    TimeSource source;
    Timer timer(source);

    ASSERT_EQ(0.0f, timer.total());
    ASSERT_EQ(1u, source.times_called);

    source.time = 10.0f;
    timer.update();

    ASSERT_EQ(10.0f, timer.total());
    ASSERT_EQ(2u, source.times_called);
}

/// Tests that when the time is reset, the total and elapsed properties are reset.
TEST(Timer, Reset)
{
    TimeSource source;
    Timer timer(source);

    source.time = 10.0f;
    timer.update();

    ASSERT_EQ(10.0f, timer.elapsed());
    ASSERT_EQ(10.0f, timer.total());

    timer.reset();

    ASSERT_EQ(0.0f, timer.elapsed());
    ASSERT_EQ(0.0f, timer.total());
}

/// Tests that the update function calls the time source.
TEST(Timer, Update)
{
    TimeSource source;
    Timer timer(source);
    ASSERT_EQ(1u, source.times_called);
    timer.update();
    ASSERT_EQ(2u, source.times_called);
}
