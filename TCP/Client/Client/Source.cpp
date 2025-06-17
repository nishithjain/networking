#include "ClientSocket.h"

int main() {
   if (!InitializeWinsock()) {
      return 1;
   }

   const SOCKET client_socket = CreateSocket();
   if (client_socket == INVALID_SOCKET) {
      WSACleanup();
      return 1;
   }

   if (!ConnectToServer(client_socket, server_ip, kPort)) {
      Cleanup(client_socket);
      return 1;
   }

   const std::string message = "Hello, Server!";
   if (!SendMessage(client_socket, message)) {
      Cleanup(client_socket);
      return 1;
   }

   ReceiveResponse(client_socket);
   Cleanup(client_socket);

   return 0;
}
