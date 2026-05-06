#pragma once

#include <memory>
#include <functional>
#include <vector>
#include <cassert>
#include <stdexcept>

namespace Rc
{
    template<typename E>
    class EventHandler;

    template<typename E>
    class Event;

    template<typename E>
    class EventDispatcher
    {
    public:
        EventDispatcher() = default;
        ~EventDispatcher() = default;
        EventDispatcher(EventDispatcher const&) = delete;
        EventDispatcher(EventDispatcher&&) = delete;
        EventDispatcher& operator=(EventDispatcher const&) = delete;
        EventDispatcher& operator=(EventDispatcher&&) = delete;

        static void Bind(std::shared_ptr<EventDispatcher<E>>& dispatcher, EventHandler<E>& handler);

        void Remove(EventHandler<E> const& handler) noexcept;

        void Dispatch(E const& e) noexcept;

    private:
        std::vector<EventHandler<E> const*> m_handlers;

        // Reentrant dispatch guard.
        bool m_dispatching {false};
    };

    template<typename E>
    class EventHandler
    {
    public:
        using Dispatcher = EventDispatcher<E>;

        EventHandler() = default;

        explicit EventHandler(std::function<void(E const&)> callback) :
            m_callback{std::move(callback)}
        {}

        template<typename T>
        EventHandler(T* instance, void(T::*callback)(E const&))
        {
            m_callback = [instance, callback](E const& e)
            {
                return std::invoke(callback, instance, e);
            };
        }

        template<typename T>
        EventHandler(T* instance, void(T::*callback)(E))
        {
            m_callback = [instance, callback](E const& e)
            {
                return std::invoke(callback, instance, e);
            };
        }

        EventHandler(EventHandler const&) = delete;
        EventHandler(EventHandler&&) = delete;
        EventHandler& operator=(EventHandler const&) = delete;
        EventHandler& operator=(EventHandler&&) = delete;

        ~EventHandler() noexcept
        {
            Unbind();
        }

        // Remove from the bound dispatcher.
        void Unbind() noexcept
        {
            if (auto ptr = m_dispatcher.lock())
            {
                ptr->Remove(*this);
            }
            m_dispatcher.reset();
        }

    private:
        friend class EventDispatcher<E>;

        void Call(E const& event) const
        {
            if (m_callback)
            {
                m_callback(event);
            }
        }

        // Only the one dispatcher can be connected with the handler.
        std::weak_ptr<EventDispatcher<E>> m_dispatcher;

        std::function<void(E const&)> m_callback {nullptr};
    };

    // EventDispatcher

    template<typename E>
    inline void EventDispatcher<E>::Bind(std::shared_ptr<EventDispatcher<E>>& dispatcher, EventHandler<E>& handler)
    {
        assert(dispatcher != nullptr);

        // Remove the handler from the current dispatcher.
        if (auto ptr = handler.m_dispatcher.lock())
        {
            ptr->Remove(handler);
        }

        handler.m_dispatcher = dispatcher;
        dispatcher->m_handlers.push_back(&handler);
    }

    template<typename E>
    inline void EventDispatcher<E>::Remove(EventHandler<E> const& handler) noexcept
    {
        for (auto& ref : m_handlers)
        {
            if (ref == &handler)
            {
                ref = nullptr;
            }
        }
    }

    template<typename E>
    inline void EventDispatcher<E>::Dispatch(E const& e) noexcept
    {
        assert(!m_dispatching && "Reentrant dispatch");

        m_dispatching = true;

        std::size_t const end = m_handlers.size();

        // Call the handler.
        for (std::size_t i = 0; i < end; i++)
        {
            try
            {
                if (auto const* handler = m_handlers[i])
                {
                    handler->Call(e);
                }
            }
            catch (...)
            {
                // TODO: Use engine log
            }
        }

        // Remove null handlers.
        std::erase_if(m_handlers, [](auto h){ return h == nullptr; });

        m_dispatching = false;
    }

    // Dispatcher wrapper to reduce std::shared_ptr verbosity...
    template<typename E>
    class Event
    {
    public:
        using Handler = EventHandler<E>;
        using Payload = E;

        void Add(EventHandler<E>& handler)
        {
            EventDispatcher<E>::Bind(m_dispatcher, handler);
        }

        void Remove(EventHandler<E> const& handler) noexcept
        {
            m_dispatcher->Remove(handler);
        }

