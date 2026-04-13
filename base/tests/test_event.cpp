#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "base/event.h"

using namespace Rc;

class Dispatcher
{
public:
    using TestEvent = Event<int>;

    void AddTestEvent(TestEvent::Handler& handler)
    {
        event.Add(handler);
    }

    void Dispatch(int value)
    {
        event.Dispatch(value);
    }
    
private:
    TestEvent event; 
};

class Listener
{
public:
    void Bind(Dispatcher& dispatcher)
    {
        dispatcher.AddTestEvent(handler);
    }

    virtual void OnTestEvent(int value) = 0;

private:
    Dispatcher::TestEvent::Handler handler {this, &Listener::OnTestEvent};
};

class ListenerMock : public Listener
{
public:
    MOCK_METHOD(void, OnTestEvent, (int value), (override));
};

TEST(EventTest, Dispatch)
{
    Dispatcher dispatcher;
    ListenerMock listener;
    listener.Bind(dispatcher);

    EXPECT_CALL(listener, OnTestEvent(8));

    dispatcher.Dispatch(8);
}
