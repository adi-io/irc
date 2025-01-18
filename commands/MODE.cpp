// // ∗ MODE - Change the channel’s mode:

// Set/Remove Invite-only mode (i)
// /MODE #channel +i    // Set channel to invite-only
// /MODE #channel -i    // Remove invite-only restriction

// Set/Remove Topic Restriction (t)
// /MODE #channel +t    // Only operators can change topic
// /MODE #channel -t    // Anyone can change topic

// Set/Remove Channel Password (k)
// /MODE #channel +k password    // Set channel password
// /MODE #channel -k password    // Remove channel password

// Give/Take Operator Status (o)
// /MODE #channel +o nickname    // Give operator status
// /MODE #channel -o nickname    // Remove operator status

// Set/Remove User Limit (l)
// /MODE #channel +l 10    // Set user limit to 10
// /MODE #channel -l      // Remove user limit

// You can also combine multiple modes
// /MODE #channel +tk password    // Set topic restriction and password
// /MODE #channel +il 5          // Set invite-only and limit to 5 users


#include "../include/server.hpp"
#include <sstream>
#include <cstdlib>

// Validates the password
bool Tool_PasswordValidator(std::string password)
{
	if(password.empty())
		return false;
	for(size_t i = 0; i < password.size(); i++)
	{
		if(!std::isalnum(password[i]) && password[i] != '_')
			return false;
	}
	return true;
}

std::string Server::mode_toAppend(std::string chain, char oprtr, char mode)
{
	std::stringstream ss;

	ss.clear();
	char last = '\0';
	for(size_t i = 0; i < chain.size(); i++)
	{
		if(chain[i] == '+' || chain[i] == '-')
			last = chain[i];
	}
	if(last != oprtr)
		ss << oprtr << mode;
	else
		ss << mode;
	return ss.str();
}

// Parses the command to extract the channel name, mode set, and parameters
void Server::Tool_GetCmdArgs(std::string cmd, std::string& name, std::string& modeset, std::string &params)
{
	std::istringstream stm(cmd);
	stm >> name;
	stm >> modeset;
	size_t found = cmd.find_first_not_of(name + modeset + " \t\v");
	if(found != std::string::npos)
		params = cmd.substr(found);
}

// Splits the parameters by commas
std::vector<std::string> Server::Tool_SplitParams(std::string params)
{
	if(!params.empty() && params[0] == ':')
		params.erase(params.begin());
	std::vector<std::string> tokens;
	std::string param;
	std::istringstream stm(params);
	while (std::getline(stm, param, ','))
	{
		tokens.push_back(param);
		param.clear();
	}
	return tokens;
}

std::string Server::inviteOnlyToggleMod(Channel *channel, char oprtr, std::string chain)
{
	std::string param;
	param.clear();
	if(oprtr == '+' && !channel->GetModeAtIndex(0))
	{
		channel->SetModeAtindex(0, true);
		channel->SetInvitOnly(1);
		param =  mode_toAppend(chain, oprtr, 'i');
	}
	else if (oprtr == '-' && channel->GetModeAtIndex(0))
	{
		channel->SetModeAtindex(0, false);
		channel->SetInvitOnly(0);
		param =  mode_toAppend(chain, oprtr, 'i');
	}
	return param;
}

// Set/remove the restrictions of the TOPIC command to channel operators
std::string Server::topicRestrictionMod(Channel *channel ,char oprtr, std::string chain)
{
	std::string param;
	param.clear();
	if(oprtr == '+' && !channel->GetModeAtIndex(1))
	{
		channel->SetModeAtindex(1, true);
		channel->SetTopicRestriction(true);
		param =  mode_toAppend(chain, oprtr, 't');
	}
	else if (oprtr == '-' && channel->GetModeAtIndex(1))
	{
		channel->SetModeAtindex(1, false);
		channel->SetTopicRestriction(false);
		param =  mode_toAppend(chain, oprtr, 't');
	}
	return param;
}

// Handles the password mode
std::string Server::passwordMod(std::vector<std::string> tokens, Channel *channel, size_t &pos, char oprtr, int fd, std::string chain, std::string &arguments)
{
	std::string param;
	std::string pass;

	param.clear();
	pass.clear();
	if(tokens.size() > pos)
		pass = tokens[pos++];
	else
	{
		SendResponse(ERR_NEEDMODEPARM(channel->GetName(),std::string("(k)")),fd);
		return param;
	}
	if(!Tool_PasswordValidator(pass))
	{
		SendResponse(ERR_INVALIDMODEPARM(channel->GetName(),std::string("(k)")),fd);
		return param;
	}
	if(oprtr == '+')
	{
		channel->SetModeAtindex(2, true);
		channel->SetPassword(pass);
		if(!arguments.empty())
			arguments += " ";
		arguments += pass;
		param = mode_toAppend(chain,oprtr, 'k');
	}
	else if (oprtr == '-' && channel->GetModeAtIndex(2))
	{
		if(pass == channel->GetPassword())
		{
			channel->SetModeAtindex(2, false);
			channel->SetPassword("");
			param = mode_toAppend(chain,oprtr, 'k');
		}
		else
			SendResponse(ERR_KEYSET(channel->GetName()),fd);
	}
	return param;
}

