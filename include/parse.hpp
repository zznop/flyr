/**
 * @file parse.hpp
 * @author Brandon Miller
 * @brief header file for parse.cpp
 */
 
#ifndef _PARSE_HPP
#define _PARSE_HPP

#include "base.hpp"
#include <fstream>
#include <regex>

namespace Parser
{
    class DudleyParser
    {
        public:
            DudleyParser(std::string filepath);
            ~DudleyParser();
            STATUS parse(std::vector<std::vector<std::string>> &msgs);
            bool is_open();
            
        private:
            void trim_whitespace(std::string &s);
            void trim_comments(std::string &s);
            STATUS parse_func(string &s, std::vector<std::vector<std::string>> &msgs);
            STATUS check_func(std::vector<std::string> &sv);
            
            bool is_string(std::string &s);
            bool good_quotes(std::string &s);
            bool is_integer(std::string &s);
            STATUS check_d_string(std::vector<std::string> &sv);
            STATUS check_d_string_repeat(std::vector<std::string> &sv);



            shared_ptr<std::ifstream> infile;
    };
}

#endif