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
# define ERR_NOSUCHNICK(src, nick, target) \
	":" + src + " 401 " + nick + " " + target + " :No such nick/channel\r\n"
# define ERR_NOSUCHCHANNEL(src, channel) \
	":" + src + " 403 " + channel + " :No such channel\r\n"
# define ERR_CANNOTSENDTOCHAN(src, channel) \
	":" + src + " 404 " + channel + " :Cannot send to channel\r\n"
# define ERR_NORECIPIENT(src, command) \
	":" + src + " 411 :No recipient given (" + command + ")\r\n"
# define ERR_NOTEXTTOSEND(src, nick) \
	":" + src + " 412 " + nick + " :No text to send\r\n"
# define ERR_NOTONCHANNEL(src, channel) \
	":" + src + " 442 " + channel + " :You're not on that channel\r\n"
# define ERR_USERONCHANNEL(src, nick, targetNick, channel) \
	":" + src + " 443 " + nick + " " + targetNick + " " + channel + " :is already on channel\r\n"
# define ERR_USERNOTINCHANNEL(src, targetNick, channel) \
	":" + src + " 441 " + targetNick + " " + channel + " :They aren't on that channel\r\n"
# define ERR_CHANOPRIVSNEEDED(src, channel) \
	":" + src + " 482 " + channel + " :You're not channel operator\r\n"
# define ERR_BADCHANMASK(channel) \
	": 476 " + channel + " :Bad Channel Mask\r\n"
# define ERR_INVITEONLYCHAN(src, channel) \
	":" + src + " 473 " + channel + " :Cannot join channel (+i)\r\n"
# define ERR_BADCHANNELKEY(src, channel) \
	":" + src + " 475 " + channel + " :Cannot join channel (+k)\r\n"
# define ERR_CHANNELISFULL(src, channel) \
	":" + src + " 471 " + channel + " :Cannot join channel (+l)\r\n"

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
# define RPL_JOIN(nick, user, userHost, channel) \
	":" + nick + "!" + user + "@" + userHost + " JOIN :" + channel + "\r\n"
# define RPL_PART(src, channel, message) \
	":" + src + " PART " + channel + " :" + message + "\r\n"
# define RPL_PRIVMSG(src, target, message) \
	":" + src + " PRIVMSG " + target + " :" + message + "\r\n"
# define RPL_NAMERPLY(src, nick, channel, list) \
	":" + src + " 353 " + nick + " = " + channel + " :" + list + "\r\n"
# define RPL_ENDOFNAMES(src, nick, channel) \
	":" + src + " 366 " + nick + " " + channel + " :End of /NAMES list\r\n"
# define RPL_INVITING(src, nick, target, channel) \
	":" + src + " 341 " + nick + " " + target + " " + channel + "\r\n"
# define RPL_INVITE(src, target, channel) \
	":" + src + " INVITE " + target + " :" + channel + "\r\n"
# define RPL_KICK(src, channel, reason, user) \
	":" + src + " KICK " + channel + " " + user + " :" + reason + "\r\n"
#define RPL_TOPIC(host, nick, chan, topic) \
	":" + host + " 332 " + nick + " " + chan + " :" + topic + "\r\n"
#define RPL_NOTOPIC(src,nick, chan) \
	":" + src + " 331 " + nick + " " + chan + " :No topic is set\r\n"
#define RPL_WHOISUSER(src, nick, username, host, rname) \
	":" + src + " 311 " + nick + " " + username + " " + host + " * :" + rname + "\r\n"
#define RPL_ENDOFWHOIS(src, nick) \
	":" + src + " 318 " + nick + " :End of WHOIS list\r\n" 
#endif
