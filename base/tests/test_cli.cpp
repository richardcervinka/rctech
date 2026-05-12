#include <gtest/gtest.h>
#include "base/cli.h"
#include <optional>
#include <array>

using namespace Rc;

class CmdArgs
{
public:
    CmdArgs(int argc, char* argv[])
    {
        for (int i = 1; i < argc; i++)
        {
            if (auto arg = Cli::Parse(argv[i]))
            {
                auto const [name, value] = arg.value();

                if (name == "option")
                {
                    option = value;
                }
                else if (name == "flag")
                {
                    flag = true;
                }
            }
        }
    }

    std::optional<std::string> option;
    std::optional<bool> flag;
};

TEST(Cli_Parse, Option)
{
    char const* argv[] {"test", "--option=10"};

    CmdArgs args(std::size(argv), const_cast<char**>(argv));

    ASSERT_TRUE(args.option.has_value());
    EXPECT_EQ(args.option.value(), "10");
}

TEST(Cli_Parse, Flag)
{
    char const* argv[] {"test", "--flag"};

    CmdArgs args(std::size(argv), const_cast<char**>(argv));

    ASSERT_TRUE(args.flag.has_value());
    EXPECT_TRUE(args.flag.value());
}