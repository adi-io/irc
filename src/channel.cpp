#include "../include/channel.hpp"
#include <cmath>
#include <cstddef>
#include <ctime>
#include <sstream>
#include <string>
#include <sys/socket.h>
#include <utility>
#include <iostream>

Channel::Channel()
{
   	invit_only = 0;
	topic = 0;
	key = 0;
	limit = 0 ;
	topic_restriction = 0;
	name = "";
	created_at = "";
	topic_name = "";
	char   chars[5] = {'i', 't', 'k', 'o', 'l'};
	for (size_t i = 0; i < 5; i++)
	   modes.push_back(std::make_pair(chars[i], false));
}

Channel::~Channel()
{
    // std::cout << "Destructor called" << std::endl;
}

Channel::Channel(Channel const &src)
{
    *this = src;
}

Channel&    Channel::operator=(Channel const &src)
{
    if (this != &src)
    {
        this -> invit_only = src.invit_only;
        this -> topic = src.topic;
        this -> key = src.key;
        this -> limit = src.limit;
        this -> topic_restriction = src.topic_restriction;
        this -> name = src.name;
        this -> time_creation = src.time_creation;
        this -> password = src.password;
        this -> created_at = src.created_at;
        this -> topic_name = src.topic_name;
        this -> clients = src.clients;
        this -> admins = src.admins;
        this -> modes = src.modes;
    }
    return (*this);
}

void    Channel::SetInvitOnly(int invit_only)
{
    this -> invit_only = invit_only;
}

void    Channel::SetTopic(int topic)
{
    this -> topic = topic;
}

void    Channel::SetKey(int key)
{
    this -> key = key;
}

void    Channel::SetLimit(int limit)
{
    this -> limit = limit;
}

void    Channel::SetTopicName(std::string topic_name)
{
    this -> topic_name = topic_name;
}

void    Channel::SetPassword(std::string password)
{
    this -> password = password;
}

void    Channel::SetName(std::string name)
{
    this -> name = name;
}

void    Channel::SetTime(std::string time)
{
    this -> time_creation = time;
}

void    Channel::SetTopicRestriction(bool value)
{
    this -> topic_restriction = value;
}

void    Channel::SetModeAtindex(size_t index, bool mode)
{
    modes[index].second = mode;
}

void Channel::set_createiontime()
{
    std::time_t _time = std::time(NULL);
    std::ostringstream str;
    str << _time;
    this -> created_at = std::string(str.str());
}

int Channel::GetInvitOnly()
{
    return (this ->invit_only);
}

int Channel::GetTopic()
{
    return (this -> topic);
}

int Channel::GetKey()
{
    return (this -> key);
}

int Channel::GetLimit()
{
    return (this -> limit);
}

int Channel::GetClientsNumber()
{
    size_t len = this -> clients.size();
    return (len);
}

bool    Channel::Gettopic_restriction() const
{
    return (this -> topic_restriction);
}

bool    Channel::GetModeAtIndex(size_t index)
{
    return (modes[index].second);
}


bool    Channel::clientInChannel(std::string &nick)
{
    for(size_t i = 0; i < clients.size(); i++)
    {
        if (this -> clients[i].GetNickName() == nick)
            return (true);
    }
    return (false);
}

std::string Channel::GetTopicName()
{
    return(this -> topic_name);
}

std::string Channel::GetPassword()
{
    return (this -> password);
}

std::string Channel::GetName()
{
    return (this -> name);
}

std::string Channel::GetTime()
{
    return (this -> time_creation);
}

std::string Channel::get_creationtime()
{
    return (this -> created_at);
}

std::string Channel::getModes()
{
    std::string str;
    for (size_t i = 0; i < modes.size(); i++)
    {
        if (modes[i].first != 'o' && modes[i].second)
            str.push_back(modes[i].first);
    }
    if (!str.empty())
    {
        str.insert(str.begin(), '+');
    }
    return (str);
}

