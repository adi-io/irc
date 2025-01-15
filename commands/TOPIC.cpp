// TOPIC message
//      Command: TOPIC
//   Parameters: <channel> [<topic>]
// The TOPIC command is used to change or view the topic of the given channel.
// If <topic> is not given, either RPL_TOPIC or RPL_NOTOPIC is returned specifying the current
// channel topic or lack of one. If <topic> is an empty string, the topic for the channel will be cleared.

// If the client sending this command is not joined to the given channel, and tries to view
// its’ topic, the server MAY return the ERR_NOTONCHANNEL (442) numeric and have the command fail.

// If RPL_TOPIC is returned to the client sending this command, RPL_TOPICWHOTIME SHOULD also be
// sent to that client.

// If the protected topic mode is set on a channel, then clients MUST have appropriate channel
// permissions to modify the topic of that channel. If a client does not have appropriate channel
// permissions and tries to change the topic, the ERR_CHANOPRIVSNEEDED (482) numeric is returned and
// the command will fail.

// If the topic of a channel is changed or cleared, every client in that channel (including the
// author of the topic change) will receive a TOPIC command with the new topic as argument (or an empty
// argument if the topic was cleared) alerting them to how the topic has changed. If the <topic> param
// is provided but the same as the previous topic (ie. it is unchanged), servers MAY notify the author
// and/or other users anyway.

// Clients joining the channel in the future will receive a RPL_TOPIC numeric (or lack thereof) accordingly.

// Numeric Replies:

// ERR_NEEDMOREPARAMS (461)
// ERR_NOSUCHCHANNEL (403)
// ERR_NOTONCHANNEL (442)
// ERR_CHANOPRIVSNEEDED (482)
// RPL_NOTOPIC (331)
// RPL_TOPIC (332)
// RPL_TOPICWHOTIME (333)

#include "../include/server.hpp"
#include <sstream>

std::string Server::tTopic()
{
	std::time_t current = std::time(NULL);
	std::stringstream res;

	res << current;
	return res.str();
}
std::string Server::gettopic(std::string &input)
{
	size_t pos = input.find(":");
	if (pos == std::string::npos)
	{
		return "";
	}
	return input.substr(pos);
}

int Server::getpos(std::string &cmd)
{
	for (int i = 0; i < (int)cmd.size(); i++)
		if (cmd[i] == ':' && (cmd[i - 1] == 32))
			return i;
	return -1;
}

