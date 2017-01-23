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
    enum CONNTYPE
    {
        TCP,
        UDP,
        SERIAL
    };

    class IPConnection
    {
        public:
            IPConnection(std::string rhost, uint16_t port, std::string conn_type);
            ~IPConnection();
            STATUS send_data(uint8_t *buffer, size_t n);
            ssize_t recv_data(uint8_t *buffer, size_t bufsize);
            bool is_good();
            void close_conn();


    private:
            void init_udp(std::string rhost, uint16_t port);
            CONNTYPE conn_type;
            int sock;
            struct sockaddr_in saddr;
            bool good;
    };
}

#endif