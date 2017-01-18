/**
 * @file craft.hpp
 * @author Brandon Miller
 * @brief header file for craft.cpp
 */

#ifndef _CRAFT_HPP
#define _CRAFT_HPP

#include "base.hpp"
#include <cstring>

#define PKTMAX 65535

namespace Crafter
{
    std::size_t d_string(std::vector<std::string> params, uint8_t *buffer, std::size_t max);
    std::size_t d_string_repeat(std::vector<std::string> params, uint8_t *buffer, std::size_t max);
    void d_clear(uint8_t *buffer, std::size_t n);
    void d_hexdump(uint8_t *buffer, size_t n);
}

#endif
