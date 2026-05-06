#include "Command.hpp"

#include <algorithm>
#include <cctype>

CommandType	getCommandType(const std::string& command)
{
	if (command == "WHOIS") return CMD_WHOIS;
	if (command == "BOT") return CMD_BOT;
	if (command == "CAP") return CMD_CAP;
	if (command == "PASS") return CMD_PASS;
	if (command == "NICK") return CMD_NICK;
	if (command == "USER") return CMD_USER;
	if (command == "PRIVMSG") return CMD_PRIVMSG;
	if (command == "INVITE") return CMD_INVITE;
	if (command == "JOIN") return CMD_JOIN;
	if (command == "PART") return CMD_PART;
	if (command == "PING") return CMD_PING;
	if (command == "KICK") return CMD_KICK;
	if (command == "MODE") return CMD_MODE;
	if (command == "TOPIC") return CMD_TOPIC;
	return CMD_UNKNOWN;
}

std::string	trimSpaces(const std::string& value)
{
	if (value.empty())
		return value;
	std::size_t start = 0;
	while (start < value.size() && (value[start] == ' ' || value[start] == '\t'))
		++start;
	if (start == value.size())
		return "";
	std::size_t end = value.size() - 1;
	while (end > start && (value[end] == ' ' || value[end] == '\t'
			|| value[end] == '\n' || value[end] == '\r'))
		--end;
	return value.substr(start, end - start + 1);
}

std::vector<std::string> splitParams(const std::string& paramsText)
{
	std::vector<std::string> paramList;
	std::string normalized = trimSpaces(paramsText);
	if (normalized.empty())
		return paramList;
	std::size_t index = 0;
	while (index < normalized.size())
	{
		while (index < normalized.size() && std::isspace(normalized[index]))
			++index;
		if (index >= normalized.size())
			break;
		if (normalized[index] == ':')
		{
			paramList.push_back(normalized.substr(index + 1));
			break;
		}
		std::size_t tokenStart = index;
		while (index < normalized.size() && !std::isspace(normalized[index]))
			++index;
		paramList.push_back(normalized.substr(tokenStart, index - tokenStart));
	}
	return paramList;
}

Command	parseCommand(const std::string& line)
{
	Command command;
	command.type = CMD_UNKNOWN;
	command.commandName.clear();
	command.paramsText.clear();
	command.paramList.clear();

	std::string normalized = trimSpaces(line);
	if (normalized.empty())
		return command;

	std::size_t separator = normalized.find(' ');
	if (separator == std::string::npos)
		command.commandName = normalized;
	else
	{
		command.commandName = normalized.substr(0, separator);
		command.paramsText = trimSpaces(normalized.substr(separator + 1));
	}
	std::transform(command.commandName.begin(), command.commandName.end(),
		command.commandName.begin(), ::toupper);

	command.type = getCommandType(command.commandName);
	command.paramList = splitParams(command.paramsText);
	return command;
}
