/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_irc.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fefo <fefo@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/09 15:54:09 by jidrizi           #+#    #+#             */
/*   Updated: 2026/05/05 19:32:08 by fefo             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef FT_IRC
# define FT_IRC


# include <cstdlib>
# include <iostream>
# include <vector>
# include <sys/socket.h>
# include <sys/types.h>
# include <netinet/in.h>
# include <fcntl.h>
# include <unistd.h>
# include <arpa/inet.h>
# include <poll.h>
# include <csignal>

#define RED "\e[1;31m"
#define WHI "\e[0;37m"
#define GRE "\e[1;32m"
#define YEL "\e[1;33m"

enum CommandType
{
    CMD_UNKNOWN = 0,
    CMD_WHOIS,
    CMD_BOT,
    CMD_CAP,
    CMD_PASS,
    CMD_NICK,
    CMD_USER,
    CMD_PRIVMSG,
    CMD_INVITE,
    CMD_JOIN,
    CMD_PART,
    CMD_PING,
    CMD_KICK,
    CMD_MODE,
    CMD_TOPIC
};

struct Command
{
	CommandType type;
	std::string	name;
	std::string	params;
	std::vector<std::string> param_list;
};

Command startParsing(std::string str);

class Client
{
	private:
		int			fd;
		std::string	ipAddr;
		
		public:
		Client(){};
		int		getFd(){return fd;}
		std::string sendBuffer;
		std::string recvBuffer;
		void	setFd(int _fd){fd = _fd;}
		void	setIpAddr(std::string _ipaddr){ipAddr = _ipaddr;}
};


class	Server
{
	private:
		int							port;
		char						buffer[1024];
		std::string					password;
		int							servSocketfd;
		static bool					signal;
		std::vector<Client*>		clients;
		std::string 				Host;
		std::vector<struct pollfd>	fds;

	public:
		Server(){servSocketfd = -1;};
		~Server(){}
		
		Client* getClientByFd(int _fd);
		int		parseArgs(char** argv);
		void 	servInit();
		void	servSocket();
		void	acceptNewClient();
		void	recieveNewData(int _fd);

		static void signalHandler(int sigNum);

		void	closeFds();
		void	ClearClients(int _fd);
		void handleCommand(Client* cli, const std::string& line);
		void sendMsg(int fd, const std::string& msg);
		int  CheckCommand(Client* cli, Command& cmd);
		void sendPending(int fd);
		void enableWrite(int fd);
		int	handlePass(Client* cli,Command &cmd);
};

std::string	RCarriage(std::string str);
std::string	RSpaces(std::string str);
int	printError(std::string	error_msg);
CommandType getCommandType(const std::string& cmd);

#endif