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

void Server::TOPIC(int fd, std::string cmd)
{
	std::ostringstream stream;
	std::vector<std::string> scmd = split_cmd(cmd);
	if (scmd.size() < 2) return senderror(461, GetClient(fd)->GetNickName(), GetClient(fd)->GetFd(), " :Not enough parameters\r\n");
	std::string channel = scmd[1];
	if (!GetChannel(channel)) return senderror(403, GetClient(fd)->GetNickName(), channel, GetClient(fd)->GetFd(), " :No such channel\r\n");
	Channel* chan = GetChannel(channel);
	if (!chan->GetClientInChannel(GetClient(fd)->GetNickName())) return senderror(442, GetClient(fd)->GetNickName(), channel, GetClient(fd)->GetFd(), " :You're not on that channel\r\n");
	if (scmd.size() == 2) {
		if (chan->GetTopic().empty())
			stream << RPL_NOTOPIC(GetClient(fd)->GetNickName(), channel);
		else
			stream << RPL_TOPIC(GetClient(fd)->GetNickName(), channel, chan->GetTopic());
	}
	else {
		if (chan->GetTopic().empty())
			stream << RPL_NOTOPIC(GetClient(fd)->GetNickName(), channel);
		else
			stream << RPL_TOPIC(GetClient(fd)->GetNickName(), channel, chan->GetTopic());
		chan->SetTopic(scmd[2]);
	}
}