std::string Channel::clientChannel_list()
{
    std::string list;
    for (size_t i = 0; i < this -> admins.size(); i++)
    {
        list += "@" + this-> admins[i].GetNickName();
        if ((i + 1) < admins.size())
            list += " ";
    }
    if (clients.size())
        list += " ";
    for (size_t i = 0; i < this -> clients.size(); i++)
    {
        list += this-> clients[i].GetNickName();
        if ((i + 1) < clients.size())
            list += " ";
    }
    return (list);
}

Client*  Channel::get_client(int fd)
{
    for (size_t i = 0; i < this-> clients.size(); i++)
    {
        if (this -> clients[i].GetFd() == fd)
            return (&this -> clients[i]);
    }
    return (NULL);
}

Client*  Channel::get_admin(int fd)
{
    for (size_t i = 0; i < this-> admins.size(); i++)
    {
        if (this -> admins[i].GetFd() == fd)
            return (&this -> admins[i]);
    }
    return (NULL);
}

Client*  Channel::GetClientInChannel(std::string name)
{
    for (size_t i = 0; i < this-> clients.size(); i++)
    {
        if (this -> clients[i].GetNickName() == name)
            return (&this -> clients[i]);
    }
    for (size_t i = 0; i < this-> admins.size(); i++)
    {
        if (this -> admins[i].GetNickName() == name)
            return (&this -> admins[i]);
    }
    return (NULL);
}


void    Channel::add_client(Client newClient)
{
    this -> clients.push_back(newClient);
}

void    Channel::add_admin(Client newadmin)
{
    this -> admins.push_back(newadmin);
}

void    Channel::remove_client(int fd)
{
    for (size_t i = 0; i < this-> clients.size(); i++)
    {
        if (this -> clients[i].GetFd() == fd)
            this -> clients.erase(i + clients.begin());
    }
}

void    Channel::remove_admin(int fd)
{
    for (size_t i = 0; i < this-> admins.size(); i++)
    {
        if (this -> admins[i].GetFd() == fd)
            this -> admins.erase(i + clients.begin());
    }
}

bool    Channel::change_clientToAdmin(std::string& nick)
{
    size_t i = 0;
    for (; i < this -> clients.size(); i++)
    {
        if (this -> clients[i].GetNickName() == nick)
            break ;
    }
    if (i < this -> clients.size())
    {
        this -> admins.push_back(this -> clients[i]);
        this -> clients.erase(this -> clients.begin() + 1);
        return (true);
    }
    return (false);
}

bool    Channel::change_adminToClient(std::string& nick)
{
    size_t i = 0;
    for (; i < this -> admins.size(); i++)
    {
        if (this -> admins[i].GetNickName() == nick)
            break ;
    }
    if (i < this -> admins.size())
    {
        this -> clients.push_back(this -> admins[i]);
        this -> admins.erase(this -> admins.begin() + 1);
        return (true);
    }
    return (false);
}

void    Channel::sendTo_all(std::string rpl1)
{
    for (size_t i = 0; i < this -> admins.size(); i++)
    {
        if (send(this -> admins[i].GetFd(), rpl1.c_str(), rpl1.size(), 0) == -1)
                std::cerr << "send() to all failed";
    }
    for (size_t i = 0; i < this -> clients.size(); i++)
    {
        if (send(this -> clients[i].GetFd(), rpl1.c_str(), rpl1.size(), 0) == -1)
                std::cerr << "send() to all failed";
    }
}

void    Channel::sendTo_all(std::string rpl1, int fd)
{
    for (size_t i = 0; i < this -> admins.size(); i++)
    {
        if (this -> admins[i].GetFd() != fd)
        {
            if (send(this -> admins[i].GetFd(), rpl1.c_str(), rpl1.size(), 0) == -1)
                std::cerr << "send() to all failed";
        }
    }
    for (size_t i = 0; i < this -> clients.size(); i++)
    {
        if (this -> clients[i].GetFd() != fd)
        {
            if (send(this -> clients[i].GetFd(), rpl1.c_str(), rpl1.size(), 0) == -1)
                std::cerr << "send() to all failed";
        }
    }
}
