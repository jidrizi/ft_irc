#ifndef REPLIES_HPP
# define REPLIES_HPP

# define ERR_UNKNOWNCOMMAND(src, cmd, nick) \
	":" + src + " 421 " + nick + " " + cmd + " :Unknown command\r\n"
# define ERR_NONICKNAMEGIVEN(src) \
	":" + src + " 431 :No nickname given\r\n"
# define ERR_ERRONEUSNICKNAME(src, nick) \
	":" + src + " 432 " + nick + " :Erroneus nickname\r\n"
# define ERR_NICKNAMEINUSE(src, nick) \
	":" + src + " 433 " + nick + " :Nickname is already in use\r\n"
# define ERR_NOTREGISTERED(source) \
	":" + source + " 451 :You have not registered\r\n"
# define ERR_NEEDMOREPARAMS(src, cmd) \
	":" + src + " 461 " + cmd + " :Not enough parameters\r\n"
# define ERR_ALREADYREGISTERED(src) \
	":" + src + " 462 :You may not reregister\r\n"
# define ERR_PASSWDMISMATCH(src) \
	":" + src + " 464 PASS :Password incorrect\r\n"
# define ERR_INPUTTOOLONG(src) \
	":" + src + " 417 :Input line too long\r\n"

# define RPL_NICK(src, nick) \
	":" + src + " :Your nickname has been set to " + nick + "\r\n"
# define RPL_PASS(src) \
	":" + src + " :Password is correct, you may continue the registration\r\n"
# define RPL_USER(src, user) \
	":" + src + " :Your username is now set to: " + user + "\r\n"
# define RPL_CAP(src) \
	":" + src + " CAP * LS : you may enter PASS now...\r\n"
# define RPL_WELCOME(src, user, host, nick) \
	":" + src + " 001 " + nick + " :Welcome to the IRC_server network " + nick + "!" + user + "@" + host + "\r\n"
# define RPL_NICKCHANGE(oldnick, user, host, newnick) \
	":" + oldnick + "!" + user + "@" + host + " NICK :" + newnick + "\r\n"

#endif
