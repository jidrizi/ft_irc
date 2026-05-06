#include "Server.hpp"
#include "Replies.hpp"

#include <algorithm>

bool Server::stopSignal = false;

Server::Server()
	: port(0), serverSocketFd(-1)
{
}

Server::~Server()
{
	closeAllFds();
	for (std::vector<ClientSession*>::iterator it = clients.begin(); it != clients.end(); ++it)
		delete *it;
	for (std::map<std::string, Channel*>::iterator it = channels.begin(); it != channels.end(); ++it)
		delete it->second;
	clients.clear();
	pollFds.clear();
	channels.clear();
}

int	printError(const std::string& errorMessage)
{
	std::cerr << "Error: " << errorMessage << std::endl;
	return 1;
}

int	Server::parseArgs(char** argv)
{
	const int parsedPort = std::atoi(argv[1]);
	if (parsedPort < 1024 || parsedPort > 65535)
		return printError("invalid port");
	port = parsedPort;

	const std::string parsedPassword = argv[2];
	if (parsedPassword.empty())
		return printError("password cannot be empty");
	if (parsedPassword.find(' ') != std::string::npos)
		return printError("password cannot contain spaces");
	password = parsedPassword;
	return 0;
}

void	Server::signalHandler(int signalNumber)
{
	(void)signalNumber;
	stopSignal = true;
}

void	Server::initSocket()
{
	char hostname[1024];
	if (gethostname(hostname, sizeof(hostname)) == -1)
		throw std::runtime_error("gethostname() failed");
	host = hostname;

	serverSocketFd = socket(AF_INET, SOCK_STREAM, 0);
	if (serverSocketFd == -1)
		throw std::runtime_error("socket() failed");

	int yes = 1;
	if (setsockopt(serverSocketFd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes)) == -1)
		throw std::runtime_error("setsockopt(SO_REUSEADDR) failed");
	if (fcntl(serverSocketFd, F_SETFL, O_NONBLOCK) == -1)
		throw std::runtime_error("fcntl(O_NONBLOCK) failed");

	struct sockaddr_in serverAddr;
	std::memset(&serverAddr, 0, sizeof(serverAddr));
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_addr.s_addr = INADDR_ANY;
	serverAddr.sin_port = htons(port);

	if (bind(serverSocketFd, reinterpret_cast<struct sockaddr*>(&serverAddr), sizeof(serverAddr)) == -1)
		throw std::runtime_error("bind() failed");
	if (listen(serverSocketFd, SOMAXCONN) == -1)
		throw std::runtime_error("listen() failed");

	struct pollfd pfd;
	pfd.fd = serverSocketFd;
	pfd.events = POLLIN;
	pfd.revents = 0;
	pollFds.push_back(pfd);
}

void	Server::run()
{
	initSocket();
	std::cout << GRE << "Server <" << serverSocketFd << "> connected" << WHI << std::endl;
	std::cout << "Listening on port " << port << std::endl;

	while (!stopSignal)
	{
		syncWriteInterest();
		if (pollFds.empty())
			break;
		if (poll(&pollFds[0], pollFds.size(), -1) == -1)
		{
			if (errno == EINTR)
				continue;
			throw std::runtime_error("poll() failed");
		}

		for (std::size_t i = 0; i < pollFds.size(); ++i)
		{
			const struct pollfd current = pollFds[i];
			if (current.revents & (POLLERR | POLLHUP | POLLNVAL))
			{
				if (current.fd != serverSocketFd)
					disconnectClient(current.fd);
				continue;
			}
			if (current.revents & POLLIN)
			{
				if (current.fd == serverSocketFd)
					acceptNewClient();
				else
					receiveFromClient(current.fd);
			}
			if (current.revents & POLLOUT)
				sendPendingToClient(current.fd);
		}
	}
}

