#include "ft_irc.hpp"

int	printError(std::string	error_msg)
{
	std::cout << error_msg << std::endl;
	return (1);
}


static int parseArgs(char** argv)
{


}

int main(int argc, char** argv)
{
	if (argc != 3)
		return (print_error("Error: 
			usage needs to be ./ircserv <port> <password>"));
	
	
	
	
	return (0);
}
