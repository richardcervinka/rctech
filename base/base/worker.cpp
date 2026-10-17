#include "worker.h"

namespace Rc
{
    void Worker::Call(std::function<void()> work)
    {
        assert(Idle());
        assert(work);

        this->work = std::move(work);

        Notify(Signal::Work);
    }

    void Worker::Kill() noexcept
    {
        thread.request_stop();

        Notify(Signal::Kill);

        if (thread.joinable())
        {
            thread.join();
        }
    }

    void Worker::Notify(Signal signal) noexcept
    {
        this->signal = signal;
        std::atomic_notify_one(&this->signal);
    }

    void Worker::Thread(std::stop_token const& stop)
    {
        while (!stop.stop_requested())
        {
            std::atomic_wait(&signal, Signal::None);

            if (!stop.stop_requested())
            {
                work();
            }

            signal = Signal::None;
        }
    }

} // Rc