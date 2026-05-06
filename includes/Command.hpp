#ifndef COMMAND_HPP
# define COMMAND_HPP

# include <string>
# include <vector>

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
	std::string	commandName;
	std::string	paramsText;
	std::vector<std::string> paramList;
};

CommandType	getCommandType(const std::string& command);
Command		parseCommand(const std::string& line);
std::string	trimSpaces(const std::string& value);
std::vector<std::string> splitParams(const std::string& paramsText);

#endif
