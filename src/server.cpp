#include "../include/server.hpp"
#include <cstddef>
#include <cstring>
#include <iostream>
#include <sstream>
#include <string>
#include <sys/poll.h>
#include <vector>


Server::Server()
{
    this -> ServerSocketFd = -1;
}

Server::~Server()
{
    std::cout << "Destructor called" << std::endl;
}

Server::Server(Server const &src)
{
    *this = src;
}

Server&  Server::operator=(Server const &src)
{
    if (this != &src)
    {
        this -> Port = src.Port;
        this -> ServerSocketFd = src.ServerSocketFd;
        this -> signal = src.signal;
        this -> password = src.password;
        this -> clients = src.clients;
        this -> channels = src.channels;
        this -> fds = src.fds;
    }
    return (*this);
}

int Server::GetFd()
{
    return (this -> ServerSocketFd);
}

int Server::GetPort()
{
    return (this -> Port);
}

std::string  Server::GetPassword()
{
    return (this -> password);

}

Client  *Server::GetClient(int fd)
{
    if (fd < 3)
        return (NULL);
    for (size_t i = 0; i < this -> clients.size(); i++)
    {
        if (this -> clients[i].GetFd() == fd)
            return (&this -> clients[i]);
    }
    return (NULL);
}

Client  *Server::GetClientNick(std::string name)
{
    if (name.empty())
        return (NULL);
    for (size_t i = 0; i < this -> clients.size(); i++)
    {
        if (this -> clients[i].GetNickName() == name)
            return (&this -> clients[i]);
    }
    return (NULL);
}

Channel *Server::GetChannel(std::string name)
{
    for (size_t i = 0; i < this -> channels.size(); i++)
    {
        if (this -> channels[i].GetName() == name)
            return (&this -> channels[i]);
    }
    return (NULL);
}

void    Server::SetFd(int fd)
{
    this -> ServerSocketFd = fd;
}

void    Server::SetPort(int num)
{
    this -> Port = num;
}

void    Server::SetPassword(std::string str)
{
    this -> password = str;
}

void Client::setLogedin(bool value)
{
    this->logedin = value;
}

void    Server::AddClient(Client client)
{
    this -> clients.push_back(client);
}

void    Server::AddChannel(Channel channel)
{
    this -> channels.push_back(channel);
}

void    Server::AddFds(pollfd fd)
{
    this -> fds.push_back(fd);
}

void    Server::RemoveClient(int fd)
{
    for (size_t i = 0; i < this -> clients.size(); i++)
    {
        if (this -> clients[i].GetFd() == fd)
        {
            this -> clients.erase(this -> clients.begin() + i);
            return ;
        }
    }
}

void    Server::RemoveChannel(std::string name)
{
    for (size_t i = 0; i < this -> channels.size(); i++)
    {
        if (this -> channels[i].GetName() == name)
        {
            this -> channels.erase(this -> channels.begin() + i);
            return ;
        }
    }
}

void    Server::RmChannels(int fd)
{
    for (size_t i = 0; i < this -> channels.size(); i++)
    {
        int flag = 0;
        if (channels[i].get_client(fd))
        {
            channels[i].remove_client(fd);
            flag = 1;
        }
        if (channels[i].get_admin(fd))
        {
            channels[i].remove_admin(fd);
            flag = 1;
        }
        if (channels[i].GetClientsNumber() == 0)
        {
            channels.erase(channels.begin() + i);
            i--;
            continue;
        }
        if (flag)
        {
            std::string mesg = ":" + GetClient(fd) -> GetNickName() + ":~" + GetClient(fd) -> GetUserName() + "@localhost QUIT Quit\r\n";
            channels[i].sendTo_all(mesg);
        }
    }
}

void    Server::RemoveFds(int fd)
{
    for (size_t i = 0; i < this -> fds.size(); i++)
    {
        if (this -> fds[i].fd == fd)
        {
            this -> fds.erase(this -> fds.begin() + i);
            return ;
        }
    }
}

void    Server::senderror(int code, std::string clientname, int fd, std::string msg)
{
    std::stringstream ss;
    ss << ":localhost " << code << " " << clientname << msg;
    std::string str = ss.str();
    if (send(fd, str.c_str(), str.size(), 0) == -1)
        std::cerr << "send() failed" << std::endl;
}

