#include "../include/server.hpp"
#include <sstream>

void getRecipients(std::string &cmd,std::vector<std::string> &tmp)
{
	std::istringstream iss(cmd);
	std::string first, second;
	iss >> first >> second;
	cmd.erase(0, first.length() + 1 + second.length() + 1);

	std::string recipient;
	std::istringstream recipients(second);
	while (std::getline(recipients, recipient, ',')) {
		tmp.push_back(recipient);
	}
}

// go through the cmd, which is in the format "TEXT #Recipient,Recipient,Recipient something message something"  ---   so the cmd should be stripped of the first string, the second string and the rest should be the only thing which is stored.. we can always see what is the first and second string, as they are delimited by spaces...

// So in this example we would strip TEXT, #Recipient,Recipient,Recipient and the rest is what is left in the cmd string

// also - we take #Recipient,Recipient,Recipient and we split it after commas and store it in the tmp vector
// so tmp[0] will be #Recipient, tmp[1] will be Recipient, etc..

// split the recipients away -- parse the command to get the recipients of the message and return the rest of the string
std::string getMessage(std::string cmd, std::vector<std::string> &tmp)
{
	getRecipients(cmd, tmp);
	std::string message = cmd;
	if (message[0] == ':') {
		message.erase(0, 1);
	} else {
		std::istringstream iss(message);
		std::string firstWord;
		iss >> firstWord;
		if (iss.rdbuf()->in_avail() > 0) {
			message = firstWord;
		}
	}
	return message;
}

// to check if the recipient users / channels are valid and throw an error on invalid PRIVMSG recipients
void Server::validateRecipients(std::vector<std::string> &tmp, int fd)
{
	for(size_t i = 0; i < tmp.size(); i++){
		if (tmp[i][0] == '#'){
			tmp[i].erase(tmp[i].begin());
			if(!GetChannel(tmp[i]))//ERR_NOSUCHNICK (401) // if the channel doesn't exist
				{SendError(401, "#" + tmp[i], GetClient(fd)->GetFd(), " :No such nick/channel\r\n"); tmp.erase(tmp.begin() + i); i--;}
			else if (!GetChannel(tmp[i])->GetClientInChannel(GetClient(fd)->GetNickName())) //ERR_CANNOTSENDTOCHAN (404) // if the client is not in the channel
				{SendError(404, GetClient(fd)->GetNickName(), "#" + tmp[i], GetClient(fd)->GetFd(), " :Cannot send to channel\r\n"); tmp.erase(tmp.begin() + i); i--;}
			else tmp[i] = "#" + tmp[i];
		}
		else{
			if (!GetClientNick(tmp[i]))//ERR_NOSUCHNICK (401) // if the client doesn't exist
				{SendError(401, tmp[i], GetClient(fd)->GetFd(), " :No such nick/channel\r\n"); tmp.erase(tmp.begin() + i); i--;}
		}
	}
}

void Server::PRIVMSG(int fd, std::string cmd)
{
	std::vector<std::string> tmp;
	std::string message = getMessage(cmd, tmp);
	if (!tmp.size())//ERR_NORECIPIENT (411) // if the client doesn't specify the recipient
		{SendError(411, GetClient(fd)->GetNickName(), GetClient(fd)->GetFd(), " :No recipient given (PRIVMSG)\r\n"); return;}
	if (message.empty())//ERR_NOTEXTTOSEND (412) // if the client doesn't specify the message
		{SendError(412, GetClient(fd)->GetNickName(), GetClient(fd)->GetFd(), " :No text to send\r\n"); return;}
	if (tmp.size() > 10) //ERR_TOOMANYTARGETS (407) // if the client send the message to more than 10 clients
		{SendError(407, GetClient(fd)->GetNickName(), GetClient(fd)->GetFd(), " :Too many recipients\r\n"); return;}
	validateRecipients(tmp, fd); // check if the channels and clients exist
	std::cout << "PRIVMSG " << message << std::endl;
}