void	Server::acceptNewClient()
{
	struct sockaddr_in clientAddr;
	socklen_t len = sizeof(clientAddr);
	const int clientFd = accept(serverSocketFd, reinterpret_cast<struct sockaddr*>(&clientAddr), &len);
	if (clientFd == -1)
	{
		if (errno != EAGAIN && errno != EWOULDBLOCK)
			std::cerr << "accept() failed: " << strerror(errno) << std::endl;
		return;
	}

	if (fcntl(clientFd, F_SETFL, O_NONBLOCK) == -1)
	{
		std::cerr << "fcntl() failed for client " << clientFd << std::endl;
		close(clientFd);
		return;
	}

	char ipBuffer[NI_MAXHOST];
	if (getnameinfo(reinterpret_cast<struct sockaddr*>(&clientAddr), len, ipBuffer, sizeof(ipBuffer), NULL, 0, NI_NUMERICHOST) != 0)
		std::strcpy(ipBuffer, "unknown");

	ClientSession* client = new ClientSession(clientFd, ipBuffer);
	clients.push_back(client);

	struct pollfd pfd;
	pfd.fd = clientFd;
	pfd.events = POLLIN;
	pfd.revents = 0;
	pollFds.push_back(pfd);
	std::cout << GRE << "Client <" << clientFd << "> connected from " << ipBuffer << WHI << std::endl;
}

void	Server::receiveFromClient(int clientFd)
{
	ClientSession* client = findClientByFd(clientFd);
	if (!client)
		return;

	char buffer[1024];
	std::memset(buffer, 0, sizeof(buffer));
	const ssize_t bytes = recv(clientFd, buffer, sizeof(buffer) - 1, 0);
	if (bytes <= 0)
	{
		disconnectClient(clientFd);
		return;
	}

	client->recvBuffer().append(buffer, bytes);
	std::string line;
	while (client->popNextLine(line))
	{
		std::cout << YEL << "RECV <" << clientFd << ">: " << WHI << line << std::endl;
		processClientLine(*client, line);
	}
}

void	Server::processClientLine(ClientSession& client, const std::string& line)
{
	Command command = parseCommand(line);
	if (command.commandName.empty())
		return;

	if (command.type == CMD_CAP)
	{
		handleCap(client, command);
		return;
	}
	if (command.type == CMD_PASS)
	{
		handlePass(client, command);
		return;
	}
	if (command.type == CMD_NICK)
	{
		handleNick(client, command);
		tryCompleteRegistration(client);
		return;
	}
	if (command.type == CMD_USER)
	{
		handleUser(client, command);
		tryCompleteRegistration(client);
		return;
	}
	if (command.type == CMD_JOIN)
	{
		handleJoin(client, command);
		return;
	}
	if (command.type == CMD_PART)
	{
		handlePart(client, command);
		return;
	}
	if (command.type == CMD_PRIVMSG)
	{
		handlePrivmsg(client, command);
		return;
	}
	if (command.type == CMD_INVITE)
	{
		handleInvite(client, command);
		return;
	}
	if (command.type == CMD_KICK)
	{
		handleKick(client, command);
		return;
	}
	if (command.type == CMD_PING)
	{
		if (!command.paramsText.empty())
			client.sendBuffer() += ":" + host + " PONG :" + command.paramsText + "\r\n";
		else
			client.sendBuffer() += ":" + host + " PONG :" + host + "\r\n";
		return;
	}
	handlePreCommandChecks(client, command);
}

int	Server::handlePreCommandChecks(ClientSession& client, Command& command)
{
	if (client.user().registrationState < 4)
	{
		client.sendBuffer() += ERR_NOTREGISTERED(host);
		return 0;
	}
	client.sendBuffer() += ERR_UNKNOWNCOMMAND(host, command.commandName, client.user().nickname);
	return 1;
}

int	Server::handleCap(ClientSession& client, Command& command)
{
	if (command.paramList.empty() || command.paramList[0] != "LS")
		return 0;
	if (client.user().registrationState == 0)
		client.user().registrationState = 1;
	client.sendBuffer() += RPL_CAP(client.user().hostname);
	return 0;
}

int	Server::handlePass(ClientSession& client, Command& command)
{
	if (client.user().registrationState == 0)
	{
		client.sendBuffer() += ERR_NOTREGISTERED(host);
		return -1;
	}
	if (client.user().registrationState >= 2)
	{
		client.sendBuffer() += ERR_ALREADYREGISTERED(host);
		return -1;
	}
	if (command.paramsText.empty())
	{
		client.sendBuffer() += ERR_NEEDMOREPARAMS(host, "PASS");
		return -1;
	}
	if (command.paramsText != password)
	{
		client.sendBuffer() += ERR_PASSWDMISMATCH(host);
		return -1;
	}
	client.user().registrationState = 2;
	client.sendBuffer() += RPL_PASS(client.user().hostname);
	return 0;
}

