#pragma once

#include <functional>
#include <thread>
#include <cassert>

namespace Rc
{
    // TODO: class AsyncFence

    class Worker
    {
    public:
        Worker()
        {
            thread = std::jthread([this](std::stop_token const& stop){ Thread(stop); });
        }

        ~Worker()
        {
            Kill();
        }

        Worker(Worker const&) = delete;
        Worker(Worker&&) = delete;
        Worker& operator=(Worker const&) = delete;
        Worker& operator=(Worker&&) = delete;

        void Call(std::function<void()> work);

        void Kill() noexcept;

        bool Idle() const noexcept
        {
            return signal == Signal::None;
        }

    private:
        enum class Signal
        {
            None,
            Work,
            Kill
        };

        void Notify(Signal signal) noexcept;

        void Thread(std::stop_token const& stop);

        std::jthread thread;

        std::atomic<Signal> signal {Signal::None};

        std::function<void()> work;
    };

} // Rc