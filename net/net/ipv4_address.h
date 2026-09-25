#pragma once

#include <array>
#include <string>
#include <cstdint>
#include <cassert>
#include <ostream>

namespace Rc::IPv4
{
    class Address
    {
    public:
        // Default IP address 0.0.0.0
        Address() = default;

        // Construct IP address from the 32-bit value in format 0xaabbccdd
        explicit Address(uint32_t address) : address{address} {}

        // Construct IP address from octets: <a>.<b>.<c>.<d>
        Address(uint8_t a, uint8_t b, uint8_t c, uint8_t d)
            : address {
                (uint32_t{a} << 24) |
                (uint32_t{b} << 16) |
                (uint32_t{c} << 8) |
                (uint32_t{d} << 0)
            }
        {}

        explicit Address(std::string_view str);

        // Get separate octets.
        std::array<uint8_t, 4> Bytes() const
        {
            return
            {
                static_cast<uint8_t>(address >> 24),
                static_cast<uint8_t>(address >> 16),
                static_cast<uint8_t>(address >> 8),
                static_cast<uint8_t>(address >> 0)
            };
        }

        // To IPv4 string.
        std::string Str() const;

        // To 32-bit value.
        uint32_t Value() const
        {
            return address;
        }

        // 127.0.0.1
        static Address Localhost()
        {
            return {127, 0, 0, 1};
        }

    private:
        uint32_t address {0};
    };

    static inline bool operator==(Address const& lhs, Address const& rhs)
    {
        return lhs.Value() == rhs.Value();
    }

    static inline bool operator!=(Address const& lhs, Address const& rhs)
    {
        return lhs.Value() != rhs.Value();
    }

    static inline bool operator<(Address const& lhs, Address const rhs)
    {
        return lhs.Value() < rhs.Value();
    }

    static inline bool operator<=(Address const& lhs, Address const rhs)
    {
        return lhs.Value() <= rhs.Value();
    }

    static inline bool operator>(Address const& lhs, Address const rhs)
    {
        return lhs.Value() > rhs.Value();
    }

    static inline bool operator>=(Address const& lhs, Address const rhs)
    {
        return lhs.Value() >= rhs.Value();
    }

    static inline std::ostream& operator<<(std::ostream& os, Address const& ip)
    {
        return os << ip.Str();
    }

} // namespace Rc::IPv4