bool	Server::isValidNickname(const std::string& nickname) const
{
	if (nickname.empty())
		return false;
	if (nickname.size() > 30)
		return false;
	if (nickname[0] == '#' || nickname[0] == '&' || nickname[0] == ':'
		|| nickname[0] == '@' || std::isdigit(nickname[0]) || std::isspace(nickname[0]))
		return false;
	if (nickname.size() < 3)
		return false;
	for (std::size_t i = 0; i < nickname.size(); ++i)
	{
		const char c = nickname[i];
		if (!std::isalnum(c) && c != '\\' && c != '|'
			&& c != '[' && c != ']' && c != '{'
			&& c != '}' && c != '-' && c != '_')
			return false;
	}
	return true;
}

bool	Server::isNicknameInUse(const std::string& nickname, int excludingFd) const
{
	for (std::vector<ClientSession*>::const_iterator it = clients.begin(); it != clients.end(); ++it)
	{
		if ((*it)->fd() == excludingFd)
			continue;
		if ((*it)->user().nickname == nickname)
			return true;
	}
	return false;
}

int	Server::handleNick(ClientSession& client, Command& command)
{
	if (client.user().registrationState < 2)
	{
		client.sendBuffer() += ERR_NOTREGISTERED(host);
		return -1;
	}
	if (command.paramsText.empty())
	{
		client.sendBuffer() += ERR_NONICKNAMEGIVEN(host);
		return -1;
	}
	if (!isValidNickname(command.paramsText))
	{
		client.sendBuffer() += ERR_ERRONEUSNICKNAME(host, command.paramsText);
		return -1;
	}
	if (isNicknameInUse(command.paramsText, client.fd()))
	{
		client.sendBuffer() += ERR_NICKNAMEINUSE(host, command.paramsText);
		return -1;
	}
	if (client.user().registrationState == 4 && client.user().nickname != "*")
	{
		client.sendBuffer() += RPL_NICKCHANGE(client.user().nickname, client.user().username,
			client.user().hostname, command.paramsText);
		client.user().nickname = command.paramsText;
		return 0;
	}

	client.user().nickname = command.paramsText;
	client.sendBuffer() += RPL_NICK(client.user().hostname, client.user().nickname);
	if (client.user().registrationState == 2)
		client.user().registrationState = 3;
	return 0;
}

int	Server::handleUser(ClientSession& client, Command& command)
{
	if (client.user().registrationState < 2)
	{
		client.sendBuffer() += ERR_NOTREGISTERED(host);
		return -1;
	}
	if (client.user().registrationState == 4 && client.user().username != "*")
	{
		client.sendBuffer() += ERR_ALREADYREGISTERED(host);
		return -1;
	}
	if (command.paramList.size() != 4)
	{
		client.sendBuffer() += ERR_NEEDMOREPARAMS(host, "USER");
		return -1;
	}
	if (command.paramList[0].size() > 18)
	{
		client.sendBuffer() += ERR_INPUTTOOLONG(host);
		return -1;
	}
	if (command.paramList[1] != "0" || command.paramList[2] != "*"
		|| command.paramList[3].size() <= 1)
	{
		client.sendBuffer() += ERR_NEEDMOREPARAMS(host, "USER");
		return -1;
	}

	client.user().username = command.paramList[0];
	client.user().realname = command.paramList[3];
	client.sendBuffer() += RPL_USER(client.user().hostname, client.user().username);
	if (client.user().registrationState == 2)
		client.user().registrationState = 3;
	return 0;
}

void	Server::tryCompleteRegistration(ClientSession& client)
{
	if (client.user().welcomeSent)
		return;
	if (client.user().registrationState < 3)
		return;
	if (client.user().nickname == "*" || client.user().username == "*")
		return;
	client.user().registrationState = 4;
	client.user().welcomeSent = true;
	client.sendBuffer() += RPL_WELCOME(host, client.user().username, client.user().hostname,
		client.user().nickname);
}

bool	Server::isValidChannelName(const std::string& channelName) const
{
	if (channelName.empty())
		return false;
	if (channelName[0] != '#')
		return false;
	for (std::size_t i = 0; i < channelName.size(); ++i)
	{
		if (channelName[i] == ' ' || channelName[i] == ',' || channelName[i] == '\a')
			return false;
	}
	return true;
}

ClientSession*	Server::findClientByNick(const std::string& nickname)
{
	for (std::vector<ClientSession*>::iterator it = clients.begin(); it != clients.end(); ++it)
	{
		if ((*it)->user().nickname == nickname)
			return *it;
	}
	return NULL;
}

