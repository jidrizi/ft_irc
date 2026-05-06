#ifndef CLIENT_HPP
# define CLIENT_HPP

# include <string>

struct UserProfile
{
	std::string nickname;
	std::string username;
	std::string realname;
	std::string hostname;
	int			registrationState;
	bool		welcomeSent;

	UserProfile();
	std::string source() const;
};

class ClientSession
{
	private:
		int			fdSocket;
		std::string	ipAddr;
		UserProfile	userData;
		std::string	recvBufferData;
		std::string	sendBufferData;

	public:
		ClientSession(int fd, const std::string& ipAddress);
		~ClientSession();

		int						fd() const;
		const std::string&		ipAddress() const;
		UserProfile&				user();
		const UserProfile&			user() const;
		std::string&				recvBuffer();
		std::string&				sendBuffer();
		const std::string&			sendBuffer() const;
		bool						hasPendingOutput() const;
		void						consumeSentBytes(std::size_t sentBytes);
		bool						popNextLine(std::string& line);
};

#endif