void Server::TOPIC(int fd, std::string cmd)
{
	std::ostringstream stream;
	std::vector<std::string> scmd = split_cmd(cmd);
	if (scmd.size() < 2) return SendError(461, GetClient(fd)->GetNickName(), GetClient(fd)->GetFd(), " :Not enough parameters\r\n");
	std::string channel = scmd[1];
	if (!GetChannel(channel)) return SendError(403, GetClient(fd)->GetNickName(), channel, GetClient(fd)->GetFd(), " :No such channel\r\n");
	Channel* chan = GetChannel(channel);
	if (!chan->GetClientInChannel(GetClient(fd)->GetNickName())) return SendError(442, GetClient(fd)->GetNickName(), channel, GetClient(fd)->GetFd(), " :You're not on that channel\r\n");
	if (scmd.size() == 2)
	{
		if (GetChannel(channel)->GetTopicName() == "")
		{SendResponse(": 331 " + GetClient(fd)->GetNickName() + " " + "#" + channel + " :No topic is set\r\n", fd);return;} // RPL_NOTOPIC (331) if no topic is set
		size_t pos = GetChannel(channel)->GetTopicName().find(":");
		if (GetChannel(channel)->GetTopicName() != "" && pos == std::string::npos)
		{
			SendResponse(": 332 " + GetClient(fd)->GetNickName() + " " + "#" + channel + " " + GetChannel(channel)->GetTopicName() + "\r\n", fd);			  // RPL_TOPIC (332) if the topic is set
			SendResponse(": 333 " + GetClient(fd)->GetNickName() + " " + "#" + channel + " " + GetClient(fd)->GetNickName() + " " + GetChannel(channel)->GetTime() + "\r\n", fd); // RPL_TOPICWHOTIME (333) if the topic is set
			return;
		}
		else
		{
			size_t pos = GetChannel(channel)->GetTopicName().find(" ");
			if (pos == 0)
				GetChannel(channel)->GetTopicName().erase(0, 1);
			SendResponse(": 332 " + GetClient(fd)->GetNickName() + " " + "#" + channel + " " + GetChannel(channel)->GetTopicName() + "\r\n", fd);			  // RPL_TOPIC (332) if the topic is set
			SendResponse(": 333 " + GetClient(fd)->GetNickName() + " " + "#" + channel + " " + GetClient(fd)->GetNickName() + " " + GetChannel(channel)->GetTime() + "\r\n", fd); // RPL_TOPICWHOTIME (333) if the topic is set
			return;
		}
	}

	if (scmd.size() >= 3)
	{
		std::vector<std::string> tmp;
		int pos = getpos(cmd);
		if (pos == -1 || scmd[2][0] != ':')
		{
			tmp.push_back(scmd[0]);
			tmp.push_back(scmd[1]);
			tmp.push_back(scmd[2]);
		}
		else
		{
			tmp.push_back(scmd[0]);
			tmp.push_back(scmd[1]);
			tmp.push_back(cmd.substr(getpos(cmd)));
		}

		if (tmp[2][0] == ':' && tmp[2][1] == '\0')
		{SendError(331, "#" + channel, fd, " :No topic is set\r\n");return;} // RPL_NOTOPIC (331) if no topic is set

		if (GetChannel(channel)->Gettopic_restriction() && GetChannel(channel)->get_client(fd))
		{SendError(482, "#" + channel, fd, " :You're Not a channel operator\r\n");return;} // ERR_CHANOPRIVSNEEDED (482) if the client is not a channel operator
		else if (GetChannel(channel)->Gettopic_restriction() && GetChannel(channel)->get_admin(fd))
		{
			GetChannel(channel)->SetTime(tTopic());
			GetChannel(channel)->SetTopicName(tmp[2]);
			std::string rpl;
			size_t pos = tmp[2].find(":");
			if (pos == std::string::npos)
				rpl = ":" + GetClient(fd)->GetNickName() + "!" + GetClient(fd)->GetUserName() + "@localhost TOPIC #" + channel + " :" + GetChannel(channel)->GetTopicName() + "\r\n"; // RPL_TOPIC (332) if the topic is set
			else
				rpl = ":" + GetClient(fd)->GetNickName() + "!" + GetClient(fd)->GetUserName() + "@localhost TOPIC #" + channel + " " + GetChannel(channel)->GetTopicName() + "\r\n"; // RPL_TOPIC (332) if the topic is set
			GetChannel(channel)->sendTo_all(rpl);
		}
		else
		{
			std::string rpl;
			size_t pos = tmp[2].find(":");
			if (pos == std::string::npos)
			{
				GetChannel(channel)->SetTime(tTopic());
				GetChannel(channel)->SetTopicName(tmp[2]);
				rpl = ":" + GetClient(fd)->GetNickName() + "!" + GetClient(fd)->GetUserName() + "@localhost TOPIC #" + channel + " " + GetChannel(channel)->GetTopicName() + "\r\n"; // RPL_TOPIC (332) if the topic is set
			}
			else
			{
				size_t poss = tmp[2].find(" ");
				GetChannel(channel)->SetTopicName(tmp[2]);
				if (poss == std::string::npos && tmp[2][0] == ':' && tmp[2][1] != ':')
					tmp[2] = tmp[2].substr(1);
				GetChannel(channel)->SetTopicName(tmp[2]);
				GetChannel(channel)->SetTime(tTopic());
				rpl = ":" + GetClient(fd)->GetNickName() + "!" + GetClient(fd)->GetUserName() + "@localhost TOPIC #" + channel + " " + GetChannel(channel)->GetTopicName() + "\r\n"; // RPL_TOPIC (332) if the topic is set
			}
			GetChannel(channel)->sendTo_all(rpl);
		}
	}
}