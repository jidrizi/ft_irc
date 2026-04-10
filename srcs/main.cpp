/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jidrizi <jidrizi@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/09 15:54:01 by jidrizi           #+#    #+#             */
/*   Updated: 2026/04/09 16:22:38 by jidrizi          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_irc.hpp"

bool Server::signal = false; //initialize the static boolean

void Server::signalHandler(int signum)
{
	(void)signum;
	std::cout << std::endl << "Signal Recieved!" << std::endl;
	Server::signal = true;
}

int	printError(std::string	error_msg)
{
	std::cerr << "Error: " << error_msg << std::endl;
	return (1);
}

void Server::servInit()
{
	// fixed port for now, will fix it later with dynamic port
	this->port = 4444;

	std::cout << "Port: " << port << std::endl;
	//start the socket somehow
	// servSocket();
	std::cout << GRE << "Server <" << servSocketfd << "> Connected" << WHI << std::endl;
	std::cout <<"Waiting to connect to Server..." << std::endl;
}

int main(int argc, char** argv)
{
	Server	serv;

	if (argc != 3)
		return (printError("usage needs to be ./ircserv <port> <password>"));
	if (serv.parseArgs(argv))
		return (1);
	
	std::cout << "Done!" << std::endl;
	std::cout << "--SERVER STARTING--" << std::endl;
	try{
		signal(SIGINT, Server::signalHandler);
		signal(SIGQUIT, Server::signalHandler);
		//SERVER INIT
		serv.servInit();
	}
	catch(const std::exception& e){
		std::cerr << e.what() << std::endl;
		//close fd's if error
	}
	std::cout << "Server closed!"<< std::endl;
	return (0);
}
