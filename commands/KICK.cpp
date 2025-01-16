// // KICK message
// //       Command: KICK
// //    Parameters: <channel> <user> *( "," <user> ) [<comment>]
// // The KICK command can be used to request the forced removal of a user from a channel. 
// It causes the <user> to be removed from the <channel> by force.

// // This message may be sent from a server to a client to notify the client that someone 
// has been removed from a channel. In this case, the message <source> will be the client who sent the kick, and <channel> will be the channel which the target client has been removed from.

// // If no comment is given, the server SHOULD use a default message instead.

// // Servers MUST NOT send multiple users in this message to clients, and MUST distribute 
// these multiple-user KICK messages as a series of messages with a single user name on each. 
// This is necessary to maintain backward compatibility with existing client software. If a 
// KICK message is distributed in this way, <comment> (if it exists) should be on each of these 
// messages.

// // Servers MAY limit the number of target users per KICK command via the TARGMAX parameter 
// of RPL_ISUPPORT, and silently drop targets if the number of targets exceeds the limit.

// // Numeric Replies:

// // ERR_NEEDMOREPARAMS (461)
// // ERR_NOSUCHCHANNEL (403)
// // ERR_CHANOPRIVSNEEDED (482)
// // ERR_USERNOTINCHANNEL (441)
// // ERR_NOTONCHANNEL (442)
// // Deprecated Numeric Reply:

// // ERR_BADCHANMASK (476)
// // Examples:

// //    KICK #Finnish Matthew           ; Command to kick Matthew from
// //                                    #Finnish

// //    KICK &Melbourne Matthew         ; Command to kick Matthew from
// //                                    &Melbourne

// //    KICK #Finnish John :Speaking English
// //                                    ; Command to kick John from #Finnish
// //                                    using "Speaking English" as the
// //                                    reason (comment).

// //    :WiZ!jto@tolsun.oulu.fi KICK #Finnish John
// //                                    ; KICK message on channel #Finnish
// //                                    from WiZ to remove John from channel

#include "../include/server.hpp"

/**
 * @brief Extracts a substring from the command string starting after the specified keyword.
 *
 * This function searches for the specified keyword within the command string. If the keyword is found,
 * it extracts the substring that follows the keyword, trims any leading spaces from the extracted substring,
 * and stores the result in the provided output string. If the keyword is not found, the output string is cleared.
 *
 * @param cmd The command string to search within.
 * @param tofind The keyword to search for within the command string.
 * @param str The output string where the extracted and trimmed substring will be stored.
 */
void ProcessCmd(const std::string &cmd, const std::string &tofind, std::string &str)
{
    size_t pos = cmd.find(tofind);
    if (pos != std::string::npos)
    {
        str = cmd.substr(pos + tofind.size());
        str.erase(0, str.find_first_not_of(' ')); // Trim leading spaces
    }
    else
        str.clear();
}

/**
 * @brief Splits the given command string into parts and extracts the reason.
 *
 * This function takes a command string and splits it into parts using a stringstream.
 * It expects the command to be split into exactly three parts. If the command does not
 * split into three parts, it returns an empty string. Otherwise, it calls the ProcessCmd
 * function to extract the reason from the command.
 *
 * @param cmd The command string to be split.
 * @param tmp A vector to store the split parts of the command.
 * @return The extracted reason if the command is valid, otherwise an empty string.
 */
std::string ExtractReason(std::string &cmd, std::vector<std::string> &tmp)
{
	std::stringstream ss(cmd);
	std::string str, reason;
	int count = 3;
	while (ss >> str && count--)
		tmp.push_back(str);
	if(tmp.size() != 3) return std::string("");
	ProcessCmd(cmd, tmp[2], reason);
	return reason;
}

/**
 * @brief Extracts the reason for a KICK command and processes the channel and user information.
 *
 * This function parses the given command to extract the reason for the KICK command, the channels involved, 
 * and the user to be kicked. It also validates the channels and formats the reason string.
 *
 * @param cmd The full command string received from the client.
 * @param tmp A vector of strings to store the parsed channel names.
 * @param user A reference to a string to store the user to be kicked.
 * @param fd The file descriptor of the client sending the command.
 * @return A string containing the reason for the KICK command.
 *
 * @note The function modifies the `tmp` vector to store the parsed channel names and the `user` string to store the user to be kicked.
 *       It also sends an error message to the client if any invalid channels are found.
 */
