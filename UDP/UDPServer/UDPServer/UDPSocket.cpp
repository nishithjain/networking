#include "UDPSocket.h"

UdpSocket::UdpSocket(const unsigned short port)
{
   /*
    *    | Step | Purpose                        |
    *    | ---- | ------------------------------ |
    *    | 1    | Initialize Winsock             |
    *    | 2    | Create a UDP socket            |
    *    | 3    | Set up the IP address and port |
    *    | 4    | Bind the socket                |
    *    | 5    | Handle errors if any           |
    */


    /* 1. Initialize Winsock
     * Before using any socket functions on Windows, we must initialize Winsock.
     * WSAStartup() sets up internal data structures needed by Winsock.
     * MAKEWORD(2,2) requests Winsock version 2.2, the most common one.
     * wsaData receives info about the implementation.
     */
   WSADATA wsa_data;
   WSAStartup(MAKEWORD(2, 2), &wsa_data);

   /* 2. Create a UDP socket
    *    AF_INET: We're using IPv4
    *    SOCK_DGRAM: Means UDP
    *    IPPROTO_UDP: Specifies the UDP protocol
    *    If this fails, socket() returns INVALID_SOCKET.
    */
   server_socket_ = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
   if (server_socket_ == INVALID_SOCKET) {
      std::cerr << "Socket creation failed\n";
      shutdown();
   }

   /* 3. Set up the IP address and port
    * This block sets up the server's IP address and port.
    *    sin_family = AF_INET: Specifies IPv4
    *    sin_addr.s_addr = INADDR_ANY: Means accept messages on any local IP
    *                   (e.g., 127.0.0.1, local WiFi IP, etc.)
    *    sin_port = htons(port): Converts the port number to network byte order
    */
   server_address_.sin_family = AF_INET;
   server_address_.sin_addr.s_addr = INADDR_ANY;
   server_address_.sin_port = htons(port);

   /* 4. Bind the socket
    * bind() tells Windows to associate this socket with the local IP and port.
    */
   if (bind(server_socket_, reinterpret_cast<sockaddr*>(&server_address_), 
      sizeof(server_address_)) == SOCKET_ERROR)
   {
      /* 5. Handle errors if any.
       * If 'bind' fails (maybe the port is in use), SOCKET_ERROR is returned.
       */
      std::cerr << "Bind failed\n";
      closesocket(server_socket_);
      WSACleanup();
      shutdown();
   }
}

void UdpSocket::shutdown() const
{
   if (server_socket_ != INVALID_SOCKET) 
   {
      closesocket(server_socket_);
   }
   WSACleanup();
}

/*
 * Listens for incoming UDP packets
 * For each packet received, spawns a new thread
 * Prints the message with sender details
 */
void UdpSocket::ReceiveLoop() const
{
   while (true) {

      /*
       * client_address: Will hold the sender's IP and port
       * client_len: Size of client_address
       * buffer: Temporary storage for incoming message
       */
      sockaddr_in client_address;
      int client_len = sizeof(client_address);
      char buffer[1024] = { 0 };

      /*
       * This blocks (waits) until a UDP message is received.
       * recvfrom() fills buffer with the message
       * Fills client_address with the sender's IP and port
       * Returns the number of bytes received
       */
      const int bytes_received = recvfrom(server_socket_, buffer, sizeof(buffer) - 1, 0,
         reinterpret_cast<sockaddr*>(&client_address), &client_len);

      if (bytes_received > 0) 
      {
         // If we received something, we terminate the
         // string with \0 so we can treat it like a proper C-style string.
         buffer[bytes_received] = '\0';

         /*
          * Launches a new thread
          * [=] captures all variables by value (a copy)
          * So each thread has its own copy of buffer, client_address, etc.
          */
         std::thread handler([=]() {
            char client_ip[INET_ADDRSTRLEN];
            // inet_ntop() converts the sender's IP to a readable string
            inet_ntop(AF_INET, &client_address.sin_addr, client_ip, sizeof(client_ip));
            // ntohs() converts port number from network to host byte order
            const int client_port = ntohs(client_address.sin_port);

            std::cout << "[Thread " << std::this_thread::get_id()
               << "] Received from " << client_ip << ":" << client_port
               << " - " << buffer << '\n';

            // Optionally send a reply
            const std::string reply = "Ack: " + std::string(buffer);
            sendto(server_socket_, reply.c_str(), reply.length(), 0,
               (sockaddr*)&client_address, client_len);
            });

         handler.detach(); // let the thread run independently
      }
   }
}

UdpSocket::~UdpSocket()
{
   closesocket(server_socket_);
   WSACleanup();
}
