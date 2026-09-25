#include "ipv4_address.h"
#include <format>
#include <stdexcept>

namespace Rc::IPv4
{
    static constexpr uint32_t ToDigit(char ch)
    {
        return static_cast<uint32_t>(ch) - static_cast<uint32_t>('0');
    }

    static constexpr bool IsDigit(char ch)
    {
        return ch >= '0' && ch <= '9';
    }

    Address::Address(std::string_view str)
    {
        std::array<uint32_t, 4> b = {};

        // Index of the parsing octet.
        uint32_t n = 0;

        // Number of digits in the current octet.
        uint32_t c = 0;

        char prev = 0;

        for (char const ch : str)
        {
            if (IsDigit(ch))
            {
                if (c == 1 && prev == '0')
                {
                    throw std::invalid_argument("IPv4 address: Leading 0!");
                }
                if (c >= 3)
                {
                    throw std::invalid_argument("IPv4 address: Out of range octet!");
                }

                b[n] = (b[n] * 10) + ToDigit(ch);

                if (b[n] > 255)
                {
                    throw std::invalid_argument("IPv4 address: Out of range octet!");
                }
                c++;
            }
            else if (ch == '.')
            {
                if (n >= 3)
                {
                    throw std::invalid_argument("IPv4 address: Illegal number of octets!");
                }
                if (c == 0)
                {
                    throw std::invalid_argument("IPv4 address: Empty octet!");
                }
                n++;
                c = 0;
            }
            else
            {
                throw std::invalid_argument(std::string("IPv4 address: Illegal '") + ch + "' character!");
            }

            prev = ch;
        }

        if (n != 3)
        {
            throw std::invalid_argument("IPv4 address: Illegal number of octets!");
        }
        if (c == 0)
        {
            throw std::invalid_argument("IPv4 address: Empty octet!");
        }

        address = (b[0] << 24) | (b[1] << 16) | (b[2] << 8) | (b[3] << 0);
    }

    std::string Address::Str() const
    {
        return std::format("{}.{}.{}.{}",
            (address >> 24) & 0xFF,
            (address >> 16) & 0xFF,
            (address >> 8)  & 0xFF,
            (address >> 0)  & 0xFF
        );
    }

} // Rc::IPv4
