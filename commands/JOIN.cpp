// JOIN message
//      Command: JOIN
//   Parameters: <channel>{,<channel>} [<key>{,<key>}]
//   Alt Params: 0
// The JOIN command indicates that the client wants to join the given channel(s),
// each channel using the given key for it. The server receiving the command checks whether or not 
// the client can join the given channel, and processes the request. Servers MUST process the parameters of this 
// command as lists on incoming commands from clients, with the first <key> being used for the first <channel>, 
// the second <key> being used for the second <channel>, etc.

// While a client is joined to a channel, they receive all relevant information about that channel including 
// the JOIN, PART, KICK, and MODE messages affecting the channel. They receive all PRIVMSG and NOTICE messages 
// sent to the channel, and they also receive QUIT messages from other clients joined to the same channel 
// (to let them know those users have left the channel and the network). This allows them to keep track of other 
// channel members and channel modes.

// If a client’s JOIN command to the server is successful, the server MUST send, in this order:

// A JOIN message with the client as the message <source> and the channel they have joined as the first parameter 
// of the message. The channel’s topic (with RPL_TOPIC (332) and optionally RPL_TOPICWHOTIME (333)), and no message if the channel 
// does not have a topic.
// A list of users currently joined to the channel (with one or more RPL_NAMREPLY (353) numerics followed 
// by a single RPL_ENDOFNAMES (366) numeric). These RPL_NAMREPLY messages sent by the server MUST include the 
// requesting client that has just joined the channel.
// The key, client limit , ban - exception, invite-only - exception, and other (depending on server software) 
// channel modes affect whether or not a given client may join a channel. More information on each of these modes 
// and how they affect the JOIN command is available in their respective sections.

// Servers MAY restrict the number of channels a client may be joined to at one time. This limit SHOULD be defined 
// in the CHANLIMIT RPL_ISUPPORT parameter. If the client cannot join this channel because they would be over their 
// limit, they will receive an ERR_TOOMANYCHANNELS (405) reply and the command will fail.

// Note that this command also accepts the special argument of ("0", 0x30) instead of any of the usual parameters, 
// which requests that the sending client leave all channels they are currently connected to. The server will process 
// this command as though the client had sent a PART command for each channel they are a member of.

// This message may be sent from a server to a client to notify the client that someone has joined a channel. In this 
// case, the message <source> will be the client who is joining, and <channel> will be the channel which that client has 
// joined. Servers SHOULD NOT send multiple channels in this message to clients, and SHOULD distribute 
// these multiple-channel JOIN messages as a series of messages with a single channel name on each.

// Numeric Replies:

// ERR_NEEDMOREPARAMS (461)
// ERR_NOSUCHCHANNEL (403)
// ERR_TOOMANYCHANNELS (405)
// ERR_BADCHANNELKEY (475)
// ERR_BANNEDFROMCHAN (474)
// ERR_CHANNELISFULL (471)
// ERR_INVITEONLYCHAN (473)
// ERR_BADCHANMASK (476)
// RPL_TOPIC (332)
// RPL_TOPICWHOTIME (333)
// RPL_NAMREPLY (353)
// RPL_ENDOFNAMES (366)
// Command Examples:

//   JOIN #foobar                    ; join channel #foobar.

//   JOIN &foo fubar                 ; join channel &foo using key "fubar".

//   JOIN #foo,&bar fubar            ; join channel #foo using key "fubar"
//                                   and &bar using no key.

//   JOIN #foo,#bar fubar,foobar     ; join channel #foo using key "fubar".
//                                   and channel #bar using key "foobar".

//   JOIN #foo,#bar                  ; join channels #foo and #bar.
// Message Examples:

//   :WiZ JOIN #Twilight_zone        ; WiZ is joining the channel
//                                   #Twilight_zone

//   :dan-!d@localhost JOIN #test    ; dan- is joining the channel #test

#include "../include/server.hpp"

int Server::SplitJoin(std::vector<std::pair<std::string, std::string> > &token, std::string cmd, int fd)
{
	std::vector<std::string> tmp;
	std::string ChStr, PassStr, buff;
	std::istringstream iss(cmd);
	while(iss >> cmd)
		tmp.push_back(cmd);
	if (tmp.size() < 2) {token.clear(); return 0;}
	tmp.erase(tmp.begin());
	ChStr = tmp[0]; tmp.erase(tmp.begin());
	if (!tmp.empty()) {PassStr = tmp[0]; tmp.clear();}
	for (size_t i = 0; i < ChStr.size(); i++){
		if (ChStr[i] == ',')
				{token.push_back(std::make_pair(buff, "")); buff.clear();}
		else buff += ChStr[i];
	}
	token.push_back(std::make_pair(buff, ""));
	if (!PassStr.empty()){
		size_t j = 0; buff.clear();
		for (size_t i = 0; i < PassStr.size(); i++){
			if (PassStr[i] == ',')
				{token[j].second = buff; j++; buff.clear();}
			else buff += PassStr[i];
		}
		token[j].second = buff;
	}
	for (size_t i = 0; i < token.size(); i++)//erase the empty channel names
		{if (token[i].first.empty())token.erase(token.begin() + i--);}
	for (size_t i = 0; i < token.size(); i++){//ERR_NOSUCHCHANNEL (403) // if the channel doesn't exist
		if (*(token[i].first.begin()) != '#')
			{SendError(403, GetClient(fd)->GetNickName(), token[i].first, GetClient(fd)->GetFd(), " :No such channel\r\n"); token.erase(token.begin() + i--);}
		else
			token[i].first.erase(token[i].first.begin());
	}
	return 1;
}