        void Dispatch(E const& e) noexcept
        {
            m_dispatcher->Dispatch(e);
        }

    private:
        std::shared_ptr<EventDispatcher<E>> m_dispatcher {std::make_shared<EventDispatcher<E>>()};
    };

    // void specialization

    template<>
    class EventHandler<void>;

    template<>
    class Event<void>;

    template<>
    class EventDispatcher<void>
    {
    public:
        EventDispatcher() = default;
        ~EventDispatcher() = default;
        EventDispatcher(EventDispatcher const&) = delete;
        EventDispatcher(EventDispatcher&&) = delete;
        EventDispatcher& operator=(EventDispatcher const&) = delete;
        EventDispatcher& operator=(EventDispatcher&&) = delete;

        static void Bind(std::shared_ptr<EventDispatcher<void>>& dispatcher, EventHandler<void>& handler);

        void Remove(EventHandler<void> const& handler) noexcept;

        void Dispatch() noexcept;

    private:
        std::vector<EventHandler<void> const*> m_handlers;

        // Reentrant dispatch guard.
        bool m_dispatching {false};
    };

    template<>
    class EventHandler<void>
    {
    public:
        using Dispatcher = EventDispatcher<void>;

        EventHandler() = default;

        explicit EventHandler(std::function<void()> callback) :
            m_callback{std::move(callback)}
        {}

        template<typename T>
        EventHandler(T* instance, void(T::*callback)())
        {
            m_callback = [instance, callback]()
            {
                return std::invoke(callback, instance);
            };
        }

        EventHandler(EventHandler const&) = delete;
        EventHandler(EventHandler&&) = delete;
        EventHandler& operator=(EventHandler const&) = delete;
        EventHandler& operator=(EventHandler&&) = delete;

        ~EventHandler() noexcept
        {
            Unbind();
        }

        // Remove from the bound dispatcher.
        void Unbind() noexcept
        {
            if (auto ptr = m_dispatcher.lock())
            {
                ptr->Remove(*this);
            }
            m_dispatcher.reset();
        }

    private:
        friend class EventDispatcher<void>;

        void Call() const
        {
            if (m_callback)
            {
                m_callback();
            }
        }

        // Only the one dispatcher can be connected with the handler.
        std::weak_ptr<EventDispatcher<void>> m_dispatcher;

        std::function<void()> m_callback {nullptr};
    };

    // EventDispatcher

    inline void EventDispatcher<void>::Bind(std::shared_ptr<EventDispatcher<void>>& dispatcher, EventHandler<void>& handler)
    {
        assert(dispatcher != nullptr);

        // Remove the handler from the current dispatcher.
        if (auto ptr = handler.m_dispatcher.lock())
        {
            ptr->Remove(handler);
        }

        handler.m_dispatcher = dispatcher;
        dispatcher->m_handlers.push_back(&handler);
    }

    inline void EventDispatcher<void>::Remove(EventHandler<void> const& handler) noexcept
    {
        for (auto& ref : m_handlers)
        {
            if (ref == &handler)
            {
                ref = nullptr;
            }
        }
    }

    inline void EventDispatcher<void>::Dispatch() noexcept
    {
        assert(!m_dispatching && "Reentrant dispatch");
        
        m_dispatching = true;

        std::size_t const end = m_handlers.size();

        // Call the handler.
        for (std::size_t i = 0; i < end; i++)
        {
            try
            {
                if (auto const* handler = m_handlers[i])
                {
                    handler->Call();
                }
            }
            catch (...)
            {
                // TODO: Use engine log
            }
        }

        // Remove null handlers.
        std::erase_if(m_handlers, [](auto h){ return h == nullptr; });

        m_dispatching = false;
    }

    // Dispatcher wrapper to reduce std::shared_ptr verbosity...
    template<>
    class Event<void>
    {
    public:
        using Handler = EventHandler<void>;
        using Payload = void;

        void Add(EventHandler<void>& handler)
        {
            EventDispatcher<void>::Bind(m_dispatcher, handler);
        }

        void Remove(EventHandler<void> const& handler) noexcept
        {
            m_dispatcher->Remove(handler);
        }

        void Dispatch() noexcept
        {
            m_dispatcher->Dispatch();
        }

    private:
        std::shared_ptr<EventDispatcher<void>> m_dispatcher {std::make_shared<EventDispatcher<void>>()};
    };

} // Rc
