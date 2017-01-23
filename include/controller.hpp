/**
 * @file controller.hpp
 * @author Brandon Miller
 * @brief header file for controller.cpp
 */
 
#ifndef _CONTROLLER_HPP
#define _CONTROLLER_HPP

#include <thread>
#include <map>
#include "parse.hpp"
#include "base.hpp"
#include "craft.hpp"
#include "network.hpp"

namespace Controller
{
    class DudleyController
    {
        public:
            DudleyController(shared_ptr<Parser::DudleyParser> parser,
                             shared_ptr<Networking::IPConnection> conn);
            ~DudleyController();
            STATUS start();
            STATUS stop();
            STATUS restart();
            bool is_running();
            void join();
        
        private:
            void run();
            bool running;
            shared_ptr<Parser::DudleyParser> parser;
            shared_ptr<Networking::IPConnection> conn;
            shared_ptr<std::thread> ctl_thr;
            std::vector<std::vector<std::string>> messages;
            uint8_t *packet;
    };
}

#endif
