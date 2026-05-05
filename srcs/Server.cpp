/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fefo <fefo@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/09 15:53:55 by jidrizi           #+#    #+#             */
/*   Updated: 2026/05/05 17:07:26 by fefo             ###   ########.fr       */
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

void Server::recieveNewData(int _fd)
{
	char buff[1024]; //-> buffer for the received data
	memset(buff, 0, sizeof(buff)); //-> clear the buffer

	ssize_t bytes = recv(_fd, buff, sizeof(buff) - 1 , 0); //-> receive the data

	if(bytes <= 0){ //-> check if the client disconnected
		std::cout << RED << "Client <" << _fd << "> Disconnected" << WHI << std::endl;
		ClearClients(_fd); //-> clear the client
		close(_fd); //-> close the client socket
	}

	else{ //-> print the received data
		buff[bytes] = '\0';
		std::cout << YEL << "Client <" << _fd << "> Data: " << WHI << buff;
		//here you can add your code to process the received data: parse, check, authenticate, handle the command, etc...
	}
}