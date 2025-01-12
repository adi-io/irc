#include "../include/server.hpp"
#include <sstream>

// to analyse the command and split into the Channel - Key pairs
// return EXIT_SUCCESS if the command is invalid
/**
 * @brief Parses the JOIN command from the client.
 *
 * This function processes the JOIN command received from a client, which is used to join one or more channels.
 * It extracts the channel names and optional keys from the command, validates them, and stores them in the provided token vector.
 *
 * @param cmd The raw JOIN command string received from the client.
 * @param token A vector of pairs to store the parsed channel names and their corresponding keys.
 * @param fd The file descriptor of the client sending the command.
 * @return int Returns EXIT_SUCCESS if empty channel name slot, otherwise returns EXIT_FAILURE.
 *
 * The function performs the following steps:
 * 1. Splits the command string into tokens.
 * 2. Extracts the channel names and optional keys.
 * 3. Validates the channel names and keys.
 * 4. Stores the valid channel names and keys in the token vector.
 * 5. Sends an error message to the client if any invalid channel names are found.
 */
int Server::parseJoinCommand(std::string cmd, std::vector<std::pair<std::string, std::string> > token, int fd)
{
	std::vector<std::string> tmp;
	std::string channel, key, buff;
	std::istringstream iss(cmd);
	while(iss >> cmd)
		tmp.push_back(cmd);
	if (tmp.size() < 2) {token.clear(); return EXIT_SUCCESS;}
	tmp.erase(tmp.begin());
	channel = tmp[0]; tmp.erase(tmp.begin());
	if (!tmp.empty()) {key = tmp[0]; tmp.clear();}
	for (size_t i = 0; i < channel.size(); i++){
		if (channel[i] == ',')
				{token.push_back(std::make_pair(buff, "")); buff.clear();}
		else buff += channel[i];
	}
	token.push_back(std::make_pair(buff, ""));
	if (!key.empty()){
		size_t j = 0; buff.clear();
		for (size_t i = 0; i < key.size(); i++){
			if (key[i] == ',')
				{token[j].second = buff; j++; buff.clear();}
			else buff += key[i];
		}
		token[j].second = buff;
	}
	for (size_t i = 0; i < token.size(); i++)//erase the empty channel names
		{if (token[i].first.empty())token.erase(token.begin() + i--);}
	for (size_t i = 0; i < token.size(); i++){//ERR_NOSUCHCHANNEL (403) // if the channel doesn't exist
		if (*(token[i].first.begin()) != '#')
			{senderror(403, GetClient(fd)->GetNickName(), token[i].first, GetClient(fd)->GetFd(), " :No such channel\r\n"); token.erase(token.begin() + i--);}
		else
			token[i].first.erase(token[i].first.begin());
	}
	return EXIT_FAILURE;
}

int Server::clientCheckCount(std::string nick)
{
	int count = 0;
	for (size_t i = 0; i < this->channels.size(); i++)
		if (this->channels[i].GetClientInChannel(nick))
			count++;
	return count;
}

bool isInvited(Client *client, std::string channelName, int flag)
{
	if(client->GetInviteChannel(channelName)){
		if (flag == 1)
			client->RmChannelInvite(channelName);
		return true;
	}
	return false;
}

void Server::handleJoinExistingChannel(int fd, std::vector<std::pair<std::string, std::string> > token, int i, int j){

}

void Server::handleJoinNewChannel(int fd, std::vector<std::pair<std::string, std::string> > token, int i){

}

void Server::JOIN(int fd, std::string cmd)
{
	std::vector<std::pair<std::string, std::string> > token;
	if (parseJoinCommand(cmd, token, fd) == EXIT_SUCCESS)
		{senderror(461, GetClient(fd)->GetNickName(), GetClient(fd)->GetFd(), " :Not enough parameters\r\n"); return;}
	if (token.size() > 10) //ERR_TOOMANYTARGETS (407) // if more than 10 Channels
		{senderror(407, GetClient(fd)->GetNickName(), GetClient(fd)->GetFd(), " :Too many channels\r\n"); return;}
	for (size_t i = 0; i < token.size(); i++){
		bool flag = false;
		for (size_t j = 0; j < this->channels.size(); j++;){
			if (this->channels[j].GetName() == token[i].first)
			{
				handleJoinExistingChannel(fd, token, i, j);
				flag = true; break;
			}
		}
		if (!flag)
			handleJoinNewChannel(fd, token, i);
		}
}


// JOIN message
//      Command: JOIN
//   Parameters: <channel>{,<channel>} [<key>{,<key>}]
//   Alt Params: 0
// The JOIN command indicates that the client wants to join the given channel(s), each channel using the given key for it. The server receiving the command checks whether or not the client can join the given channel, and processes the request. Servers MUST process the parameters of this command as lists on incoming commands from clients, with the first <key> being used for the first <channel>, the second <key> being used for the second <channel>, etc.

// While a client is joined to a channel, they receive all relevant information about that channel including the JOIN, PART, KICK, and MODE messages affecting the channel. They receive all PRIVMSG and NOTICE messages sent to the channel, and they also receive QUIT messages from other clients joined to the same channel (to let them know those users have left the channel and the network). This allows them to keep track of other channel members and channel modes.

// If a client’s JOIN command to the server is successful, the server MUST send, in this order:

// A JOIN message with the client as the message <source> and the channel they have joined as the first parameter of the message.
// The channel’s topic (with RPL_TOPIC (332) and optionally RPL_TOPICWHOTIME (333)), and no message if the channel does not have a topic.
// A list of users currently joined to the channel (with one or more RPL_NAMREPLY (353) numerics followed by a single RPL_ENDOFNAMES (366) numeric). These RPL_NAMREPLY messages sent by the server MUST include the requesting client that has just joined the channel.
// The key, client limit , ban - exception, invite-only - exception, and other (depending on server software) channel modes affect whether or not a given client may join a channel. More information on each of these modes and how they affect the JOIN command is available in their respective sections.

// Servers MAY restrict the number of channels a client may be joined to at one time. This limit SHOULD be defined in the CHANLIMIT RPL_ISUPPORT parameter. If the client cannot join this channel because they would be over their limit, they will receive an ERR_TOOMANYCHANNELS (405) reply and the command will fail.

// Note that this command also accepts the special argument of ("0", 0x30) instead of any of the usual parameters, which requests that the sending client leave all channels they are currently connected to. The server will process this command as though the client had sent a PART command for each channel they are a member of.

// This message may be sent from a server to a client to notify the client that someone has joined a channel. In this case, the message <source> will be the client who is joining, and <channel> will be the channel which that client has joined. Servers SHOULD NOT send multiple channels in this message to clients, and SHOULD distribute these multiple-channel JOIN messages as a series of messages with a single channel name on each.

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
