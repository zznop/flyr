/**
 * @file craft.cpp
 * @author Brandon Miller
 * @brief craft packets from parsed dudley files
 */

#include "../include/craft.hpp"

namespace Crafter
{
    /// populate dudley test case with string
    std::size_t d_string(std::vector<std::string> params, uint8_t *buffer, std::size_t max)
    {
        std::size_t n;
        if (buffer == NULL || params[1].size() > max)
            return 0;

        std::memcpy(buffer, params[1].c_str(), params[1].size());
        return params[1].size();
    }

    /// zeroize the packet buffer
    void d_clear(uint8_t *buffer, std::size_t n)
    {
        if (buffer != NULL)
            std::memset(buffer, '\0', n);
    }
}