int Server::ClientCountCheck(std::string nickname)
{
	int count = 0;
	for (size_t i = 0; i < this->channels.size(); i++){
		if (this->channels[i].GetClientInChannel(nickname))
			count++;
	}
	return count;
}

bool IsInvited(Client *cli, std::string ChName, int flag){
	if(cli->GetInviteChannel(ChName)){
		if (flag == 1)
			cli->RmChannelInvite(ChName);
		return true;
	}
	return false;
}

void Server::HandleJoinExistingChannel(std::vector<std::pair<std::string, std::string> >&token, int i, int j, int fd)
{
	if (this->channels[j].GetClientInChannel(GetClient(fd)->GetNickName()))// if the client is already registered
		return;
	if (ClientCountCheck(GetClient(fd)->GetNickName()) >= 10)//ERR_TOOMANYCHANNELS (405) // if the client is already in 10 channels
		{SendError(405, GetClient(fd)->GetNickName(), GetClient(fd)->GetFd(), " :You have joined too many channels\r\n"); return;}
	if (!this->channels[j].GetPassword().empty() && this->channels[j].GetPassword() != token[i].second){// ERR_BADCHANNELKEY (475) // if the password is incorrect
		if (!IsInvited(GetClient(fd), token[i].first, 0))
			{SendError(475, GetClient(fd)->GetNickName(), "#" + token[i].first, GetClient(fd)->GetFd(), " :Cannot join channel (+k) - bad key\r\n"); return;}
	}
	if (this->channels[j].GetInvitOnly()){// ERR_INVITEONLYCHAN (473) // if the channel is invit only
		if (!IsInvited(GetClient(fd), token[i].first, 1))
			{SendError(473, GetClient(fd)->GetNickName(), "#" + token[i].first, GetClient(fd)->GetFd(), " :Cannot join channel (+i)\r\n"); return;}
	}
	if (this->channels[j].GetLimit() && this->channels[j].GetClientsNumber() >= this->channels[j].GetLimit())// ERR_CHANNELISFULL (471) // if the channel reached the limit of number of clients
		{SendError(471, GetClient(fd)->GetNickName(), "#" + token[i].first, GetClient(fd)->GetFd(), " :Cannot join channel (+l)\r\n"); return;}
	// add the client to the channel
	Client *cli = GetClient(fd);
	this->channels[j].add_client(*cli);
	if(channels[j].GetTopicName().empty())
		SendResponse(RPL_JOINMSG(GetClient(fd)->getHostname(),GetClient(fd)->getIpAdd(),token[i].first) + \
			RPL_NAMREPLY(GetClient(fd)->GetNickName(),channels[j].GetName(),channels[j].clientChannel_list()) + \
			RPL_ENDOFNAMES(GetClient(fd)->GetNickName(),channels[j].GetName()),fd);
	else
		SendResponse(RPL_JOINMSG(GetClient(fd)->getHostname(),GetClient(fd)->getIpAdd(),token[i].first) + \
			RPL_TOPICIS(GetClient(fd)->GetNickName(),channels[j].GetName(),channels[j].GetTopicName()) + \
			RPL_NAMREPLY(GetClient(fd)->GetNickName(),channels[j].GetName(),channels[j].clientChannel_list()) + \
			RPL_ENDOFNAMES(GetClient(fd)->GetNickName(),channels[j].GetName()),fd);
    channels[j].sendTo_all(RPL_JOINMSG(GetClient(fd)->getHostname(),GetClient(fd)->getIpAdd(),token[i].first), fd);
}


void Server::HandleJoinNewChannel(std::vector<std::pair<std::string, std::string> >&token, int i, int fd)
{
	if (ClientCountCheck(GetClient(fd)->GetNickName()) >= 10)//ERR_TOOMANYCHANNELS (405) // if the client is already in 10 channels
		{SendError(405, GetClient(fd)->GetNickName(), GetClient(fd)->GetFd(), " :You have joined too many channels\r\n"); return;}
	Channel newChannel;
	newChannel.SetName(token[i].first);
	newChannel.add_admin(*GetClient(fd));
	newChannel.set_createiontime();
	this->channels.push_back(newChannel);
	// notifiy thet the client joined the channel
    SendResponse(RPL_JOINMSG(GetClient(fd)->getHostname(),GetClient(fd)->getIpAdd(),newChannel.GetName()) + \
        RPL_NAMREPLY(GetClient(fd)->GetNickName(),newChannel.GetName(),newChannel.clientChannel_list()) + \
        RPL_ENDOFNAMES(GetClient(fd)->GetNickName(),newChannel.GetName()),fd);
}

void Server::JOIN(int fd, std::string cmd)
{
	std::vector<std::pair<std::string, std::string> > token;
	// SplitJoin(token, cmd, fd);
	if (!SplitJoin(token, cmd, fd))// ERR_NEEDMOREPARAMS (461) // if the channel name is empty
		{SendError(461, GetClient(fd)->GetNickName(), GetClient(fd)->GetFd(), " :Not enough parameters\r\n"); return;}
	if (token.size() > 10) //ERR_TOOMANYTARGETS (407) // if more than 10 Channels
		{SendError(407, GetClient(fd)->GetNickName(), GetClient(fd)->GetFd(), " :Too many channels\r\n"); return;}
	for (size_t i = 0; i < token.size(); i++){
		bool flag = false;
		for (size_t j = 0; j < this->channels.size(); j++){
			if (this->channels[j].GetName() == token[i].first){
				HandleJoinExistingChannel(token, i, j, fd);
				flag = true; break;
			}
		}
		if (!flag)
			HandleJoinNewChannel(token, i, fd);
	}
}