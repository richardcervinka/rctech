#pragma once

#include <chrono>

namespace Rc
{
    template<typename C = std::chrono::high_resolution_clock>
    class Stopwatch
    {
    public:
        using Clock = C;
        using Duration = C::duration;

        Stopwatch() = default;

        Stopwatch(Clock::time_point begin) : begin{begin} {}

        // Get time duration to the end.
        template<typename T>
        T Elapsed(Clock::time_point end) const noexcept
        {
            return std::chrono::duration_cast<T>(end - begin);
        }

        // Get time duration to the Clock::now() as T.
        template<typename T = Duration>
        T Elapsed() const
        {
            return Elapsed<T>(Clock::now());
        }

        // Reset to the Clock::now()
        void Reset()
        {
            begin = Clock::now();
        }

        // Reset to the Clock::now() and return elapsed time.
        template<typename T>
        T Restart()
        {
            auto const beg = begin;
            Reset();
            return std::chrono::duration_cast<T>(begin - beg);
        }

        // Reset to the Clock::now() and return elapsed time.
        Duration Restart()
        {
            return Restart<Duration>();
        }

        Clock::time_point Begin() const noexcept { return begin; }

    private:
        Clock::time_point begin {Clock::now()};
    };

} // Rc
