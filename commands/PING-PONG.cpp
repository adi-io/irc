// // PING message
//      Command: PING
//   Parameters: <token>
// The PING command is sent by either clients or servers to check the other side of the connection 
// is still connected and/or to check for connection latency, at the application layer.

// The <token> may be any non-empty string.

// When receiving a PING message, clients or servers must reply to it with a PONG message with the same <token> 
// value. This allows either to match PONG with the PING they reply to, for example to compute latency.

// Clients should not send PING during connection registration, though servers may accept it. Servers may send 
// PING during connection registration and clients must reply to them.

// Older versions of the protocol gave specific semantics to the <token> and allowed an extra parameter; but 
// these features are not consistently implemented and should not be relied on. Instead, the <token> should be treated as an opaque value by the receiver.

// // Numeric Replies:

// ERR_NEEDMOREPARAMS (461)
// ERR_NOORIGIN (409)
// Deprecated Numeric Reply:

// ERR_NOSUCHSERVER (402)

// -------------- // // -------------- // // -------------- // 



// // PONG message
//      Command: PONG
//   Parameters: [<server>] <token>
// The PONG command is used as a reply to PING commands, by both clients and servers. 
// The <token> should be the same as the one in the PING message that triggered this PONG.

// Servers MUST send a <server> parameter, and clients SHOULD ignore it. It exists for historical 
// reasons, and indicates the name of the server sending the PONG. Clients MUST NOT send a <server> parameter.

// Numeric Replies:

// None

// -------------- // 
// -------------- // 


#include "../include/server.hpp"
#include <sstream>
#include <cstdlib>

void	Server::PING(int fd, std::string cmd)
{
	PONG(fd, cmd);
}

// We will take in the cmd, parse it into tmp and return only the token
std::string getToken(std::string cmd, std::vector<std::string> &tmp)
{
	(void)tmp;
	std::string token = cmd;
	if (token[0] == ':')
		token.erase(0, 1);
	else
	{
		std::istringstream iss(token);
		std::string firstWord;
		iss >> firstWord;
		if (iss.rdbuf()->in_avail() > 0)
			token = firstWord;
	}
	return token;
}

void Server::PONG(int fd, std::string cmd)
{
    std::vector<std::string> tmp;
    std::string token = getToken(cmd, tmp);
    std::string response = ":" + this->GetServerName() + " PONG :" + token + "\r\n";
	SendResponse(response, fd);
}