std::vector<std::string> Server::splitByComma(const std::string& text) const
{
	std::vector<std::string> values;
	std::string token;
	for (std::size_t i = 0; i < text.size(); ++i)
	{
		if (text[i] == ',')
		{
			if (!token.empty())
				values.push_back(token);
			token.clear();
			continue;
		}
		token += text[i];
	}
	if (!token.empty())
		values.push_back(token);
	return values;
}

void	Server::sendToClient(int fd, const std::string& message)
{
	ClientSession* client = findClientByFd(fd);
	if (!client)
		return;
	client->sendBuffer() += message;
}

void	Server::broadcastToChannel(const Channel& channel, const std::string& message, int exceptFd)
{
	const std::set<int>& members = channel.getMembers();
	for (std::set<int>::const_iterator it = members.begin(); it != members.end(); ++it)
	{
		if (*it == exceptFd)
			continue;
		sendToClient(*it, message);
	}
}

std::string	Server::buildNamesList(const Channel& channel) const
{
	std::string list;
	const std::set<int>& members = channel.getMembers();
	for (std::set<int>::const_iterator it = members.begin(); it != members.end(); ++it)
	{
		ClientSession* member = NULL;
		for (std::vector<ClientSession*>::const_iterator ci = clients.begin(); ci != clients.end(); ++ci)
		{
			if ((*ci)->fd() == *it)
			{
				member = *ci;
				break;
			}
		}
		if (!member)
			continue;
		if (!list.empty())
			list += " ";
		if (channel.hasOperator(*it))
			list += "@";
		list += member->user().nickname;
	}
	return list;
}

int	Server::handleJoin(ClientSession& client, Command& command)
{
	if (client.user().registrationState < 4)
		return (client.sendBuffer() += ERR_NOTREGISTERED(host), -1);
	if (command.paramList.empty())
		return (client.sendBuffer() += ERR_NEEDMOREPARAMS(host, "JOIN"), -1);

	std::vector<std::string> targets = splitByComma(command.paramList[0]);
	for (std::vector<std::string>::iterator it = targets.begin(); it != targets.end(); ++it)
	{
		if (!isValidChannelName(*it))
		{
			client.sendBuffer() += ERR_BADCHANMASK(*it);
			continue;
		}
		Channel* channel = NULL;
		std::map<std::string, Channel*>::iterator chIt = channels.find(*it);
		if (chIt == channels.end())
		{
			channel = new Channel(*it);
			channels[*it] = channel;
			if (command.paramList.size() > 1)
			{
				std::vector<std::string> keys = splitByComma(command.paramList[1]);
				if (!keys.empty())
					channel->setKey(keys[0]);
			}
		}
		else
			channel = chIt->second;

		if (channel->hasMember(client.fd()))
			continue;

		if (channel->isInviteOnly() && !channel->isInvited(client.fd()))
		{
			client.sendBuffer() += ERR_INVITEONLYCHAN(host, channel->getName());
			continue;
		}
		if (channel->isFull())
		{
			client.sendBuffer() += ERR_CHANNELISFULL(client.user().source(), channel->getName());
			continue;
		}
		if (channel->hasKey())
		{
			std::string givenKey;
			if (command.paramList.size() > 1)
			{
				std::vector<std::string> keys = splitByComma(command.paramList[1]);
				if (!keys.empty())
					givenKey = keys[0];
			}
			if (!channel->keyMatches(givenKey))
			{
				client.sendBuffer() += ERR_BADCHANNELKEY(client.user().source(), channel->getName());
				continue;
			}
		}

		channel->addMember(client.fd());
		channel->removeInvite(client.fd());
		if (channel->getMembers().size() == 1)
			channel->addOperator(client.fd());

		const std::string joinMsg = RPL_JOIN(client.user().nickname, client.user().username,
			client.user().hostname, channel->getName());
		broadcastToChannel(*channel, joinMsg, -1);
		client.sendBuffer() += RPL_NAMERPLY(host, client.user().nickname, channel->getName(), buildNamesList(*channel));
		client.sendBuffer() += RPL_ENDOFNAMES(host, client.user().nickname, channel->getName());
	}
	return 0;
}

