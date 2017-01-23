/**
 * @file network.cpp
 * @author Brandon Miller
 * @brief IP connection logic for network-based fuzzing
 */
 
#include "../include/network.hpp"

namespace Networking
{
    /// constructor for IPConnection object
    IPConnection::IPConnection(std::string rhost, uint16_t port, std::string conn_type)
    {
        this->good = true;
        if (conn_type == "tcp")
        {
            this->conn_type = CONNTYPE::TCP;
        }
        else if (conn_type == "udp")
        {
            this->conn_type = CONNTYPE::UDP;
            IPConnection::init_udp(rhost, port);
        }
        else if (conn_type == "serial")
        {
            this->conn_type = CONNTYPE::SERIAL;
        }
        else
        {
            ERRPRINT("Unknown connection type");
            return;
        }
    }
    
    /// deconstructor for IPConnection
    IPConnection::~IPConnection()
    {
        //left blank intentionally
    }

    /// ensure everything is good with the connection
    bool IPConnection::is_good()
    {
        return this->good;
    }

    /// setup UDP socket
    void IPConnection::init_udp(std::string rhost, uint16_t port)
    {
        this->sock = socket(AF_INET, SOCK_DGRAM, 0);
        if (sock < 0)
        {
            this->good = false;
            return;
        }

        this->saddr.sin_family = AF_INET;
        this->saddr.sin_addr.s_addr = inet_addr(rhost.c_str());
        this->saddr.sin_port = htons(port);
    }


    /// send data over UDP socket
    STATUS IPConnection::send_data(uint8_t *buffer, size_t n)
    {
        if (sendto(this->sock, buffer, n, 0, (struct sockaddr *)&this->saddr, sizeof(this->saddr)) < 0)
            return STATUS::ERROR;

        return STATUS::GOOD;
    }

    /// recv data over UDP socket
    ssize_t IPConnection::recv_data(uint8_t *buffer, size_t bufsize)
    {
        ssize_t n;
        socklen_t addrlen = sizeof(saddr);
        n = recvfrom(this->sock, buffer, bufsize, 0, (struct sockaddr *)&this->saddr, &addrlen);
        return n;
    }

    /// close socket or serial connection
    void IPConnection::close_conn()
    {
        close(this->sock);
    }
} 