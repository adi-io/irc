#include "include/server.hpp"
#include <string>
#include <cstdlib>

int isPortValid(std::string str)
{
    int port = atoi(str.c_str());
    if (port < 1024 || port > 65535)
        return (0);
    return (1);
}

int main(int ac, char *av[])
{
    Server ser;
    if (ac != 3)
    {
        std::cout << "Usage: " << av[0] << " <port number> <password>" << std::endl;
        return (1);
    }
    std::cout << ".........SERVER HAS STARTED.........." << std::endl;
    try
    {
        signal(SIGINT, Server::SignalHandler);
        signal(SIGQUIT, Server::SignalHandler);
        if (!isPortValid(av[1]) || std::strlen(av[1]) > 20)
        {
            std::cout << "Invalid Port or Password. Please try again" << std::endl;
            return (1);
        }
        ser.init(atoi(av[1]), av[2]);
    } catch (const std::exception &e)
    {
        ser.close_fds();
        std::cerr << e.what() << std::endl;
    }
    std::cout << ".........SERVER HAS TERMINATED.........." << std::endl;
    return (0);
}