int	Server::handlePart(ClientSession& client, Command& command)
{
	if (client.user().registrationState < 4)
		return (client.sendBuffer() += ERR_NOTREGISTERED(host), -1);
	if (command.paramList.empty())
		return (client.sendBuffer() += ERR_NEEDMOREPARAMS(host, "PART"), -1);

	std::vector<std::string> targets = splitByComma(command.paramList[0]);
	std::string reason = "Leaving";
	if (command.paramList.size() > 1)
		reason = command.paramList[1];

	for (std::vector<std::string>::iterator it = targets.begin(); it != targets.end(); ++it)
	{
		std::map<std::string, Channel*>::iterator chIt = channels.find(*it);
		if (chIt == channels.end())
		{
			client.sendBuffer() += ERR_NOSUCHCHANNEL(host, *it);
			continue;
		}
		Channel& channel = *chIt->second;
		if (!channel.hasMember(client.fd()))
		{
			client.sendBuffer() += ERR_NOTONCHANNEL(host, channel.getName());
			continue;
		}

		const std::string partMsg = RPL_PART(client.user().source(), channel.getName(), reason);
		broadcastToChannel(channel, partMsg, -1);
		channel.removeMember(client.fd());
		channel.ensureOperator();
		if (channel.empty())
		{
			delete chIt->second;
			channels.erase(chIt);
		}
	}
	return 0;
}

int	Server::handlePrivmsg(ClientSession& client, Command& command)
{
	if (client.user().registrationState < 4)
		return (client.sendBuffer() += ERR_NOTREGISTERED(host), -1);
	if (command.paramList.empty())
		return (client.sendBuffer() += ERR_NORECIPIENT(host, "PRIVMSG"), -1);
	if (command.paramList.size() < 2 || command.paramList[1].empty())
		return (client.sendBuffer() += ERR_NOTEXTTOSEND(host, client.user().nickname), -1);

	std::vector<std::string> targets = splitByComma(command.paramList[0]);
	const std::string& text = command.paramList[1];
	for (std::vector<std::string>::iterator it = targets.begin(); it != targets.end(); ++it)
	{
		if ((*it)[0] == '#')
		{
			std::map<std::string, Channel*>::iterator chIt = channels.find(*it);
			if (chIt == channels.end())
			{
				client.sendBuffer() += ERR_NOSUCHCHANNEL(host, *it);
				continue;
			}
			Channel& channel = *chIt->second;
			if (!channel.hasMember(client.fd()))
			{
				client.sendBuffer() += ERR_CANNOTSENDTOCHAN(host, channel.getName());
				continue;
			}
			broadcastToChannel(channel, RPL_PRIVMSG(client.user().source(), channel.getName(), text), client.fd());
		}
		else
		{
			ClientSession* target = findClientByNick(*it);
			if (!target)
			{
				client.sendBuffer() += ERR_NOSUCHNICK(host, client.user().nickname, *it);
				continue;
			}
			target->sendBuffer() += RPL_PRIVMSG(client.user().source(), target->user().nickname, text);
		}
	}
	return 0;
}

int	Server::handleInvite(ClientSession& client, Command& command)
{
	if (client.user().registrationState < 4)
		return (client.sendBuffer() += ERR_NOTREGISTERED(host), -1);
	if (command.paramList.size() < 2)
		return (client.sendBuffer() += ERR_NEEDMOREPARAMS(host, "INVITE"), -1);

	const std::string targetNick = command.paramList[0];
	const std::string channelName = command.paramList[1];

	std::map<std::string, Channel*>::iterator chIt = channels.find(channelName);
	if (chIt == channels.end())
		return (client.sendBuffer() += ERR_NOSUCHCHANNEL(host, channelName), -1);
	Channel& channel = *chIt->second;
	if (!channel.hasMember(client.fd()))
		return (client.sendBuffer() += ERR_NOTONCHANNEL(host, channelName), -1);
	if (channel.isInviteOnly() && !channel.hasOperator(client.fd()))
		return (client.sendBuffer() += ERR_CHANOPRIVSNEEDED(host, channelName), -1);

	ClientSession* target = findClientByNick(targetNick);
	if (!target)
		return (client.sendBuffer() += ERR_NOSUCHNICK(host, client.user().nickname, targetNick), -1);
	if (channel.hasMember(target->fd()))
		return (client.sendBuffer() += ERR_USERONCHANNEL(host, client.user().nickname, targetNick, channelName), -1);

	channel.addInvite(target->fd());
	client.sendBuffer() += RPL_INVITING(host, client.user().nickname, targetNick, channelName);
	target->sendBuffer() += RPL_INVITE(client.user().source(), targetNick, channelName);
	return 0;
}

