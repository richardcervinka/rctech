#pragma once

#include "WinSock2.h"
#include "ipv4_address.h"

namespace Rc::Net
{
    class UdpSocket
    {
    public:
        using Port = uint16_t;

        UdpSocket(IPv4Address address, Port port);

        // void Open();
        // void Close();
        // void Receive();
        // void Send();
        // Poll();

    private:
        SOCKET socket {};

        IPv4Address address;
        Port port {};
    };

} // Rc::Net