#include "Client.hpp"

#include <unistd.h>

UserProfile::UserProfile()
	: nickname("*"),
	  username("*"),
	  realname("*"),
	  hostname("*"),
	  registrationState(0),
	  welcomeSent(false)
{
}

std::string	UserProfile::source() const
{
	return nickname + "!" + username + "@" + hostname;
}

ClientSession::ClientSession(int fd, const std::string& ipAddress)
	: fdSocket(fd), ipAddr(ipAddress)
{
	userData.hostname = ipAddress;
}

ClientSession::~ClientSession()
{
	if (fdSocket >= 0)
		close(fdSocket);
}

int	ClientSession::fd() const
{
	return fdSocket;
}

const std::string&	ClientSession::ipAddress() const
{
	return ipAddr;
}

UserProfile&	ClientSession::user()
{
	return userData;
}

const UserProfile&	ClientSession::user() const
{
	return userData;
}

std::string&	ClientSession::recvBuffer()
{
	return recvBufferData;
}

std::string&	ClientSession::sendBuffer()
{
	return sendBufferData;
}

const std::string&	ClientSession::sendBuffer() const
{
	return sendBufferData;
}

bool	ClientSession::hasPendingOutput() const
{
	return !sendBufferData.empty();
}

void	ClientSession::consumeSentBytes(std::size_t sentBytes)
{
	sendBufferData.erase(0, sentBytes);
}

bool	ClientSession::popNextLine(std::string& line)
{
	// TODO: switch back to strict CRLF parsing if needed later.
	std::size_t lineEnd = recvBufferData.find('\n');
	if (lineEnd == std::string::npos)
		return false;
	line = recvBufferData.substr(0, lineEnd);
	if (!line.empty() && line[line.size() - 1] == '\r')
		line.erase(line.size() - 1);
	recvBufferData.erase(0, lineEnd + 1);
	return true;
}