// Handles the operator privilege mode
std::string Server::userPrivilegeMod(std::vector<std::string> tokens, Channel *channel, size_t& pos, int fd, char oprtr, std::string chain, std::string& arguments)
{
	std::string user;
	std::string param;

	param.clear();
	user.clear();
	if(tokens.size() > pos)
		user = tokens[pos++];
	else
	{
		SendResponse(ERR_NEEDMODEPARM(channel->GetName(),"(o)"),fd);
		return param;
	}
	if(!channel->clientInChannel(user))
	{
		SendResponse(ERR_NOSUCHNICK(channel->GetName(), user),fd);
		return param;
	}
	if(oprtr == '+')
	{
		channel->SetModeAtindex(3,true);
		if(channel->change_clientToAdmin(user))
		{
			param = mode_toAppend(chain, oprtr, 'o');
			if(!arguments.empty())
				arguments += " ";
			arguments += user;
		}
	}
	else if (oprtr == '-')
	{
		channel->SetModeAtindex(3,false);
		if(channel->change_adminToClient(user))
		{
			param = mode_toAppend(chain, oprtr, 'o');
				if(!arguments.empty())
					arguments += " ";
			arguments += user;

		}
	}
	return param;
}

// Validates the channel limit
bool Server::Tool_LimitValidator(std::string& limit)
{
	return (!(limit.find_first_not_of("0123456789")!= std::string::npos) && std::atoi(limit.c_str()) > 0);
}

// Handles the channel limit mode
std::string Server::userLimitMode(std::vector<std::string> tokens,  Channel *channel, size_t &pos, char oprtr, int fd, std::string chain, std::string& arguments)
{
	std::string limit;
	std::string param;

	param.clear();
	limit.clear();
	if(oprtr == '+')
	{
		if(tokens.size() > pos )
		{
			limit = tokens[pos++];
			if(!Tool_LimitValidator(limit))
			{
				SendResponse(ERR_INVALIDMODEPARM(channel->GetName(),"(l)"), fd);
			}
			else
			{
				channel->SetModeAtindex(4, true);
				channel->SetLimit(std::atoi(limit.c_str()));
				if(!arguments.empty())
					arguments += " ";
				arguments += limit;
				param =  mode_toAppend(chain, oprtr, 'l');
			}
		}
		else
			SendResponse(ERR_NEEDMODEPARM(channel->GetName(),"(l)"),fd);
	}
	else if (oprtr == '-' && channel->GetModeAtIndex(4))
	{
		channel->SetModeAtindex(4, false);
		channel->SetLimit(0);
		param  = mode_toAppend(chain, oprtr, 'l');
	}
	return param;
}

void Server::MODE(int fd, std::string &cmd)
{
	std::string channelName;
	std::string params;
	std::string modeset;
	std::stringstream mode_chain;
	std::string arguments = ":";
	Channel *channel;
	char opera = '\0';

	arguments.clear();
	mode_chain.clear();
	Client *cli = GetClient(fd);
	size_t found = cmd.find_first_not_of("MODEmode \t\v");
	if(found != std::string::npos)
		cmd = cmd.substr(found);
	else
	{
		SendResponse(ERR_NOTENOUGHPARAM(cli->GetNickName()), fd); 
		return ;
	}
	Tool_GetCmdArgs(cmd ,channelName, modeset ,params);
	std::vector<std::string> tokens = Tool_SplitParams(params);
	if(channelName[0] != '#' || !(channel = GetChannel(channelName.substr(1))))
	{
		SendResponse(ERR_CHANNELNOTFOUND(cli->GetUserName(),channelName), fd);
		return ;
	}
	else if (!channel->get_client(fd) && !channel->get_admin(fd))
	{
		SendError(442, GetClient(fd)->GetNickName(), channelName, GetClient(fd)->GetFd(), " :You're not on that channel\r\n"); return;
	}
	else if (modeset.empty()) // response with the channel modes (MODE #channel)
	{
		SendResponse(RPL_CHANNELMODES(cli->GetNickName(), channel->GetName(), channel->getModes()) + \
		RPL_CREATIONTIME(cli->GetNickName(), channel->GetName(),channel->get_creationtime()),fd);
		return ;
	}
	else if (!channel->get_admin(fd)) // client is not channel operator
	{
		SendResponse(ERR_NOTOPERATOR(channel->GetName()), fd);
		return ;
	}
	else if(channel)
	{
		size_t pos = 0;
		for(size_t i = 0; i < modeset.size(); i++)
		{
			if(modeset[i] == '+' || modeset[i] == '-')
				opera = modeset[i];
			else
			{
				if(modeset[i] == 'i')//invite mode
					mode_chain << inviteOnlyToggleMod(channel , opera, mode_chain.str());
				else if (modeset[i] == 't') //topic restriction mode
					mode_chain << topicRestrictionMod(channel, opera, mode_chain.str());
				else if (modeset[i] == 'k') //password set/remove
					mode_chain <<  passwordMod(tokens, channel, pos, opera, fd, mode_chain.str(), arguments);
				else if (modeset[i] == 'o') //set/remove user operator privilege
						mode_chain << userPrivilegeMod(tokens, channel, pos, fd, opera, mode_chain.str(), arguments);
				else if (modeset[i] == 'l') //set/remove channel limits
					mode_chain << userLimitMode(tokens, channel, pos, opera, fd, mode_chain.str(), arguments);
				else
					SendResponse(ERR_UNKNOWNMODE(cli->GetNickName(), channel->GetName(),modeset[i]),fd);
			}
		}
	}
	std::string chain = mode_chain.str();
	if(chain.empty())
		return ;
 	channel->sendTo_all(RPL_CHANGEMODE(cli->getHostname(), channel->GetName(), mode_chain.str(), arguments));
}
