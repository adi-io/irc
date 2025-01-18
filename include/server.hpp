#ifndef SERVER_HPP
#define SERVER_HPP

#include <sys/poll.h>
#include <vector>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <fcntl.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <poll.h>
#include <csignal>
#include <unistd.h>
#include <string>
#include <cstring>
#include <sstream>

#include "replies.hpp"
#include "client.hpp"
#include "channel.hpp"
// Color stuff starts here

#define RED "\e[1;31m"
#define WHI "\e[1;37m"
#define GRE "\e[1;32m"
#define YEL "\e[1;33m"

// Color stuff ends here

class Client;
class Channel;

class Server
{
    private:
            int Port;
            int ServerSocketFd;
            static bool signal;
            std::string password;
            std::vector<Client> clients;
            std::vector<Channel> channels;
            std::vector<struct pollfd> fds;
            struct sockaddr_in add;
            struct sockaddr_in cliadd;
            struct pollfd new_cli;
    public:
            Server();
            ~Server();
            Server(Server const &src);
            Server &operator=(Server const &ser);

           	//---------------//Getters
           	int GetFd();
           	int GetPort();
           	std::string GetPassword();
           	Client *GetClient(int fd);
           	Client *GetClientNick(std::string nickname);
           	Channel *GetChannel(std::string name);
			std::string GetServerName();
           	//---------------//Setters
           	void SetFd(int server_fdsocket);
           	void SetPort(int port);
           	void SetPassword(std::string password);
           	void AddClient(Client newClient);
           	void AddChannel(Channel newChannel);
           	void AddFds(pollfd newFd);
           	void USER(std::string& username, int fd);
           	void NICK(std::string cmd, int fd);
           	//---------------//Remove Methods
           	void RemoveClient(int fd);
           	void RemoveChannel(std::string name);
           	void RemoveFds(int fd);
           	void RmChannels(int fd);
           	//---------------//Send Methods
           	void SendError(int code, std::string clientname, int fd, std::string msg);
           	void SendError(int code, std::string clientname, std::string channelname, int fd, std::string msg);
           	void SendResponse(std::string response, int fd);
           	//---------------//Close and Signal Methods
           	static void SignalHandler(int signum);
           	void close_fds();
           	//---------------//Server Methods
           	void init(int port, std::string pass);
           	void accept_new_client();
           	void set_sever_socket();
           	void reciveNewData(int fd);
           	//---------------//Parsing Methods
           	std::vector<std::string> split_recivedBuffer(std::string str);
           	std::vector<std::string> split_cmd(std::string &str);
           	void COMMANDS(std::string &cmd, int fd);
           	//---------------//Authentification Methods
           	bool notregistered(int fd);
           	bool nickNameInUse(std::string& nickname);
           	bool is_validNickname(std::string& nickname);
           	void PASS(int fd, std::string pass);
			// --------------//COMMANDS IMPLEMENTATION
			// JOIN
			void	JOIN(int fd, std::string cmd);
			int		SplitJoin(std::vector<std::pair<std::string, std::string> > &token, std::string cmd, int fd);
			void	HandleJoinExistingChannel(std::vector<std::pair<std::string, std::string> >&token, int i, int j, int fd);
			void	HandleJoinNewChannel(std::vector<std::pair<std::string, std::string> >&token, int i, int fd);
			int		ClientCountCheck(std::string nickname);
			// PRIVMSG
			void PRIVMSG(int fd, std::string cmd);
			void validateRecipients(std::vector<std::string> &tmp, int fd);
			// TOPIC
			void TOPIC(int fd, std::string cmd);
			std::string tTopic();
			std::string gettopic(std::string& input);
			int getpos(std::string &cmd);
			// MODE
			void MODE(int fd, std::string &cmd);
			std::string inviteOnlyToggleMod(Channel *channel, char oprtr, std::string chain);
			std::string topicRestrictionMod(Channel *channel ,char oprtr, std::string chain);
			std::string passwordMod(std::vector<std::string> splited, Channel *channel, size_t &pos, char oprtr, int fd, std::string chain, std::string& arguments);
			std::string userPrivilegeMod(std::vector<std::string> splited, Channel *channel, size_t& pos, int fd, char oprtr, std::string chain, std::string& arguments);
			std::string userLimitMode(std::vector<std::string> splited, Channel *channel, size_t &pos, char oprtr, int fd, std::string chain, std::string& arguments);
			bool		Tool_LimitValidator(std::string& limit);
			std::string mode_toAppend(std::string chain, char oprtr, char mode);
			std::vector<std::string> Tool_SplitParams(std::string params);
			void Tool_GetCmdArgs(std::string cmd,std::string& name, std::string& modeset ,std::string &params);
			// INVITE
			void INVITE( int &fd, std::string &cmd);
			// KICK
			void KICK(int fd, std::string cmd);
			std::string ParseKickCommand(std::string cmd, std::vector<std::string> &tmp, std::string &user, int fd);
			// PART
			void	PART(int fd, std::string cmd);
			int		SplitCmdPart(std::string cmd, std::vector<std::string> &tmp, std::string &reason, int fd);
			// QUIT
			void	QUIT(int fd, std::string cmd);
			// PING & PONG
			void	PING(int fd, std::string cmd);
			void	PONG(int fd, std::string cmd);

			//
			void	LIST(int fd, std::string cmd);
};

#endif
