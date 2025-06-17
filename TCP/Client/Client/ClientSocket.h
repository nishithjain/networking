#pragma once
/*
 * 1. Create Socket: The client creates a socket object.
 * 2. Connect: The client connects to the server using the server's port.
 *    The server's port is predefined or shared with the client.
 * 3. Send Data: The client sends requests to the server.
 * 4. Receive Response: The client receives responses from the server.
 * 5. End Connection: The client disconnects once the communication is complete.
 */

#include <winsock2.h>
#include <ws2tcpip.h>
#include <iostream>
#include <string>

#pragma comment(lib, "ws2_32.lib")  // Link with Winsock library

inline auto server_ip = "127.0.0.1";  // Localhost
inline constexpr int kPort = 54000;
inline constexpr int kBufferSize = 1024;

bool InitializeWinsock();
SOCKET CreateSocket();
bool ConnectToServer(SOCKET sock, const char* server_ip, int port);
bool SendMessage(SOCKET sock, const std::string& message);
bool ReceiveResponse(SOCKET sock);
void Cleanup(SOCKET sock);
