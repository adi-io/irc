// CAP message
//      Command: CAP
//   Parameters: <subcommand> [:<capabilities>]
// The CAP command is used to negotiate IRCv3 capabilities with the server.
// It allows clients to request and enable protocol extensions.

// The subcommands are:
// LS - List available capabilities
// LIST - List currently enabled capabilities
// REQ - Request specific capabilities
// ACK - Acknowledge capability negotiation
// NAK - Reject capability negotiation
// END - End capability negotiation

// The capabilities parameter is a space-separated list of capability names.
// Some capabilities may have values specified after an equals sign.

// Capability negotiation must be completed before registration can complete.
// Clients should send CAP END when done negotiating capabilities.

// Numeric Replies:
// ERR_INVALIDCAPCMD (410)
// ERR_NEEDMOREPARAMS (461)

#include "../include/server.hpp"
#include <sstream>
#include <cstdlib>

void Server::CAP(int fd, std::string cmd)
{
				std::vector<std::string> tokens;
				std::istringstream iss(cmd);
				std::string token;
				std::string capabilities = "JOIN PART INVITE KICK MODE TOPIC PRIVMSG QUIT PING PONG CAP";

				// Parse the CAP command
				while(iss >> token) {
								tokens.push_back(token);
				}

				// Need at least a subcommand
				if(tokens.size() < 2) {
								SendError(461, GetClient(fd)->GetNickName(), fd, " :Not enough parameters\r\n");
								return;
				}

				std::string subcommand = tokens[1];

				// Handle each subcommand
				if(subcommand == "LS")
        			SendResponse("CAP * LS :" + capabilities + "\r\n", fd);
				else if(subcommand == "LIST")
       				SendResponse("CAP * LIST :\r\n", fd);
				else if(subcommand == "REQ") {
								// Reject all capability requests since none supported
								SendResponse("CAP * NAK :" + tokens[2] + "\r\n", fd);
				}
				else if(subcommand == "END") {
								// End capability negotiation
								return;
				}
				else {
								// Invalid subcommand
								SendError(410, GetClient(fd)->GetNickName(), fd, " :Invalid CAP command\r\n");
				}
}
