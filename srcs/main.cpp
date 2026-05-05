/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jidrizi <jidrizi@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/09 15:54:01 by jidrizi           #+#    #+#             */
/*   Updated: 2026/04/28 18:36:51 by fefo             ###   ########.fr       */
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

void Server::closeFds()
{
	//looping through all the clients and closing all of them
	for (size_t i = 0; i < clients.size(); i++)
	{
		std::cout << RED << "Client <" << clients[i].getFd() << "> Disconnected" << WHI << std::endl;
		close(clients[i].getFd());
	}
	//closing the server socket
	if (servSocketfd != -1)
	{
		close(servSocketfd);
		std::cout << RED << "Server <" << servSocketfd << "> Disconnected" << WHI << std::endl;
	}
}

void Server::servSocket()
{
	struct sockaddr_in add;
	struct pollfd NewPoll;
	add.sin_family = AF_INET; //set the address to the ipv4 family
	add.sin_port = htons(this->port); //conver port into network byte order (the big endian thing)
	add.sin_addr.s_addr = INADDR_ANY; //for any machine addr

	std::cout << "sin_port = " << add.sin_port << std::endl;

	servSocketfd = socket(AF_INET, SOCK_STREAM, 0);
	if (servSocketfd == -1)
		throw(std::runtime_error("Failed to create the socket"));
		
	int en = 1;
	if (setsockopt(servSocketfd, SOL_SOCKET, SO_REUSEADDR, &en, sizeof(en)) == -1)
		throw(std::runtime_error("Failed to set SO_REUSEADDR on socket"));
	if (fcntl(servSocketfd, F_SETFD, O_NONBLOCK) == -1)
		throw(std::runtime_error("Failed to set O_NONBLOCK on socket"));
	if (bind(servSocketfd, (struct sockaddr *)&add, sizeof(add)) == -1)
		throw(std::runtime_error("Failed to bind socket"));
	if (listen(servSocketfd, SOMAXCONN) == -1)
		throw(std::runtime_error("Failed listen()"));

	NewPoll.fd = servSocketfd;
	NewPoll.events = POLLIN;
	NewPoll.revents = 0;
	fds.push_back(NewPoll);
}

void Server::servInit()
{
	// fixed port for now, will fix it later with dynamic port
	this->port = 4444;

	std::cout << "Port: " << port << std::endl;
	//start the socket somehow
	servSocket();
	std::cout << GRE << "Server <" << servSocketfd << "> Connected" << WHI << std::endl;
	std::cout <<"Waiting to connect to Server..." << std::endl;

	while(Server::Signal == false)
	{
		if (poll(&fds[0], fds.size(), -1) == -1 && Server::Signal == false)
			throw(std::runtime_error("poll() failed"));
	
		for (size_t i = 0; i < fds.size(); i++)
		{
			if (fds[i].revents & POLLIN)
			{
				if (fds[i].fd == servSocketfd)
					AcceptNewClient();
				else
					RecieveNewData(fds[i].fd);
			}
		}
	}
	closeFds();
}

void Server::acceptNewClient()
{
	Client cli; //-> create a new client
	struct sockaddr_in cliadd;
	struct pollfd NewPoll;
	socklen_t len = sizeof(cliadd);

	int incofd = accept(servSocketfd, (sockaddr *)&(cliadd), &len); //-> accept the new client
	if (incofd == -1)
		{std::cout << "accept() failed" << std::endl; return;}

	if (fcntl(incofd, F_SETFL, O_NONBLOCK) == -1) //-> set the socket option (O_NONBLOCK) for non-blocking socket
		{std::cout << "fcntl() failed" << std::endl; return;}

	NewPoll.fd = incofd; //-> add the client socket to the pollfd
	NewPoll.events = POLLIN; //-> set the event to POLLIN for reading data
	NewPoll.revents = 0; //-> set the revents to 0

	cli.setFd(incofd); //-> set the client file descriptor
	cli.setIpAddr(inet_ntoa((cliadd.sin_addr))); //-> convert the ip address to string and set it
	clients.push_back(cli); //-> add the client to the vector of clients
	fds.push_back(NewPoll); //-> add the client socket to the pollfd

	std::cout << GRE << "Client <" << incofd << "> Connected" << WHI << std::endl;
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
		//close fd's if error
		serv.closeFds();
		std::cerr << e.what() << std::endl;
	}
	std::cout << "Server closed!"<< std::endl;
	return (0);
}
