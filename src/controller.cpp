/**
 * @file controller.cpp
 * @author Brandon Miller
 * @brief contains logic to control fuzzing runs
 */
 
#include "../include/controller.hpp"

namespace Controller
{
    /// DudleyController object constructor
    DudleyController::DudleyController(shared_ptr<Parser::DudleyParser> parser)
    {
        is_running = false;
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

        this->is_running = true;
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
        while (this->is_running && iter != this->messages.end())
        {
            iter++;
            if ((*iter)[0] == "d_string")
            {
                nb = Crafter::d_string(*iter, this->packet + tb, PKTMAX - tb);
                if (nb == 0)
                    ERRPRINT("packet overflow detected");
            }
            else if ((*iter)[0] == "d_clear")
            {
                Crafter::d_clear(this->packet, PKTMAX);
                tb = 0;
            }

            tb += nb;
        }
    }
    
    /// stop run
    STATUS DudleyController::stop()
    {
        if (is_running)
            is_running = false;
    }
    
    /// restart run
    STATUS DudleyController::restart()
    {
        if (is_running)
        {
            DudleyController::stop();
            DudleyController::start();
            return STATUS::GOOD;
        }

        DudleyController::start();
        return STATUS::GOOD;
    }

    void DudleyController::join()
    {
        this->is_running = false;
        if (this->packet != NULL)
            free(this->packet);

        this->ctl_thr->join();
    }
}
