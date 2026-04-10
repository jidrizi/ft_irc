/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_irc.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jidrizi <jidrizi@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/09 15:53:55 by jidrizi           #+#    #+#             */
/*   Updated: 2026/04/09 16:22:46 by jidrizi          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_irc.hpp"


int Server::parseArgs(char** argv)
{
	int	_port = std::atoi(argv[1]);
	if (_port < 1024 || _port > 65535)
		return (printError("invalid port"));
	this->port = _port;
	
	std::string	_password = argv[2];
	size_t		spacePos;
	if (_password.empty())
		return (printError("password cannot be empty"));
	spacePos = _password.find(' ');
	if (spacePos != std::string::npos)
		return (printError("password cannot have spaces"));
	this->password = _password;

	return (0);
}

void	Server::ClearClients(int _fd)
{
	for (size_t i = 0; i < fds.size(); i++)
	{
		if (fds[i].fd == _fd)
		{
			fds.erase(fds.begin() + i);
			break ;
		}
	}

	for (size_t i = 0; i < clients.size(); i++)
	{
		if (clients[i].getFd() == _fd)
		{
			clients.erase(clients.begin() + i);
			break ;
		}
	}
}