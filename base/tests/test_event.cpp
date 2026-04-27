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
    virtual void OnTestEvent2(int value) = 0;

    Dispatcher::Event::Handler handler1 {this, &Listener::OnTestEvent1};
    Dispatcher::Event::Handler handler2 {this, &Listener::OnTestEvent2};
};

class ListenerMock : public Listener
{
public:
    MOCK_METHOD(void, OnTestEvent1, (int value), (override));
    MOCK_METHOD(void, OnTestEvent2, (int value), (override));
};

TEST(EventTest, Dispatch)
{
    Dispatcher dispatcher;
    ListenerMock listener;

    dispatcher.event.Add(listener.handler1);
    dispatcher.event.Add(listener.handler1);
    dispatcher.event.Add(listener.handler2);

    EXPECT_CALL(listener, OnTestEvent1(8)).Times(1);
    EXPECT_CALL(listener, OnTestEvent2(8)).Times(1);

    dispatcher.event.Dispatch(8);

    listener.handler1.Unbind();

    EXPECT_CALL(listener, OnTestEvent1(8)).Times(0);
    EXPECT_CALL(listener, OnTestEvent2(8)).Times(1);

    dispatcher.event.Dispatch(8);
}