int	Server::handleKick(ClientSession& client, Command& command)
{
	if (client.user().registrationState < 4)
		return (client.sendBuffer() += ERR_NOTREGISTERED(host), -1);
	if (command.paramList.size() < 2)
		return (client.sendBuffer() += ERR_NEEDMOREPARAMS(host, "KICK"), -1);

	const std::string channelName = command.paramList[0];
	const std::string reason = command.paramList.size() > 2 ? command.paramList[2] : client.user().nickname;
	std::map<std::string, Channel*>::iterator chIt = channels.find(channelName);
	if (chIt == channels.end())
		return (client.sendBuffer() += ERR_NOSUCHCHANNEL(host, channelName), -1);
	Channel& channel = *chIt->second;
	if (!channel.hasMember(client.fd()))
		return (client.sendBuffer() += ERR_NOTONCHANNEL(host, channelName), -1);
	if (!channel.hasOperator(client.fd()))
		return (client.sendBuffer() += ERR_CHANOPRIVSNEEDED(host, channelName), -1);

	std::vector<std::string> targets = splitByComma(command.paramList[1]);
	for (std::vector<std::string>::iterator it = targets.begin(); it != targets.end(); ++it)
	{
		ClientSession* target = findClientByNick(*it);
		if (!target)
		{
			client.sendBuffer() += ERR_NOSUCHNICK(host, client.user().nickname, *it);
			continue;
		}
		if (!channel.hasMember(target->fd()))
		{
			client.sendBuffer() += ERR_USERNOTINCHANNEL(host, *it, channelName);
			continue;
		}

		const std::string kickMsg = RPL_KICK(client.user().source(), channelName, reason, *it);
		broadcastToChannel(channel, kickMsg, -1);
		channel.removeMember(target->fd());
	}
	channel.ensureOperator();
	if (channel.empty())
	{
		delete chIt->second;
		channels.erase(chIt);
	}
	return 0;
}

void	Server::sendPendingToClient(int clientFd)
{
	ClientSession* client = findClientByFd(clientFd);
	if (!client || !client->hasPendingOutput())
		return;

	const std::string& pending = client->sendBuffer();
	std::cout << GRE << "SEND <" << clientFd << ">: " << WHI << pending;
	const ssize_t sent = send(clientFd, pending.c_str(), pending.size(), 0);
	if (sent < 0)
	{
		if (errno == EAGAIN || errno == EWOULDBLOCK)
			return;
		disconnectClient(clientFd);
		return;
	}
	if (sent == 0)
	{
		disconnectClient(clientFd);
		return;
	}
	client->consumeSentBytes(static_cast<std::size_t>(sent));
}

void	Server::disconnectClient(int clientFd)
{
	removeClientFromAllChannels(clientFd);
	for (std::vector<struct pollfd>::iterator it = pollFds.begin(); it != pollFds.end(); ++it)
	{
		if (it->fd == clientFd)
		{
			pollFds.erase(it);
			break;
		}
	}

	for (std::vector<ClientSession*>::iterator it = clients.begin(); it != clients.end(); ++it)
	{
		if ((*it)->fd() == clientFd)
		{
			std::cout << RED << "Client <" << clientFd << "> disconnected" << WHI << std::endl;
			delete *it;
			clients.erase(it);
			break;
		}
	}
}

void	Server::removeClientFromAllChannels(int clientFd)
{
	for (std::map<std::string, Channel*>::iterator it = channels.begin(); it != channels.end(); )
	{
		Channel& channel = *it->second;
		channel.removeMember(clientFd);
		channel.ensureOperator();
		if (channel.empty())
		{
			delete it->second;
			channels.erase(it++);
		}
		else
			++it;
	}
}

void	Server::syncWriteInterest()
{
	for (std::vector<struct pollfd>::iterator it = pollFds.begin(); it != pollFds.end(); ++it)
	{
		if (it->fd == serverSocketFd)
			continue;
		ClientSession* client = findClientByFd(it->fd);
		if (client && client->hasPendingOutput())
			it->events = POLLIN | POLLOUT;
		else
			it->events = POLLIN;
	}
}

ClientSession*	Server::findClientByFd(int clientFd)
{
	for (std::vector<ClientSession*>::iterator it = clients.begin(); it != clients.end(); ++it)
	{
		if ((*it)->fd() == clientFd)
			return *it;
	}
	return NULL;
}

void	Server::closeAllFds()
{
	if (serverSocketFd != -1)
	{
		close(serverSocketFd);
		serverSocketFd = -1;
	}
}