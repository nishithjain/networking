#include "TCPServer.h"
#include <iostream>

constexpr auto kBufferSize = 1024;

TcpServer::TcpServer(const int port) : port_(port),
server_socket_(INVALID_SOCKET), client_socket_(INVALID_SOCKET)
{
   ZeroMemory(&server_address_, sizeof(server_address_));
   ZeroMemory(&client_address_, sizeof(client_address_));
}

TcpServer::~TcpServer() {
   cleanup();
}

bool TcpServer::initialize() {
   WSADATA wsa_data;
   const int result = WSAStartup(MAKEWORD(2, 2), &wsa_data);
   if (result != 0) {
      std::cerr << "WSAStartup failed: " << result << '\n';
      return false;
   }

   server_socket_ = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
   if (server_socket_ == INVALID_SOCKET) {
      std::cerr << "Socket creation failed!" << '\n';
      return false;
   }
   return true;
}

bool TcpServer::BindSocket() {
   server_address_.sin_family = AF_INET;
   server_address_.sin_port = htons(port_);
   server_address_.sin_addr.s_addr = INADDR_ANY;

   if (bind(server_socket_, (sockaddr*)&server_address_, sizeof(server_address_)) == SOCKET_ERROR) {
      std::cerr << "Bind failed! Error: " << WSAGetLastError() << '\n';
      return false;
   }
   return true;
}

bool TcpServer::ListenSocket() const
{
   if (listen(server_socket_, SOMAXCONN) == SOCKET_ERROR) {
      std::cerr << "Listen failed! Error: " << WSAGetLastError() << '\n';
      return false;
   }
   return true;
}

SOCKET TcpServer::AcceptClient() {
   int client_address_size = sizeof(client_address_);
   const SOCKET client_socket = accept(server_socket_, (struct sockaddr*)&client_address_, &client_address_size);
   if (client_socket == INVALID_SOCKET) {
      std::cerr << "Accept failed! Error: " << WSAGetLastError() << '\n';
   }
   return client_socket;
}

bool TcpServer::start() {
   if (!BindSocket()) return false;
   if (!ListenSocket()) return false;

   std::cout << "Server listening on port " << port_ << "...\n";
   client_socket_ = AcceptClient();
   if (client_socket_ == INVALID_SOCKET) 
      return false;

   std::cout << "Client connected.\n";
   return true;
}

void TcpServer::HandleClient() {
   char buffer[kBufferSize];
   int received_size;

   while ((received_size = recv(client_socket_, buffer, kBufferSize - 1, 0)) > 0) {
      buffer[received_size] = '\0';
      std::cout << "Received from client: " << buffer << '\n';

      std::string response = "Hello Client! We received: ";
      response += buffer;

      send(client_socket_, response.c_str(), static_cast<int>(response.length()), 0);
   }

   if (received_size == 0) {
      std::cout << "Client disconnected.\n";
   }
   else {
      std::cerr << "Receive failed. Error: " << WSAGetLastError() << '\n';
   }

   closesocket(client_socket_);
   client_socket_ = INVALID_SOCKET;
}

void TcpServer::cleanup() const
{
   if (client_socket_ != INVALID_SOCKET) {
      closesocket(client_socket_);
   }
   if (server_socket_ != INVALID_SOCKET) {
      closesocket(server_socket_);
   }
   WSACleanup();
}