void    Server::senderror(int code, std::string clientname, std::string channelname, int fd, std::string msg)
{
    std::stringstream ss;
    ss << ":localhost " << code << " " << clientname << " " << channelname << msg;
    std::string str = ss.str();
    if (send(fd, str.c_str(), str.size(), 0) == -1)
        std::cerr << "send() failed" << std::endl;
}


void    sendResponse(std::string response, int fd)
{
    if (send(fd, response.c_str(), response.size(), 0) == -1)
        std::cerr << "Response send() failed" << std::endl;
}

void Server::parse_exec_cmd(std::string &cmd, int fd)
{
    if (cmd.empty())
        return ;
    std::vector<std::string> splited_cmd = split_cmd(cmd);
    size_t found = cmd.find_first_not_of(" \t\v");
    if (found != std::string::npos)
        cmd = cmd.substr(found);
    if (splited_cmd.size() && (splited_cmd[0] == "BONG" || splited_cmd[0] == "bong"))
        return;
    if (splited_cmd.size() && (splited_cmd[0] == "PASS" || splited_cmd[0] == "pass"))
    client_authen(fd, cmd);
    else if (splited_cmd.size() && (splited_cmd[0] == "NICK" || splited_cmd[0] == "nick"))
    set_nickname(cmd,fd);
    else if(splited_cmd.size() && (splited_cmd[0] == "USER" || splited_cmd[0] == "user"))
    set_username(cmd, fd);

    else if(notregistered(fd))
    {
        if (splited_cmd.size() && (splited_cmd[0] == "KICK" || splited_cmd[0] == "kick"))
            std::cout << "command -> KICK" << std::endl;
        else if (splited_cmd.size() && (splited_cmd[0] == "JOIN" || splited_cmd[0] == "join"))
            JOIN(fd, cmd);
		else if (splited_cmd.size() && (splited_cmd[0] == "TOPIC" || splited_cmd[0] == "topic"))
		    TOPIC(fd, cmd);
		else if (splited_cmd.size() && (splited_cmd[0] == "MODE" || splited_cmd[0] == "mode"))
		    std::cout << "command -> MODE" << std::endl;
		else if (splited_cmd.size() && (splited_cmd[0] == "PART" || splited_cmd[0] == "part"))
		    std::cout << "command -> PART" << std::endl;

		else if (splited_cmd.size() && (splited_cmd[0] == "PRIVMSG" || splited_cmd[0] == "privmsg"))
			PRIVMSG(fd, cmd);

		else if (splited_cmd.size())
			sendResponse(ERR_CMDNOTFOUND(GetClient(fd)->GetNickName(),splited_cmd[0]),fd);

	}

    else if (!notregistered(fd))
        sendResponse(ERR_NOTREGISTERED(std::string("*")),fd);
}

void Server::accept_new_client()
{
    Client cli;
    memset(&cliadd, 0, sizeof(cliadd));
    socklen_t len = sizeof(cliadd);

    int intcofd = accept(ServerSocketFd, (sockaddr *)&cliadd, &len);
    if (intcofd == -1)
        {
            std::cout << "accept() failed" << std::endl;
            return ;
        }
    if (fcntl(intcofd, F_SETFD, O_NONBLOCK) == -1)
    {
        std::cout << "fcntl() failed" << std::endl;
        return ;
    }
    new_cli.fd = intcofd;
    new_cli.events = POLLIN;
    new_cli.revents = 0;

    cli.SetFd(intcofd);
    cli.setIpAdd((inet_ntoa((cliadd.sin_addr))));
    clients.push_back(cli);
    fds.push_back(new_cli);

    std::cout << GRE << "Client <" << intcofd << " Connected" << std::endl;
}

