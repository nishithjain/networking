#include "TCPServer.h"

int main() {
   TcpServer server(54000);

   if (!server.initialize()) return 1;
   if (!server.start()) return 1;

   server.HandleClient();
   return 0;
}
