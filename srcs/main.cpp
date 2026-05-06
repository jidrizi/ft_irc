#include "ft_irc.hpp"

int main(int argc, char** argv)
{
	Server server;

	if (argc != 3)
		return printError("usage: ./ircserv <port> <password>");
	if (server.parseArgs(argv) != 0)
		return 1;

	try
	{
		signal(SIGINT, Server::signalHandler);
		signal(SIGQUIT, Server::signalHandler);
		server.run();
	}
	catch (const std::exception& e)
	{
		std::cerr << e.what() << std::endl;
		return 1;
	}
	return 0;
}