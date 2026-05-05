/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fefo <fefo@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/09 15:53:55 by jidrizi           #+#    #+#             */
/*   Updated: 2026/05/05 20:22:25 by fefo             ###   ########.fr       */
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
		if (clients[i]->getFd() == _fd)
		{
			clients.erase(clients.begin() + i);
			break ;
		}
	}
}

void Server::recieveNewData(int _fd)
{
	Client* cli = getClientByFd(_fd);
	if (!cli)
	{
		std::cerr << "Error: client not found for fd " << _fd << std::endl;
		return;
	}
	
	memset(this->buffer, 0, sizeof(this->buffer)); //-> clear the buffer

	ssize_t bytes = recv(_fd, this->buffer, sizeof(this->buffer) - 1 , 0); //-> receive the data

	if(bytes <= 0){ //-> check if the client disconnected
		std::cout << RED << "Client <" << _fd << "> Disconnected" << WHI << std::endl;
		ClearClients(_fd); //-> clear the client
		close(_fd); //-> close the client socket
	}
	else{ //-> print the received data
		this->buffer[bytes] = '\0';
		cli->recvBuffer += this->buffer;
		std::cout << YEL << "Client <" << _fd << "> Data: " << WHI << this->buffer;
		size_t pos;
		while ((pos = cli->recvBuffer.find("\r\n")) != std::string::npos)
		{
			std::string line = cli->recvBuffer.substr(0, pos);
			cli->recvBuffer.erase(0, pos + 2);
			Command cmd = startParsing(line);
			if (!CheckCommand(cli, cmd))
				handleCommand(cli, line);
		}
		//here you can add your code to process the received data: parse, check, authenticate, handle the command, etc...
	}
}

void Server::enableWrite(int fd)
{
    for (size_t i = 0; i < fds.size(); i++)
    {
        if (fds[i].fd == fd)
        {
            fds[i].events |= POLLOUT;
            break;
        }
    }
}
int Server::handlePass(Client* cli, Command &cmd)
{
    std::cout << "HERE" << std::endl;

    if (cmd.params.empty())
    {
        cli->sendBuffer += "461 PASS :Not enough parameters\r\n";
        enableWrite(cli->getFd());
        return -1;
    }

    cmd.param_list.push_back(cmd.params);
    std::string pass = cmd.param_list[0];

    if (pass != this->password)
    {
        cli->sendBuffer += "464 :Password incorrect\r\n";
        enableWrite(cli->getFd());
        return -1;
    }

    cli->sendBuffer += "OK :Password accepted\r\n";
    enableWrite(cli->getFd());
    return 0;
}

void Server::sendPending(int fd)
{
    Client* cli = getClientByFd(fd);
    if (!cli)
        return;

    if (cli->sendBuffer.empty())
    {
        for (size_t i = 0; i < fds.size(); i++)
        {
            if (fds[i].fd == fd)
            {
                fds[i].events &= ~POLLOUT;
                break;
            }
        }
        return;
    }
	// to send a specific msg
	// std::string msg = "reply";
	// cli->sendBuffer = msg + "/r/n";
	std::cout <<"SENDING: "<< cli->sendBuffer << std::endl;
    ssize_t sent = send(fd, cli->sendBuffer.c_str(), cli->sendBuffer.size(), 0);
    if (sent <= 0)
        return;

    cli->sendBuffer.erase(0, sent);
}

int Server::CheckCommand(Client* cli, Command& cmd)
{
    switch (cmd.type)
    {
        case CMD_CAP:
            return 0;

        case CMD_PASS:
            return handlePass(cli, cmd);

    
        default:
            // cli->setBuffer(ERR_UNKNOWNCOMMAND(Host, cmd.name, cli->getNickname()));
            return -1;
    }
}

void Server::sendMsg(int fd, const std::string& msg)
{
	send(fd, msg.c_str(), msg.size(), 0);
}

Command startParsing(std::string str)
{
	Command cmd;
	str = RSpaces(str);
	str = RCarriage(str);
	int j = 0;
	std::cout << str << " <-- str" << std::endl;
	for (int i = 0; str[i]!= '\0';i++) {
		std::cout << "here"<< std::endl;
		if (str[i] == ' ')
		{
			cmd.name = str.substr(0, i);
			break ;
		}
		j = i;
	}
	if (cmd.name.empty())
		cmd.name = str.substr(0, str.length());
	if (cmd.name.length() < str.length())
		cmd.params = str.substr(j + 1, str.length());
	cmd.params = RSpaces(cmd.params);
	cmd.type = getCommandType(cmd.name);
	return cmd;
}
CommandType getCommandType(const std::string& cmd)
{
    if (cmd == "WHOIS") return CMD_WHOIS;
    if (cmd == "BOT") return CMD_BOT;
    if (cmd == "CAP") return CMD_CAP;
    if (cmd == "PASS") return CMD_PASS;
    if (cmd == "NICK") return CMD_NICK;
    if (cmd == "USER") return CMD_USER;
    if (cmd == "PRIVMSG") return CMD_PRIVMSG;
    if (cmd == "INVITE") return CMD_INVITE;
    if (cmd == "JOIN") return CMD_JOIN;
    if (cmd == "PART") return CMD_PART;
    if (cmd == "PING") return CMD_PING;
    if (cmd == "KICK") return CMD_KICK;
    if (cmd == "MODE") return CMD_MODE;
    if (cmd == "TOPIC") return CMD_TOPIC;

    return CMD_UNKNOWN;
}
  
std::string	RCarriage(std::string str)
{
	std::string res;
	for (size_t i = 0;i < str.length();i++)
	{
		if (str[i] != '\r')
			res += str[i];
	}
	return res;
}

std::string	RSpaces(std::string str)
{
	int i = 0;
	std::string res;

	res = str;
	while (str[i] && (str[i] == ' ' || str[i] == '\t'))
		i++;
	if (i)
		res = str.substr(i, str.length() - i);
	i = 0;
	while (res[i])
		i++;
	i--;
	while (res[i] && (res[i] == ' ' || res[i] == '\t' || res[i] == '\n' || res[i] == '\r'))
		i--;
	res = res.substr(0, i + 1);
	return res;
}

void Server::handleCommand(Client* cli, const std::string& line)
{
	// std::istringstream iss(line);
	// std::string cmd;
	// iss >> cmd;

	// if (cmd == "NICK")
	// 	handleNick(cli, iss);
	// else if (cmd == "USER")
	// 	handleUser(cli, iss);
	// else if (cmd == "PING")
	// 	handlePing(cli, iss);
}

Client* Server::getClientByFd(int _fd)
{
    for (size_t i = 0; i < clients.size(); i++)
    {
        if (clients[i] && clients[i]->getFd() == _fd)
            return clients[i];
    }
    return NULL;
}