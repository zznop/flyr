/**
 * @file base.hpp
 * @author Brandon Miller
 * @brief base header file
 */

#ifndef _BASE_HPP
#define _BASE_HPP

#include <iostream>
#include <memory>
#include <string>
#include <vector>

#define RED    "\033[0;31m"
#define BOLD   "\033[1m"
#define ENDC   "\033[0m"
#define DEBUG

using std::cout;
using std::endl;
using std::string;
using std::unique_ptr;
using std::shared_ptr;
using std::make_shared;

enum STATUS
{
    UNK_ERROR,
    ERROR,
    GOOD
};

#ifdef DEBUG
#define DBGPRINT(x) do { std::cout << BOLD << "[DEBUG] " << ENDC << x << std::endl; } while (false)
#else
#define DBGPRINT(x) do { continue; } while (false);
#endif

#define ERRPRINT(x) do { std::cerr << RED << "[ERROR] " << ENDC << x << std::endl; } while (false)

#endif