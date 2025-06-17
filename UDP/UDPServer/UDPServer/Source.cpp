#include "UDPSocket.h"

int main() {
   const UdpSocket server(8080);
   std::cout << "UDP Server listening on port 8080...\n";
   server.ReceiveLoop();  // never returns
   return 0;
}