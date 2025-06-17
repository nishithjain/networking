#pragma once
#include <winsock2.h>
#include <ws2tcpip.h>
#include <iostream>
#include <thread>
#include <vector>
#include <string>

#pragma comment(lib, "Ws2_32.lib")
class UdpSocket
{
   SOCKET server_socket_;
   sockaddr_in server_address_;
public:
   explicit UdpSocket(unsigned short port);
   void shutdown() const;
   void ReceiveLoop() const;
   ~UdpSocket();
};

