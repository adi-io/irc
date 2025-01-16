#include "../include/server.hpp"
#include <cstddef>
#include <string>
#include <unistd.h>
#include <vector>

void    Server::PASS(int fd, std::string pass)
{
    Client  *cli = GetClient(fd);
    pass = pass.substr(4);
    size_t pos = pass.find_first_not_of(" \t\v");
    if (pos < pass.size())
    {
        pass = pass.substr(pos);
        if (pass[0] == ':')
            pass.erase(pass.begin());
    }
    if (pos == std::string::npos || pass.empty())
        SendResponse(ERR_NOTENOUGHPARAM(std::string(pass)), fd);
    if (!cli -> getRegistered())
    {
        std::string pass_str = pass;
        if (pass_str == GetPassword())
            cli -> setRegistered(true);
        else
            SendResponse(ERR_INCORPASS(cli -> GetNickName()), fd);
    }
    else
    {
        SendResponse(ERR_ALREADYREGISTERED(cli -> GetNickName()), fd);
    }
}


bool Server::is_validNickname(std::string& nickname)
{
    // std::cout << "Validating nickname: " << nickname << std::endl;
    if ((!nickname.empty() && nickname[0] == '#') || nickname[0] == '&' || nickname[0] == ':')
    {
        std::cout << "Invalid start character: " << nickname[0] << std::endl;
        return (false);
    }
    for (size_t i = 0; i < nickname.size(); i++)
    {
        if (!isalnum(nickname[i]) && nickname[i] != '_')
        {
            std::cout << "Invalid character: " << nickname[i] << std::endl;
            return (false);
        }
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

void    Server::NICK(std::string cmd, int fd)
{
    std::string inuse;
    Client  *cli = GetClient(fd);
    cmd = cmd.substr(4);
    size_t pos = cmd.find_first_not_of(" \t\v");
    if (pos < cmd.size())
    {
        cmd = cmd.substr(pos);
        if (cmd[0] == ':')
            cmd.erase(cmd.begin());
    }
    if (pos == std::string::npos || cmd.empty())
        SendResponse(ERR_NOTENOUGHPARAM(std::string(cmd)), fd);
    if (nickNameInUse(cmd) && cli -> GetNickName() != cmd)
    {
        inuse = "*";
        if (cli -> GetNickName().empty())
            cli -> SetNickname(cmd);
        SendResponse(ERR_NICKINUSE(std::string(cmd)), fd);
        return ;
    }
    if (!is_validNickname(cmd))
        SendResponse(ERR_ERRONEUSNICK(std::string(cmd)), fd);
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
				if (oldnick == "*" && !cli->GetUserName().empty()){
						cli -> setLogedin(true);
						SendResponse(RPL_CONNECTED(cli -> GetNickName()), fd);
						SendResponse(RPL_NICKCHANGE(cli -> GetNickName(), cmd), fd);
					}
					else
					{
						SendResponse(RPL_NICKCHANGE(oldnick, cmd), fd);
						return ;
					}
				}
        }
        if (cli && !(cli -> getRegistered()))
        {
            SendResponse(ERR_NOTREGISTERED(cmd), fd);
        }
        if (cli && cli -> getRegistered() && !cli -> GetUserName().empty()
            && cli -> GetNickName() != "*" && !cli -> GetLogedIn())
        {
            cli -> setLogedin(true);
            SendResponse(RPL_CONNECTED(cli -> GetNickName()), fd);
        }
    }
}

void    Server::USER(std::string &cmd, int fd)
{
    std::vector<std::string> splited_cmds = split_cmd(cmd);
    Client *cli = GetClient(fd);
    if (cli && splited_cmds.size() < 5)
        SendResponse(ERR_NOTENOUGHPARAM(cli -> GetNickName()), fd);
    if (cli && !cli -> getRegistered())
        SendResponse(ERR_NOTREGISTERED(cli -> GetNickName()), fd);
    if (cli && !cli -> GetUserName().empty())
        SendResponse(ERR_ALREADYREGISTERED(cli -> GetNickName()), fd);
    else
        cli -> SetUsername(splited_cmds[1]);
    if (cli && cli -> getRegistered() && !cli -> GetUserName().empty()
        && !cli -> GetNickName().empty() && cli -> GetNickName() != "*" && !cli -> GetLogedIn())
    {
        cli -> setLogedin(true);
        SendResponse(RPL_CONNECTED(cli -> GetNickName()), fd);
    }
}
