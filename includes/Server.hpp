#ifndef SERVER_HPP
# define SERVER_HPP

# include <cerrno>
# include <csignal>
# include <cstdlib>
# include <cstring>
# include <iostream>
# include <map>
# include <set>
# include <stdexcept>
# include <string>
# include <vector>

# include <arpa/inet.h>
# include <fcntl.h>
# include <netdb.h>
# include <netinet/in.h>
# include <poll.h>
# include <sys/socket.h>
# include <sys/types.h>
# include <unistd.h>

# include "Client.hpp"
# include "Channel.hpp"
# include "Command.hpp"

# define RED "\e[1;31m"
# define WHI "\e[0;37m"
# define GRE "\e[1;32m"
# define YEL "\e[1;33m"

class Server
{
	private:
		int							port;
		std::string					password;
		int							serverSocketFd;
		std::string					host;
		std::vector<ClientSession*>	clients;
		std::vector<struct pollfd>	pollFds;
		std::map<std::string, Channel*> channels;
		static bool					stopSignal;

		Server(const Server& rhs);
		Server& operator=(const Server& rhs);

		void	initSocket();
		void	acceptNewClient();
		void	receiveFromClient(int clientFd);
		void	sendPendingToClient(int clientFd);
		void	processClientLine(ClientSession& client, const std::string& line);
		int		handlePreCommandChecks(ClientSession& client, Command& command);
		int		handlePass(ClientSession& client, Command& command);
		int		handleCap(ClientSession& client, Command& command);
		int		handleNick(ClientSession& client, Command& command);
		int		handleUser(ClientSession& client, Command& command);
		int		handleJoin(ClientSession& client, Command& command);
		int		handlePart(ClientSession& client, Command& command);
		int		handlePrivmsg(ClientSession& client, Command& command);
		int		handleInvite(ClientSession& client, Command& command);
		int		handleKick(ClientSession& client, Command& command);
		int		handleTopic(ClientSession& client, Command& command);
		void	tryCompleteRegistration(ClientSession& client);
		bool	isNicknameInUse(const std::string& nickname, int excludingFd) const;
		bool	isValidNickname(const std::string& nickname) const;
		bool	isValidChannelName(const std::string& channelName) const;
		ClientSession*	findClientByNick(const std::string& nickname);
		std::vector<std::string> splitByComma(const std::string& text) const;
		void	sendToClient(int fd, const std::string& message);
		void	broadcastToChannel(const Channel& channel, const std::string& message, int exceptFd);
		std::string	buildNamesList(const Channel& channel) const;
		void	removeClientFromAllChannels(int clientFd);
		void	disconnectClient(int clientFd);
		void	syncWriteInterest();
		ClientSession*	findClientByFd(int clientFd);
		void	closeAllFds();

	public:
		Server();
		~Server();

		int		parseArgs(char** argv);
		void	run();
		static void signalHandler(int signalNumber);
};

int	printError(const std::string& errorMessage);

#endif