std::string Server::ParseKickCommand(std::string cmd, std::vector<std::string> &tmp, std::string &user, int fd)
{
	std::string reason = ExtractReason(cmd, tmp);
	if (tmp.size() < 3) // check if the client send the channel name and the client to kick
		return std::string("");
	tmp.erase(tmp.begin());
	std::string str = tmp[0]; std::string str1;
	user = tmp[1]; tmp.clear();
	for (size_t i = 0; i < str.size(); i++){//split the first string by ',' to get the channels names
		if (str[i] == ',')
			{tmp.push_back(str1); str1.clear();}
		else str1 += str[i];
	}
	tmp.push_back(str1);
	for (size_t i = 0; i < tmp.size(); i++)//erase the empty strings
		{if (tmp[i].empty())tmp.erase(tmp.begin() + i--);}
	if (reason[0] == ':') reason.erase(reason.begin());
	else //shrink to the first space
		{for (size_t i = 0; i < reason.size(); i++){if (reason[i] == ' '){reason = reason.substr(0, i);break;}}}
	for (size_t i = 0; i < tmp.size(); i++){// erase the '#' from the channel name and check if the channel valid
			if (*(tmp[i].begin()) == '#')
				tmp[i].erase(tmp[i].begin());
			else
				{SendError(403, GetClient(fd)->GetNickName(), tmp[i], GetClient(fd)->GetFd(), " :No such channel\r\n"); tmp.erase(tmp.begin() + i--);}
		}
	return reason;
}

void	Server::KICK(int fd, std::string cmd)
{
	//ERR_BADCHANMASK (476) // if the channel mask is invalid
	std::vector<std::string> tmp;
	std::string reason ,user;
	reason = ParseKickCommand(cmd, tmp, user, fd);
	if (user.empty())
		{SendError(461, GetClient(fd)->GetNickName(), GetClient(fd)->GetFd(), " :Not enough parameters\r\n"); return;}
	for (size_t i = 0; i < tmp.size(); i++){ // search for the channel
		if (GetChannel(tmp[i])){// check if the channel exist
			Channel *ch = GetChannel(tmp[i]);
			if (!ch->get_client(fd) && !ch->get_admin(fd)) // check if the client is in the channel
				{SendError(442, GetClient(fd)->GetNickName(), "#" + tmp[i], GetClient(fd)->GetFd(), " :You're not on that channel\r\n"); continue;}
			if(ch->get_admin(fd)){ // check if the client is admin
				if (ch->GetClientInChannel(user)){ // check if the client to kick is in the channel
					std::stringstream ss;
					ss << ":" << GetClient(fd)->GetNickName() << "!~" << GetClient(fd)->GetUserName() << "@" << "localhost" << " KICK #" << tmp[i] << " " << user;
					if (!reason.empty())
						ss << " :" << reason << "\r\n";
					else ss << "\r\n";
					ch->sendTo_all(ss.str());
					if (ch->get_admin(ch->GetClientInChannel(user)->GetFd()))
						ch->remove_admin(ch->GetClientInChannel(user)->GetFd());
					else
						ch->remove_client(ch->GetClientInChannel(user)->GetFd());
					if (ch->GetClientsNumber() == 0)
						channels.erase(channels.begin() + i);
				}
				else // if the client to kick is not in the channel
					{SendError(441, GetClient(fd)->GetNickName(), "#" + tmp[i], GetClient(fd)->GetFd(), " :They aren't on that channel\r\n"); continue;}
			}
			else // if the client is not admin
				{SendError(482, GetClient(fd)->GetNickName(), "#" + tmp[i], GetClient(fd)->GetFd(), " :You're not channel operator\r\n"); continue;}
		}
		else // if the channel doesn't exist
			SendError(403, GetClient(fd)->GetNickName(), "#" + tmp[i], GetClient(fd)->GetFd(), " :No such channel\r\n");
	}
}