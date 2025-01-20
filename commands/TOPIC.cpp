// TOPIC message
//      Command: TOPIC
//   Parameters: <channel> [<topic>]
// The TOPIC command is used to change or view the topic of the given channel.
// If <topic> is not given, either RPL_TOPIC or RPL_NOTOPIC is returned specifying the current
// channel topic or lack of one. If <topic> is an empty string, the topic for the channel will be cleared.

// If the client sending this command is not joined to the given channel, and tries to view
// its' topic, the server MAY return the ERR_NOTONCHANNEL (442) numeric and have the command fail.

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
#include <ctime>

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
	if (scmd.size() < 2)
		return SendError(461, GetClient(fd)->GetNickName(), GetClient(fd)->GetFd(), " :Not enough parameters\r\n");

	std::string channelWithHash = scmd[1];
	if (channelWithHash[0] != '#')
		return SendError(403, GetClient(fd)->GetNickName(), channelWithHash, GetClient(fd)->GetFd(), " :No such channel\r\n");

	std::string channelName = channelWithHash.substr(1);
	if (!GetChannel(channelName))
		return SendError(403, GetClient(fd)->GetNickName(), channelWithHash, GetClient(fd)->GetFd(), " :No such channel\r\n");

	Channel* chan = GetChannel(channelName);
	if (!chan->GetClientInChannel(GetClient(fd)->GetNickName()))
		return SendError(442, GetClient(fd)->GetNickName(), channelWithHash, GetClient(fd)->GetFd(), " :You're not on that channel\r\n");

	if (scmd.size() == 2)
	{
		if (chan->GetTopicName().empty())
		{
			SendResponse(": 331 " + GetClient(fd)->GetNickName() + " " + channelWithHash + " :No topic is set\r\n", fd);
			return;
		}
		SendResponse(": 332 " + GetClient(fd)->GetNickName() + " " + channelWithHash + " " + chan->GetTopicName() + "\r\n", fd);
		SendResponse(": 333 " + GetClient(fd)->GetNickName() + " " + channelWithHash + " " + GetClient(fd)->GetNickName() + " " + chan->GetTime() + "\r\n", fd);
		return;
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
		{SendError(331, channelWithHash, fd, " :No topic is set\r\n");return;}

		if (chan->Gettopic_restriction() && chan->get_client(fd))
		{SendError(482, channelWithHash, fd, " :You're Not a channel operator\r\n");return;}
		else if (chan->Gettopic_restriction() && chan->get_admin(fd))
		{
			chan->SetTime(tTopic());
			chan->SetTopicName(tmp[2]);
			std::string rpl;
			size_t pos = tmp[2].find(":");
			if (pos == std::string::npos)
				rpl = ":" + GetClient(fd)->GetNickName() + "!" + GetClient(fd)->GetUserName() + "@localhost TOPIC " + channelWithHash + " :" + chan->GetTopicName() + "\r\n";
			else
				rpl = ":" + GetClient(fd)->GetNickName() + "!" + GetClient(fd)->GetUserName() + "@localhost TOPIC " + channelWithHash + " " + chan->GetTopicName() + "\r\n";
			chan->sendTo_all(rpl);
		}
		else
		{
			std::string rpl;
			size_t pos = tmp[2].find(":");
			if (pos == std::string::npos)
			{
				chan->SetTime(tTopic());
				chan->SetTopicName(tmp[2]);
				rpl = ":" + GetClient(fd)->GetNickName() + "!" + GetClient(fd)->GetUserName() + "@localhost TOPIC " + channelWithHash + " " + chan->GetTopicName() + "\r\n";
			}
			else
			{
				size_t poss = tmp[2].find(" ");
				chan->SetTopicName(tmp[2]);
				if (poss == std::string::npos && tmp[2][0] == ':' && tmp[2][1] != ':')
					tmp[2] = tmp[2].substr(1);
				chan->SetTopicName(tmp[2]);
				chan->SetTime(tTopic());
				rpl = ":" + GetClient(fd)->GetNickName() + "!" + GetClient(fd)->GetUserName() + "@localhost TOPIC " + channelWithHash + " " + chan->GetTopicName() + "\r\n";
			}
			chan->sendTo_all(rpl);
		}
	}
}
