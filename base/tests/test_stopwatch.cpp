#include "gtest/gtest.h"
#include "base/stopwatch.h"

using namespace Rc;

TEST(StopwatchTest, Stopwatch)
{
    Stopwatch s;
    s.Elapsed();
    s.Elapsed<std::chrono::seconds>();
    s.Reset();
    s.Restart();
    s.Restart<std::chrono::seconds>();

    SUCCEED();
}
