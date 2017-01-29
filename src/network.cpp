/**
 * @file network.cpp
 * @author Brandon Miller
 * @brief IP connection logic for network-based fuzzing
 */
 
#include "../include/network.hpp"

namespace Networking
{
    /// constructor for IPConnection object
    IPConnection::IPConnection(std::string rhost, uint16_t port, std::string conn_type, int time_out_secs)
    {
        this->good = true;
        if (conn_type == "tcp")
        {
            this->conn_type = CONNTYPE::TCP;
            IPConnection::init_tcp(rhost, port, time_out_secs);
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

    /// setup TCP socket
    void IPConnection::init_tcp(std::string rhost, uint16_t port, int time_out_secs)
    {
        this->sock = socket(AF_INET , SOCK_STREAM , 0);
        if (sock < 0)
        {
            this->good = false;
            return;
        }

        this->saddr.sin_family = AF_INET;
        this->saddr.sin_addr.s_addr = inet_addr(rhost.c_str());
        this->saddr.sin_port = htons(port);
        this->is_connected = false;

        this->timeout.tv_sec = time_out_secs;
        this->timeout.tv_usec = 0;

        // set send and receive timeouts
        setsockopt(this->sock, SOL_SOCKET, SO_RCVTIMEO, (char *)&timeout, sizeof(timeout));
        setsockopt(this->sock, SOL_SOCKET, SO_SNDTIMEO, (char *)&timeout, sizeof(timeout));
    }

    /// send data over UDP socket
    STATUS IPConnection::send_data(uint8_t *buffer, size_t n)
    {
        // send over UDP
        if (conn_type == CONNTYPE::UDP)
        {
            if (sendto(this->sock, buffer, n, 0, (struct sockaddr *) &this->saddr, sizeof(this->saddr)) < 0)
            {
                ERRPRINT("failed to send data over UDP");
                return STATUS::ERROR;
            }

            return STATUS::GOOD;
        }

        // send over TCP
        if (conn_type == CONNTYPE::TCP)
        {
            // connect if not connected
            if (!is_connected)
            {
                if (IPConnection::tcp_connect() == STATUS::ERROR)
                {
                    ERRPRINT("failed to connect to target service");
                    return STATUS::ERROR;
                }
            }

            if (send(this->sock , buffer, n, 0) < 0)
            {
                ERRPRINT("failed to send data - check for crash");
                return STATUS::ERROR;
            }

            return STATUS::GOOD;
        }

        return STATUS::ERROR;
    }

    /// connect to target process over TCP
    STATUS IPConnection::tcp_connect()
    {
        if (connect(this->sock, (struct sockaddr *)&this->saddr, sizeof(this->saddr)) < 0)
            return STATUS::ERROR;

        this->is_connected = true;
        return STATUS::GOOD;
    }

    /// recv data over UDP socket
    ssize_t IPConnection::recv_data(uint8_t *buffer, size_t bufsize)
    {
        ssize_t n = -1;
        socklen_t addrlen = sizeof(saddr);

        if (conn_type == CONNTYPE::UDP)
            n = recvfrom(this->sock, buffer, bufsize, 0, (struct sockaddr *) &this->saddr, &addrlen);

        if (conn_type == CONNTYPE::TCP)
            n = recv(this->sock, buffer, bufsize, 0);

        return n;
    }

    /// close socket or serial connection
    void IPConnection::close_conn()
    {
        close(this->sock);
    }
} 