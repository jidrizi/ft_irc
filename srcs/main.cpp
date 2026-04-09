/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jidrizi <jidrizi@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/09 15:54:01 by jidrizi           #+#    #+#             */
/*   Updated: 2026/04/09 16:32:29 by jidrizi          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_irc.hpp"

int	printError(std::string	error_msg)
{
	std::cerr << "Error: " << error_msg << std::endl;
	return (1);
}



int main(int argc, char** argv)
{
	ft_irc	x;

	if (argc != 3)
		return (printError("usage needs to be ./ircserv <port> <password>"));
	
	if (x.parseArgs(argv))
		return (1);
	
	return (0);
}
