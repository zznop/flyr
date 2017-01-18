/**
 * @file main.cpp
 * @author Brandon Miller
 * @brief parse arguments and run application
 */
 
#include "../include/main.hpp"

static bool is_alive = true;

/// handle signal interrupts
static void signal_handler(int interrupt)
{
    is_alive = false;
}

/// display help menu
static void print_usage(void)
{
    std::cout << "Usage: dudley [options] -f [file]" << std::endl;
    std::cout << "-h         display this menu" << std::endl;
    std::cout << "-t         fuzz using TCP protocol" << std::endl;
    std::cout << "-u         fuzz using UDP protocol" << std::endl;
    std::cout << "-i IP      target service host address" << std::endl;
    std::cout << "-p PORT    target service port" << std::endl;
    std::cout << "-f FILE    path to Dudley fuzz file" << std::endl;
}
 
/// parse the command line arguments and run the application
int main (int argc, char **argv)
{
    signal(SIGINT, signal_handler);
    std::string host;
    std::string filepath;
    int opt, port;
    bool udp, tcp;
    
    if (argc < 2)
    {
        print_usage();
        return 1;
    }
    
    while ((opt = getopt(argc, argv, "htui:p:f:")) != -1)
    {
        switch (opt)
        {
            case 'h':
                print_usage();
                return 0;

            case 't':
                tcp = true;
                break;
                
            case 'u':
                udp = true;
                break;
                
            case 'i':
                host = optarg;
                break;
                
            case 'p':
                port = std::stoi(optarg);
                break;
                
            case 'f':
                filepath = optarg;
                break;
                
            default:
                return 1;
        }
    }
    
    if (tcp && udp)
    {
        ERRPRINT("select TCP or UDP, not both -- 't', 'u'");
        return 1;
    }
    
    if (!tcp && !udp)
    {
        ERRPRINT("select TCP or UDP -- 't', 'u'");
        return 1;
    }
    
    if (filepath.empty())
    {
        ERRPRINT("you must supply a path to your fuzz file -- 'f'");
        return 1;
    }
    
    shared_ptr<Parser::DudleyParser> parser =
        make_shared<Parser::DudleyParser>(filepath);
        
    if (!parser->is_open())
    {
        ERRPRINT("failed to open dudley file: " + filepath);
        return 1;
    }
    
    Controller::DudleyController controller(parser);
    if (controller.start() != STATUS::GOOD)
    {
        ERRPRINT("failed to start run");
        return 1;
    }

    DBGPRINT("starting fuzzing scenario");
    while (is_alive && controller.is_running())
    {
        //TODO add logic for interfacing with debugger attached to target pid
        sleep(1);
    }

    DBGPRINT("joining controller thread");
    controller.join();
        
    return 0;
}
