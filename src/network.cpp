/**
 * @file network.cpp
 * @author Brandon Miller
 * @brief IP connection logic for network-based fuzzing
 */
 
#include "../include/network.hpp"

namespace Networking
{
    /// constructor for UDPConnection object
    UDPConnection::UDPConnection(std::string rhost, int port)
    {
        this->sock = socket(AF_INET, SOCK_DGRAM, 0);
        this->saddr.sin_family = AF_INET;
        this->saddr.sin_addr.s_addr = inet_addr(rhost.c_str());
        this->saddr.sin_port = htons(port);
    }
    
    /// deconstructor for UDPConnection object
    UDPConnection::~UDPConnection()
    {
        // intentionally left blank
    }
} 