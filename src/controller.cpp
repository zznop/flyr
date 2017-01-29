/**
 * @file controller.cpp
 * @author Brandon Miller
 * @brief contains logic to control fuzzing runs
 */
 
#include "../include/controller.hpp"

namespace Controller
{
    /// DudleyController object constructor
    DudleyController::DudleyController(shared_ptr<Parser::DudleyParser> parser,
                                       shared_ptr<Networking::IPConnection> conn)
    {
        running = false;
        this->conn = conn;
        this->parser = parser;
    }
    
    /// DudleyController object deconstructor
    DudleyController::~DudleyController()
    {
        //intentionally left blank
    }
    
    /// start run
    STATUS DudleyController::start()
    {
        if (this->parser->parse(this->messages) == STATUS::ERROR)
            return STATUS::ERROR;

        this->running = true;
        this->packet = (uint8_t *)calloc(PKTMAX, sizeof(uint8_t));
        this->ctl_thr = make_shared<std::thread>(&DudleyController::run, this);
        return STATUS::GOOD;
    }

    /// run DudleyController thread
    void DudleyController::run()
    {
        size_t tb = 0;
        size_t nb;
        std::vector<std::vector<std::string>>::iterator iter;
        iter = this->messages.begin();
        while (this->running && iter != this->messages.end())
        {
            if ((*iter)[0] == "d_string")
            {
                nb = Crafter::d_string(*iter, this->packet + tb, PKTMAX - tb);
                if (nb == 0)
                    ERRPRINT("packet overflow detected");
                tb += nb;
            }
            else if ((*iter)[0] == "d_string_repeat")
            {
                nb = Crafter::d_string_repeat(*iter, this->packet + tb, PKTMAX - tb);
                if (nb == 0)
                    ERRPRINT("packet overflow detected");
                tb += nb;

            }
            else if ((*iter)[0] == "d_clear")
            {
                Crafter::d_clear(this->packet, PKTMAX);
                tb = 0;
            }
            else if ((*iter)[0] == "d_hexdump")
            {
                Crafter::d_hexdump(this->packet, tb);
            }
            else if ((*iter)[0] == "d_send")
            {
                if (this->conn->send_data(this->packet, tb) != STATUS::GOOD)
                    break;
            }
            else if ((*iter)[0] == "d_binary")
            {
                nb = Crafter::d_binary(*iter, this->packet + tb, PKTMAX - tb);
                if (nb == 0)
                    ERRPRINT("packet overflow detected");
                tb += nb;
            }

            iter++;
        }

        this->running = false;
    }
    
    /// stop run
    void DudleyController::stop()
    {
        if (this->running)
            this->running = false;
    }
    
    /// restart run
    STATUS DudleyController::restart()
    {
        if (this->running)
        {
            DudleyController::stop();
            DudleyController::start();
            return STATUS::GOOD;
        }

        DudleyController::start();
        return STATUS::GOOD;
    }

    /// return information to the main thread on whether or not the fuzzer is still running
    bool DudleyController::is_running()
    {
        return this->running;
    }

    /// join thread and free the packet buffer
    void DudleyController::join()
    {
        this->running = false;
        if (this->packet != NULL)
            free(this->packet);

        this->ctl_thr->join();
    }
}
