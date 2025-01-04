#include "../include/client.hpp"
#include <atomic>
#include <iostream>
#include <sched.h>
#include <set>
#include <string>
#include <type_traits>
#include <unistd.h>

Client::Client()
{
    this->nickname = "";
	this->username = "";
	this->fd = -1;
	this->isOperator= false;
	this->registered = false;
	this->buffer = "";
	this->ipadd = "";
	this->logedin = false;
}

Client::~Client()
{
    std::cout << "Destructer called" << std::endl;
}

Client::Client(Client const &src)
{
    *this = src;
}

Client&  Client::operator=(Client const &src)
{
    this -> fd = src.fd;
    this -> isOperator = src.isOperator;
    this -> registered = src.registered;
    this -> nickname = src.nickname;
    this -> logedin = src.logedin;
    this -> username = src.username;
    this -> buffer = src.buffer;
    this -> ipadd = src.ipadd;
    this -> ChannelsInvite = src.ChannelsInvite;
}

int Client::GetFd()
{
    return (this -> fd);
}

bool    Client::getRegistered()
{
    return (this -> registered);
}

bool    Client::GetInviteChannel(std::string &str)
{
    for(int i = 0; i < this -> ChannelsInvite.size(); i++)
    {
        if (this -> ChannelsInvite[i] == str)
            return (true);
    }
    return (false);
}

std::string Client::GetNickName()
{
    return (this -> nickname);
}

bool    Client::GetLogedIn()
{
    return (this -> logedin);
}

std::string Client::GetUserName()
{
    return (this -> username);
}

std::string Client::getIpAdd()
{
    return (this -> ipadd);
}

std::string Client::getBuffer()
{
    return (this -> buffer);
}

std::string Client::getHostname()
{
    //return (//TODO);
}

void    Client::SetFd(int fd)
{
    this -> fd = fd;
}

void    Client::SetNickname(std::string &name)
{
    this -> nickname = name;
}

void    Client::SetUsername(std::string &user)
{
    this -> username = user;
}

void    Client::setBuffer(std::string buf)
{
    this -> buffer = buf;
}

void    Client::setRegistered(bool val)
{
    this -> registered = val;
}

void    Client::setIpAdd(std::string ipadd)
{
    this -> ipadd = ipadd;
}

void    Client::clearBuffer()
{
    buffer.clear();
}
