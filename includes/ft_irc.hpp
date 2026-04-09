/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_irc.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jidrizi <jidrizi@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/09 15:54:09 by jidrizi           #+#    #+#             */
/*   Updated: 2026/04/09 16:01:52 by jidrizi          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef FT_IRC
# define FT_IRC


# include <iostream>
# include <cstdlib>

class	ft_irc
{
	private:
		int			port;
		std::string	password;

	public:
		ft_irc();
		ft_irc(const ft_irc& other);
		ft_irc& operator=(const ft_irc& other);
		~ft_irc();

		int parseArgs(char** argv);

};


int	print_error(std::string	error_msg);


#endif