/*
 * Copyright (C) 2024 Thanasis Petsas <thanpetsas@gmail.com>
 * Licence: MIT Licence
 */

#ifndef UDPCLIENT_H
#define UDPCLIENT_H

#include <string>

namespace UDPClient
{
    enum ErrorCode {
        ServerEmptyError = 1,
        DataEmptyError,
        WinSockStartupError,
        WinSockCreationError,
        WinSockSendError,
        WinSockCloseError,
        WinSockCleanupError,
        ClientNotInitializedError,
    };

    extern const int Success;
    
    /**
     * Initializes a UDP client for sending UDP packets
     * @param server The IP address of hostname of target UDP server
     * @param port   The port of the target UDP server
     * @return 0 if the client was successfully initialized,
     *         or an error code otherwise (see enum class ErrorCode)
     */
    int init(std::string server, unsigned short port);

    /*
     * Sends a UDP packet
     * @param data   The data to be sent in the form of a string
     * @return 0 if data were sent successfully,
     *         or an error code otherwise (see enum class ErrorCode)
     */
    int send(std::string data);

    /**
     * Terminates a UDP client
     * @return 0 if the client was successfully terminated,
     *         or an error code otherwise (see enum class ErrorCode)
     */
    int terminate(void);
}

#endif // UDPCLIENT_H
