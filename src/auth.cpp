#include "../include/server.hpp"
#include <cstddef>
#include <string>
#include <unistd.h>
#include <vector>

void    Server::client_authen(int fd, std::string pass)
{
    Client  *cli = GetClient(fd);
    pass = pass.substr(4);
    size_t pos = pass.find_first_not_of("\t\v");
    if (pos < pass.size())
    {
        pass = pass.substr(pos);
        if (pass[0] == ':')
            pass.erase(pass.begin());
    }
    if (pos == std::string::npos || pass.empty())
        sendResponse(ERR_NOTENOUGHPARAM(std::string(pass)), fd);
    if (!cli -> getRegistered())
    {
        std::string pass_str = pass;
        if (pass_str == GetPassword())
            cli -> setRegistered(true);
        else
            sendResponse(ERR_INCORPASS(cli -> GetNickName()), fd);
    }
    else
    {
        sendResponse(ERR_ALREADYREGISTERED(cli -> GetNickName()), fd);
    }
}


bool Server::is_validNickname(std::string& nickname)
{
    if ((!nickname.empty() && nickname[0] == '#') || nickname[0] == '&' || nickname[0] == ':')
        return (false);
    for (size_t i = 0; i < nickname.size(); i++)
    {
        if (!isalnum(nickname[i]) && nickname[i] != '_')
            return (false);
    }
    return (true);
}

bool    Server::nickNameInUse(std::string& nickname)
{
    for (size_t i = 0; i < clients.size(); i++)
    {
        if (clients[i].GetNickName() == nickname)
            return (true);
    }
    return (false);
}

void    Server::set_nickname(std::string cmd, int fd)
{
    std::string inuse;
    Client  *cli = GetClient(fd);
    cmd = cmd.substr(4);
    size_t pos = cmd.find_first_not_of("\t\v");
    if (pos < cmd.size())
    {
        cmd = cmd.substr(pos);
        if (cmd[0] == ':')
            cmd.erase(cmd.begin());
    }
    if (pos == std::string::npos || cmd.empty())
        sendResponse(ERR_NOTENOUGHPARAM(std::string(cmd)), fd);
    if (nickNameInUse(cmd) && cli -> GetNickName() != cmd)
    {
        inuse = "*";
        if (cli -> GetNickName().empty())
            cli -> SetNickname(cmd);
        sendResponse(ERR_NICKINUSE(std::string(cmd)), fd);
        return ;
    }
    if (!is_validNickname(cmd))
        sendResponse(ERR_ERRONEUSNICK(std::string(cmd)), fd);
    else
    {
        if (cli && cli ->getRegistered())
        {
            std::string oldnick = cli -> GetNickName();
            cli -> SetNickname(cmd);
            for (size_t i = 0; i < channels.size(); i++)
            {
                Client *cli = channels[i].GetClientInChannel(cmd);
                if (cli)
                    cli -> SetNickname(cmd);
            }
            if (!oldnick.empty() && oldnick != cmd)
            {
                cli -> setLogedin(true);
                sendResponse(RPL_CONNECTED(cli -> GetNickName()), fd);
                sendResponse(RPL_NICKCHANGE(cli -> GetNickName(), cmd), fd);
            }
            else
            {
                sendResponse(RPL_NICKCHANGE(oldnick, cmd), fd);
                return ;
            }
        }
        if (cli && !(cli -> getRegistered()))
        {
            sendResponse(ERR_NOTREGISTERED(cmd), fd);
        }
        if (cli && cli -> getRegistered() && !cli -> GetUserName().empty()
            && cli -> GetNickName() != "*" && !cli -> GetLogedIn())
        {
            cli -> setLogedin(true);
            sendResponse(RPL_CONNECTED(cli -> GetNickName()), fd);
        }
    }
}

void    Server::set_username(std::string &cmd, int fd)
{
    std::vector<std::string> splited_cmds = split_cmd(cmd);
    Client *cli = GetClient(fd);
    if (cli && splited_cmds.size() < 5)
        sendResponse(ERR_NOTENOUGHPARAM(cli -> GetNickName()), fd);
    if (cli && !cli -> getRegistered())
        sendResponse(ERR_NOTREGISTERED(cli -> GetNickName()), fd);
    if (cli && !cli -> GetUserName().empty())
        sendResponse(ERR_ALREADYREGISTERED(cli -> GetNickName()), fd);
    else
        cli -> SetUsername(cmd);
    if (cli && cli -> getRegistered() && !cli -> GetUserName().empty()
        && !cli -> GetNickName().empty() && cli -> GetNickName() != "*" && !cli -> GetLogedIn())
    {
        cli -> setLogedin(true);
        sendResponse(RPL_CONNECTED(cli -> GetNickName()), fd);
    }
}