void    Server::reciveNewData(int fd)
{
    char buff[1024];
    memset(buff, 0, sizeof(buff));
    Client *cli = GetClient(fd);

    ssize_t bytes = recv(fd, buff, sizeof(buff) - 1 , 0);

    if (bytes <= 0)
    {
        std::cout << RED << "Client " << fd << " disconnected" << std::endl;
        RmChannels(fd);
        RemoveClient(fd);
        RemoveFds(fd);
        close(fd);
    }
    else
    {
        cli -> setBuffer(buff);
        if (cli -> getBuffer().find_first_of("\r\n") == std::string::npos)
            return ;

        std::vector<std::string> cmd =  split_recivedBuffer(cli -> getBuffer());
        for (size_t i = 0; i < cmd.size(); i++)
            parse_exec_cmd(cmd[i], fd);
        if (GetClient(fd))
            GetClient(fd) -> clearBuffer();
    }
}


std::vector<std::string>    Server::split_recivedBuffer(std::string str)
{
    std::vector<std::string> vec;
    std::istringstream ss(str);
    std::string snew;

    while (std::getline(ss, snew))
    {
        size_t pos = snew.find_first_of("\r\n");
        if (pos != std::string::npos)
            snew = snew.substr(0, pos);
        vec.push_back(snew);
    }
    return (vec);
}

std::vector<std::string>    Server::split_cmd(std::string &str)
{
    std::vector<std::string> vec;
    std::istringstream ss(str);
    std::string token;
    while (ss >> token)
    {
        vec.push_back(token);
        token.clear();
    }
    return (vec);
}

bool    Server::signal = false;

void    Server::SignalHandler(int sig)
{
    (void)sig;
    std::cout << "Signal Recived" << std::endl;
    Server::signal = true;

}

void    Server::close_fds()
{
    for(size_t i = 0; i < clients.size(); i ++)
        {
            std::cout << RED << "Client <" << clients[i].GetFd() << "> Disconnected" << std::endl;
            close(clients[i].GetFd());
        }
        if (ServerSocketFd != -1)
        {
            std::cout << "Server " << ServerSocketFd << " Disconnected" << std::endl;
            close(ServerSocketFd);
        }
}

void    Server::set_sever_socket()
{
    add.sin_family = AF_INET;
    add.sin_port = htons(this -> Port);
    add.sin_addr.s_addr = INADDR_ANY;

    ServerSocketFd = socket(AF_INET, SOCK_STREAM, 0);
    if (ServerSocketFd == -1)
        throw(std::runtime_error("Socket creation failed"));

    int en = 1;
    if (setsockopt(ServerSocketFd, SOL_SOCKET, SO_REUSEADDR, &en, sizeof(en)) == -1)
        throw(std::runtime_error("Failed to set the SO_REUSERADDR on the socket"));
    if (fcntl(ServerSocketFd, F_SETFL, O_NONBLOCK) == -1 )
        throw(std::runtime_error("Failed to set O_NONBLOCK option on the socket"));
    if (bind(ServerSocketFd, (struct sockaddr *)&add, sizeof(add)) == -1)
        throw(std::runtime_error("Failed to bind the sockets"));
    if (listen(ServerSocketFd, SOMAXCONN) == -1)
        throw(std::runtime_error("Socket listen() failed"));

    new_cli.fd = ServerSocketFd;
    new_cli.events = POLLIN;
    new_cli.revents = 0;
    fds.push_back(new_cli);
}

void    Server::init(int port, std::string pass)
{
    this -> Port = port;
    this -> password = pass;
    this -> set_sever_socket();

    std::cout << GRE << "Server " << this -> ServerSocketFd << " Connected" << WHI << std::endl;
    std::cout << "Waiting for server connection" << std::endl;

    while(Server::signal == false)
    {
        if ((poll(&fds[0], fds.size(), -1) == -1) && Server::signal == false)
            throw (std::runtime_error("poll() failed"));
        for (size_t i = 0; i < fds.size(); i++)
            {
                if (fds[i].revents && POLLIN)
                {
                    if (fds[i].fd == this -> ServerSocketFd)
                        this -> accept_new_client();
                    else
                        this -> reciveNewData(fds[i].fd);

                }
            }
    }
    this -> close_fds();
}

void Server::sendResponse(std::string response, int fd)
{
    if (send(fd, response.c_str(), response.size(), 0) == -1)
        std::cerr << "Response send() failed" << std::endl;
}

bool Server::notregistered(int fd)
{
    Client* client = GetClient(fd);
    if (!client || !client->getRegistered())
        return false;
    return true;
}
