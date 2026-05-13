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

        Stopwatch(Clock::time_point begin) : m_begin{begin} {}

        // Get time duration to the end.
        template<typename T>
        T Elapsed(Clock::time_point end) const noexcept
        {
            return std::chrono::duration_cast<T>(end - m_begin);
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
            m_begin = Clock::now();
        }

        // Reset to the Clock::now() and return elapsed time.
        template<typename T>
        T Restart()
        {
            auto const beg = m_begin;
            Reset();
            return std::chrono::duration_cast<T>(m_begin - beg);
        }

        // Reset to the Clock::now() and return elapsed time.
        Duration Restart()
        {
            return Restart<Duration>();
        }

        Clock::time_point Begin() const noexcept { return m_begin; }

    private:
        Clock::time_point m_begin {Clock::now()};
    };

} // Rc
