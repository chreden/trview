/// @file Timer.h
/// @brief Class that keeps track of time and records time between updates.
/// 
/// Polls a time source and keeps track of the interval between calls. This can be
/// used to make sure that operations are scaled by time, so that they are framerate
/// independent.

#pragma once

#include <cstdint>
#include <functional>

namespace trview
{
    /// Keeps track of time and records time between updates by polling a time source.
    class Timer
    {
    public:
        /// Creates a new timer that will poll the specified time source.
        /// @param time_source The time source to poll on each update.
        explicit Timer(const std::function<float()>& time_source);

        /// Update the timer. This will poll the time source and will update the elapsed
        /// and total values of the timer.
        void update();

        /// Get the time in seconds that elapsed between the latest call to the time source (via update) and
        /// the previous call to the time source.
        /// @returns The time that elapsed between the last call and the previous call to time source.
        float elapsed() const;

        /// Get the total time in seconds that have elapsed over the lifetime of the timer.
        /// @returns The time in seconds since the timer was started.
        float total() const;

        /// Reset the counters on the timer - this will reset the elapsed and total times that have been
        /// recorded so far.
        void reset();
    private:
        std::function<float()> _time_source;
        float _previous;
        float _elapsed;
        float _total;
    };

    /// Creates a time source that uses the Windows high performance counter to measure the passage of time.
    /// @returns The time source function to call.
    std::function<float()> default_time_source();
}
