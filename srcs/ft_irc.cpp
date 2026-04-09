/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_irc.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jidrizi <jidrizi@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/09 15:53:55 by jidrizi           #+#    #+#             */
/*   Updated: 2026/04/09 16:03:52 by jidrizi          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_irc.hpp"


ft_irc::ft_irc() : port(0), password("abc")
{
	
}
ft_irc::ft_irc(const ft_irc& other) : port(other.port), password(other.password)
{
	
}

ft_irc& ft_irc::operator=(const ft_irc& other)
{
	if (this != &other)
	{
		this->port = other.port;
		this->password = other.password;
	}
	return (*this);
}

ft_irc::~ft_irc()
{
	
}


int ft_irc::parseArgs(char** argv)
{
	int	_port = std::atoi(argv[1]);
	if (_port < 1024 || _port > 65535)
		return (print_error("Invalid port"));
	this->port = _port;
	
	std::string	_password = argv[2];
	size_t		spacePos;
	if (_password.empty())
		return (print_error("Password cannot be empty"));
	spacePos = _password.find(' ');
	if (spacePos != std::string::npos)
		return (print_error("Password cannot have spaces"));
	this->password = _password;

	return (0);
}