// INVITE message
//      Command: INVITE
//   Parameters: <nickname> <channel>
// The INVITE command is used to invite a user to a channel. The parameter <nickname> 
// is the nickname of the person to be invited to the target channel <channel>.

// The target channel SHOULD exist (at least one user is on it). Otherwise, the server 
// SHOULD reject the command with the ERR_NOSUCHCHANNEL numeric.

// Only members of the channel are allowed to invite other users. Otherwise, the server 
// MUST reject the command with the ERR_NOTONCHANNEL numeric.

// Servers MAY reject the command with the ERR_CHANOPRIVSNEEDED numeric. In particular, 
// they SHOULD reject it when the channel has invite-only mode set, and the user is not a 
// channel operator.

// If the user is already on the target channel, the server MUST reject the command 
// with the ERR_USERONCHANNEL numeric.

// When the invite is successful, the server MUST send a RPL_INVITING numeric to 
// the command issuer, and an INVITE message, with the issuer as <source>, to the target 
// user. Other channel members SHOULD NOT be notified.

// Numeric Replies:

// RPL_INVITING (341)
// ERR_NEEDMOREPARAMS (461)
// ERR_NOSUCHCHANNEL (403)
// ERR_NOTONCHANNEL (442)
// ERR_CHANOPRIVSNEEDED (482)
// ERR_USERONCHANNEL (443)
// Command Examples:

//   INVITE Wiz #foo_bar    ; Invite Wiz to #foo_bar
// Message Examples:

//   :dan-!d@localhost INVITE Wiz #test    ; dan- has invited Wiz
//                                         to the channel #test
// See also:

// IRCv3 invite-notify Extension
// Invite list

// Servers MAY allow the INVITE with no parameter, and reply with a list of 
// channels the sender is invited to as RPL_INVITELIST (336) numerics, ending with a RPL_ENDOFINVITELIST (337) numeric.

// Some rare implementations use numerics 346/347 instead of 336/337 as 
// `RPL_INVITELIST`/`RPL_ENDOFINVITELIST`. You should check the server you are 
// using implements them as expected.

// 346/347 now generally stands for `RPL_INVEXLIST`/`RPL_ENDOFINVEXLIST`, used 
// for invite-exception list.

#include "../include/server.hpp"

void Server::INVITE( int &fd, std::string &cmd)
{
	std::vector<std::string> scmd = split_cmd(cmd);
	if(scmd.size() < 3)// ERR_NEEDMOREPARAMS (461) if there are not enough parameters
		{SendError(461, GetClient(fd)->GetNickName(), fd, " :Not enough parameters\r\n"); return;}
	std::string channelname = scmd[2].substr(1);
	if(scmd[2][0] != '#' || !GetChannel(channelname))// ERR_NOSUCHCHANNEL (403) if the given channel does not exist
	    {SendError(403, channelname, fd, " :No such channel\r\n"); return;}
	if (!(GetChannel(channelname)->get_client(fd)) && !(GetChannel(channelname)->get_admin(fd)))// ERR_NOTONCHANNEL (442) if the client is not on the channel
	    {SendError(442, channelname, fd, " :You're not on that channel\r\n"); return;}
	if (GetChannel(channelname)->GetClientInChannel(scmd[1]))// ERR_USERONCHANNEL (443) if the given nickname is already on the channel
	    {SendError(443, GetClient(fd)->GetNickName(), channelname, fd, " :is already on channel\r\n"); return;}
	Client *clt = GetClientNick(scmd[1]);
	if (!clt)// ERR_NOSUCHNICK (401) if the given nickname is not found
		{SendError(401, scmd[1], fd, " :No such nick\r\n");return;}
	if (GetChannel(channelname)->GetInvitOnly() && !GetChannel(channelname)->get_admin(fd))// ERR_INVITEONLYCHAN (473) if the channel is invite-only
		{SendError(482,GetChannel(channelname)->get_client(fd)->GetNickName(),scmd[1],fd," :You're not channel operator\r\n"); return;}
	if (GetChannel(channelname)->GetLimit() && GetChannel(channelname)->GetClientsNumber() >= GetChannel(channelname)->GetLimit()) // ERR_CHANNELISFULL (471) if the channel is full
		{SendError(473,GetChannel(channelname)->get_client(fd)->GetNickName(),channelname,fd," :Cannot invit to channel (+i)\r\n"); return;}
	// RPL_INVITING (341) if the invite was successfully sent
	clt->AddChannelInvite(channelname);
	std::string rep1 = ": 341 "+ GetClient(fd)->GetNickName()+" "+ clt->GetNickName()+" "+ scmd[2]+"\r\n";
	SendResponse(rep1, fd);
	std::string rep2 = ":"+ clt->getHostname() + " INVITE " + clt->GetNickName() + " " + scmd[2]+"\r\n";
	SendResponse(rep2, clt->GetFd());
	
}