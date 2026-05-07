#include "cli.h"
#include <cctype>

namespace Rc::Str
{
    template<>
    std::string From(Rc::Cli::Error value)
    {
        switch (value)
        {
            case Rc::Cli::Error::BadPrefix:
                return "BadPrefix";
            case Rc::Cli::Error::BadName:
                return "BadName";
            case Rc::Cli::Error::BadValue:
                return "BadValue";
            case Rc::Cli::Error::BadFormat:
                return "BadFormat";
        }

        assert(false && "Unknown Rc::Cli::Error value");
        throw std::invalid_argument("Unknown Rc::Cli::Error value");
    }

    template<>
    std::expected<Rc::Cli::Error, std::errc> To(std::string_view str)
    {
        if (str ==  "BadPrefix")
        {
            return Rc::Cli::Error::BadPrefix;
        }
        if (str ==  "BadName")
        {
            return Rc::Cli::Error::BadName;
        }
        if (str ==  "BadValue")
        {
            return Rc::Cli::Error::BadValue;
        }
        if (str ==  "BadFormat")
        {
            return Rc::Cli::Error::BadFormat;
        }

        assert(false && "Unknown Rc::Cli::Error string");
        throw std::invalid_argument("Unknown Rc::Cli::Error string");
    }

} // Rc::Str

namespace Rc::Cli
{
    std::expected<std::pair<std::string_view, std::optional<std::string_view>>, Error> Parse(std::string_view arg)
    {
        enum class State
        {
            Start,
            Dash,
            Option,
            Name,
            Value
        };

        auto state = State::Start;

        std::size_t name_size = 0;

        for (const char ch : arg)
        {
            switch (state)
            {
                case State::Start:
                    if (ch == '-')
                    {
                        state = State::Dash;
                    }
                    else
                    {
                        return std::unexpected{Error::BadPrefix};
                    }
                    break;

                case State::Dash:
                    if (ch == '-')
                    {
                        state = State::Option;
                    }
                    else
                    {
                        return std::unexpected{Error::BadPrefix};
                    }
                    break;

                case State::Option:
                    if (std::isalpha(static_cast<unsigned char>(ch)) != 0)
                    {
                        name_size++;
                        state = State::Name;
                    }
                    else
                    {
                        return std::unexpected{Error::BadName};
                    }
                    break;

                case State::Name:
                    if ((ch == '-') || (ch == '_') || (std::isalnum(static_cast<unsigned char>(ch)) != 0) )
                    {
                        name_size++;
                    }
                    else if (ch == '=')
                    {
                        state = State::Value;
                    }
                    else
                    {
                        return std::unexpected{Error::BadName};
                    }
                    break;

                case State::Value:
                    if (std::isprint(static_cast<unsigned char>(ch)) == 0)
                    {
                        return std::unexpected{Error::BadValue};
                    }
                    break;
            }
        }

        if (state == State::Name)
        {
            return std::make_pair(arg.substr(2, name_size), std::nullopt);
        }
        if (state == State::Value)
        {
            return std::make_pair(arg.substr(2, name_size), arg.substr(2 + name_size + 1));
        }

        return std::unexpected{Error::BadFormat};
    }

    std::map<std::string, std::string> Map(int argc, char* argv[])
    {
        std::map<std::string, std::string> result;

        for (int i = 0; i < argc; i++)
        {
            if (auto arg = Parse(std::string_view(argv[i])))
            {
                result[std::string{arg.value().first}] = arg.value().second.value_or({});
            }
        }

        return result;
    }

} // Rc::Cli