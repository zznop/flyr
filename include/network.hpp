/**
 * @file network.hpp
 * @author Brandon Miller
 * @brief header file for network.cpp
 */
 
#ifndef _NETWORK_HPP
#define _NETWORK_HPP
 
#include "base.hpp"
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <unistd.h>

namespace Networking
{
    class UDPConnection
    {
        public:
            UDPConnection(std::string rhost, int port);
            ~UDPConnection();
    
        private:
            int sock;
            struct sockaddr_in saddr;
    };
}

#endif