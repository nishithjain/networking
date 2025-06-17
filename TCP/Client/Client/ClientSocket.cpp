#include "ClientSocket.h"

bool InitializeWinsock() {
   WSADATA wsa_data;
   if (const int result = WSAStartup(MAKEWORD(2, 2), &wsa_data); 
      result != 0) {
      std::cerr << "WSAStartup failed: " << result << '\n';
      return false;
   }
   return true;
}

// Creates and returns a TCP socket
SOCKET CreateSocket() {
   const SOCKET sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
   if (sock == INVALID_SOCKET) {
      std::cerr << "Socket creation failed!" << '\n';
      return INVALID_SOCKET;
   }
   return sock;
}

// Connects to the server
bool ConnectToServer(const SOCKET sock, const char* server_ip, const int port) {
   sockaddr_in server_address{};
   server_address.sin_family = AF_INET;
   server_address.sin_port = htons(port);
   inet_pton(AF_INET, server_ip, &server_address.sin_addr);


   if (connect(sock, (sockaddr*)&server_address, sizeof(server_address))
      == SOCKET_ERROR) {
      std::cerr << "Connect failed!" << '\n';
      return false;
   }
   return true;
}

// Sends a message to the server
bool SendMessage(const SOCKET sock, const std::string& message) {
   const int bytes_sent = send(sock, message.c_str(), 
      static_cast<int>(message.length()), 0);
   if (bytes_sent == SOCKET_ERROR) {
      std::cerr << "Send failed!" << '\n';
      return false;
   }
   return true;
}

// Receives a response from the server
bool ReceiveResponse(const SOCKET sock) {
   char buffer[kBufferSize];
   const int receive_size = recv(sock, buffer, sizeof(buffer) - 1, 0);
   if (receive_size > 0) {
      buffer[receive_size] = '\0';  // Null-terminate the received data
      std::cout << "Response from server: " << buffer << '\n';
      return true;
   }
   std::cerr << "Receive failed or connection closed." << '\n';
   return false;
}

// Cleanup
void Cleanup(const SOCKET sock) {
   closesocket(sock);
   WSACleanup();
}