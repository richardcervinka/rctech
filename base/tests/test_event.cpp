#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "base/event.h"

class Dispatcher
{
public:
    using Event = Rc::Event<int>;

    Event event; 
};

class Listener
{
public:
    virtual void OnTestEvent1(int value) = 0;
    virtual void OnTestEvent2(int const& value) = 0;

    Dispatcher::Event::Handler handler1 {this, &Listener::OnTestEvent1};
    Dispatcher::Event::Handler handler2 {this, &Listener::OnTestEvent2};
};

class ListenerMock : public Listener
{
public:
    MOCK_METHOD(void, OnTestEvent1, (int value), (override));
    MOCK_METHOD(void, OnTestEvent2, (int const& value), (override));
};

TEST(EventHandler, ConstructFromLambda_ValueParam)
{
    Rc::Event<int>::Handler handler {[](int){}};
    SUCCEED();
}

TEST(EventHandler, ConstructFromLambda_ConstRefParam)
{
    Rc::Event<int>::Handler handler {[](int const&){}};
    SUCCEED();
}

TEST(EventHandler_Void, ConstructFromLambda_NoParam)
{
    Rc::Event<void>::Handler handler {[](){}};
}

TEST(EventDispatcher, MultipleHandlers_AllReceiveEvent)
{
    Dispatcher dispatcher;
    ListenerMock listener;

    dispatcher.event.Add(listener.handler1);
    dispatcher.event.Add(listener.handler2);

    EXPECT_CALL(listener, OnTestEvent1(8)).Times(1);
    EXPECT_CALL(listener, OnTestEvent2(8)).Times(1);

    dispatcher.event.Dispatch(8);
}

TEST(EventDispatcher, DuplicateHandler_CalledOnce)
{
    Dispatcher dispatcher;
    ListenerMock listener;

    dispatcher.event.Add(listener.handler1);
    dispatcher.event.Add(listener.handler1);

    EXPECT_CALL(listener, OnTestEvent1(8)).Times(1);

    dispatcher.event.Dispatch(8);
}

TEST(EventDispatcher, UnboundHandler_NotCalled)
{
    Dispatcher dispatcher;
    ListenerMock listener;

    dispatcher.event.Add(listener.handler1);
    dispatcher.event.Add(listener.handler2);
    listener.handler1.Unbind();

    EXPECT_CALL(listener, OnTestEvent1(8)).Times(0);
    EXPECT_CALL(listener, OnTestEvent2(8)).Times(1);

    dispatcher.event.Dispatch(8);
}

TEST(EventDispatcher, HandlerCanUnbindDuringDispatch)
{
    Rc::Event<int> event;
    ListenerMock listener;

    // The handler1 will unbind itself.
    EXPECT_CALL(listener, OnTestEvent1(8)).WillOnce([&](int){ listener.handler1.Unbind(); });

    // The handler2 will be still called.
    EXPECT_CALL(listener, OnTestEvent2(8)).Times(1);

    event.Add(listener.handler1);
    event.Add(listener.handler2);
    event.Dispatch(8);

    // The only handler2 will called.
    EXPECT_CALL(listener, OnTestEvent1(8)).Times(0);
    EXPECT_CALL(listener, OnTestEvent2(8)).Times(1);

    event.Dispatch(8);
}

TEST(EventDispatcher, HandlerCanUnbindAnotherHandler)
{
    Rc::Event<int> event;
    ListenerMock listener;

    // handler1 will unbind handler2 during dispatch
    EXPECT_CALL(listener, OnTestEvent1(8)).WillOnce([&](int){ listener.handler2.Unbind(); });

    // handler2 must NOT be called during the same dispatch
    EXPECT_CALL(listener, OnTestEvent2).Times(0);

    event.Add(listener.handler1);
    event.Add(listener.handler2);

    // First dispatch: handler1 runs and unbinds handler2 before it would be called
    event.Dispatch(8);

    // Second dispatch: handler2 must remain unbound
    EXPECT_CALL(listener, OnTestEvent1(8)).Times(1);
    EXPECT_CALL(listener, OnTestEvent2).Times(0);

    event.Dispatch(8);
}

TEST(EventDispatcher, HandlerCanAddAnotherHandlerDuringDispatch)
{
    Rc::Event<int> event;
    ListenerMock listener;

    // handler1 will add handler2 during dispatch
    EXPECT_CALL(listener, OnTestEvent1(8)).WillOnce([&](int){ event.Add(listener.handler2); });

    // handler2 must NOT be called during the same dispatch
    EXPECT_CALL(listener, OnTestEvent2).Times(0);

    event.Add(listener.handler1);

    // First dispatch: handler2 is added but must not run yet
    event.Dispatch(8);

    // Second dispatch: handler2 must now be called
    EXPECT_CALL(listener, OnTestEvent1(8)).Times(1);
    EXPECT_CALL(listener, OnTestEvent2(8)).Times(1);

    event.Dispatch(8);
}

TEST(EventDispatcher, HandlerCanUnbindItselfTwice)
{
    Rc::Event<int> event;
    ListenerMock listener;

    // handler1 will unbind itself twice during dispatch
    EXPECT_CALL(listener, OnTestEvent1(8)).WillOnce([&](int){
        listener.handler1.Unbind();
        listener.handler1.Unbind(); // Second unbind must be harmless
    });

    event.Add(listener.handler1);

    // First dispatch: handler1 runs once
    event.Dispatch(8);

    // Second dispatch: handler1 must NOT be called anymore
    EXPECT_CALL(listener, OnTestEvent1).Times(0);

    event.Dispatch(8);
}

TEST(EventDispatcher, HandlersAreCalledInRegistrationOrder)
{
    Rc::Event<int> event;
    ListenerMock listener;

    {
        ::testing::InSequence seq;

        EXPECT_CALL(listener, OnTestEvent1(8));
        EXPECT_CALL(listener, OnTestEvent2(8));
    }

    event.Add(listener.handler1);
    event.Add(listener.handler2);
    event.Dispatch(8);
}

TEST(EventDispatcher, NewlyAddedHandlerIsNotCalledInSameDispatch)
{
    Rc::Event<int> event;
    ListenerMock listener;

    EXPECT_CALL(listener, OnTestEvent1(8)).WillOnce([&](int){
        event.Add(listener.handler2); // Added during dispatch
    });

    EXPECT_CALL(listener, OnTestEvent2).Times(0); // must not run yet

    event.Add(listener.handler1);
    event.Dispatch(8);

    // Now handler2 must run
    EXPECT_CALL(listener, OnTestEvent1(8)).Times(1);
    EXPECT_CALL(listener, OnTestEvent2(8)).Times(1);
    event.Dispatch(8);
}

TEST(EventDispatcher, RemovingHandlerDuringDispatchDoesNotAffectOrder)
{
    Rc::Event<int> event;
    ListenerMock listener;

    EXPECT_CALL(listener, OnTestEvent1(8)).WillOnce([&](int){
        listener.handler1.Unbind(); // Remove itself
    });

    EXPECT_CALL(listener, OnTestEvent2(8)).Times(1);

    event.Add(listener.handler1);
    event.Add(listener.handler2);
    event.Dispatch(8);
}