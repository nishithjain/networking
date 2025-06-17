#pragma once
#ifndef TCPSERVER_H
#define TCPSERVER_H

#include <winsock2.h>
#include <ws2tcpip.h>
#include <string>

#pragma comment(lib, "ws2_32.lib")

class TcpServer {
public:
   explicit TcpServer(int port);
   ~TcpServer();

   bool initialize();
   bool start();
   void HandleClient();

private:
   int port_;
   SOCKET server_socket_;
   SOCKET client_socket_;
   sockaddr_in server_address_;
   sockaddr_in client_address_;

   bool BindSocket();
   bool ListenSocket() const;
   SOCKET AcceptClient();
   void cleanup() const;
};

#endif  // TCPSERVER_H
