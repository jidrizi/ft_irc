/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_irc.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jidrizi <jidrizi@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/09 15:54:09 by jidrizi           #+#    #+#             */
/*   Updated: 2026/04/09 16:20:34 by jidrizi          ###   ########.fr       */
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


class Client
{
	private:
		int			fd;
		std::string	ipAddr;

	public:
		Client(){};
		int		getFd(){return fd;}

		void	setFd(int _fd){fd = _fd;}
		void	setIpAddr(std::string _ipaddr){ipAddr = _ipaddr;}
};


class	Server
{
	private:
		int							port;
		std::string					password;
		int							servSocketfd;
		static bool					signal;
		std::vector<Client>			clients;
		std::vector<struct pollfd>	fds;

	public:
		Server(){servSocketfd = -1;};
		~Server(){}

		int		parseArgs(char** argv);
		void 	servInit();
		void	servSocket();
		void	acceptNewClient();
		void	recieveNewData(int _fd);

		static void signalHandler(int sigNum);

		void	closeFds();
		void	ClearClients(int _fd);

};


int	printError(std::string	error_msg);


#endif