#include "gtest/gtest.h"
#include "base/stopwatch.h"

using namespace Rc;
using namespace std::chrono;

TEST(StopwatchTest, Stopwatch)
{
    Stopwatch<steady_clock> s(steady_clock::now());
    
    s.Begin();
    s.Elapsed();
    s.Elapsed<seconds>();
    s.Reset();
    s.Restart();
    s.Restart<seconds>();

    SUCCEED();
}
