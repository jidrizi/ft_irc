#ifndef SERVER_HPP
# define SERVER_HPP

# include <cerrno>
# include <csignal>
# include <cstdlib>
# include <cstring>
# include <iostream>
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
		void	tryCompleteRegistration(ClientSession& client);
		bool	isNicknameInUse(const std::string& nickname, int excludingFd) const;
		bool	isValidNickname(const std::string& nickname) const;
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
