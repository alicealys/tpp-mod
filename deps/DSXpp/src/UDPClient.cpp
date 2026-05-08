/*
 * Copyright (C) 2024 Thanasis Petsas <thanpetsas@gmail.com>
 * Licence: MIT Licence
 */

#include "UDPClient.h"
#include <winsock2.h>
#include <ws2tcpip.h>

#pragma comment(lib, "ws2_32.lib")
#define BUFLEN 512


namespace UDPClient
{
    SOCKET sock;
    sockaddr_in serverAddress;
    const int Success = 0;
    bool initialized = false;

    
    int init(std::string server, unsigned short port) {
        WSADATA wsData;

        if (server.empty())
            return ServerEmptyError;

        // Inititialize the Winsock lib
        int res = WSAStartup(MAKEWORD(2, 2), &wsData);
        if (res != NO_ERROR) {
            return WinSockStartupError;
        }

        // Create the client socket
        if ((sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == INVALID_SOCKET)
            return WinSockCreationError;

        // Set the target server
        memset(&serverAddress, 0, sizeof(sockaddr_in));
        serverAddress.sin_family = AF_INET;
        serverAddress.sin_port = htons(port);
        InetPton(AF_INET, server.c_str(), &serverAddress.sin_addr.s_addr);
        initialized = true;
        return Success;
    }

    int send(std::string data)
    {
        if (!initialized)
            return ClientNotInitializedError;
        if (data.empty())
            return DataEmptyError;
        if ( sendto(sock, data.c_str(),
                (int) data.size(), 0,
                (sockaddr *) &serverAddress,
                sizeof(serverAddress)) == SOCKET_ERROR ) {
            return WinSockSendError;
        }
        return Success;
    }

    int terminate(void)
    {
        // Reset the target server
        memset(&serverAddress, 0, sizeof(sockaddr_in));

        // Close the client socket
        if (closesocket(sock))
            return WinSockCreationError;

        // Teardown Winsock lib
        if (WSACleanup() == SOCKET_ERROR)
            return WinSockCleanupError;

        initialized = false;
        return Success;
    }
}
