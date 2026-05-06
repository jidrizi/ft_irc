#include "Channel.hpp"

Channel::Channel(const std::string& channelName)
	: name(channelName),
	  topic(""),
	  key(""),
	  userLimit(0),
	  inviteOnly(false),
	  topicRestricted(false),
	  keyEnabled(false),
	  userLimitEnabled(false)
{
}

Channel::~Channel()
{
}

const std::string&	Channel::getName() const
{
	return name;
}

const std::string&	Channel::getTopic() const
{
	return topic;
}

void	Channel::setTopic(const std::string& value)
{
	topic = value;
}

bool	Channel::isInviteOnly() const
{
	return inviteOnly;
}

void	Channel::setInviteOnly(bool value)
{
	inviteOnly = value;
}

bool	Channel::isTopicRestricted() const
{
	return topicRestricted;
}

void	Channel::setTopicRestricted(bool value)
{
	topicRestricted = value;
}

bool	Channel::hasKey() const
{
	return keyEnabled;
}

const std::string&	Channel::getKey() const
{
	return key;
}

void	Channel::setKey(const std::string& value)
{
	key = value;
	keyEnabled = !value.empty();
}

void	Channel::clearKey()
{
	key.clear();
	keyEnabled = false;
}

bool	Channel::keyMatches(const std::string& value) const
{
	return !keyEnabled || key == value;
}

bool	Channel::hasUserLimit() const
{
	return userLimitEnabled;
}

std::size_t	Channel::getUserLimit() const
{
	return userLimit;
}

void	Channel::setUserLimit(std::size_t value)
{
	userLimit = value;
	userLimitEnabled = true;
}

void	Channel::clearUserLimit()
{
	userLimit = 0;
	userLimitEnabled = false;
}

bool	Channel::isFull() const
{
	return userLimitEnabled && members.size() >= userLimit;
}

bool	Channel::hasMember(int fd) const
{
	return members.count(fd) != 0;
}

void	Channel::addMember(int fd)
{
	members.insert(fd);
}

void	Channel::removeMember(int fd)
{
	members.erase(fd);
	operators.erase(fd);
	invitedUsers.erase(fd);
}

const std::set<int>&	Channel::getMembers() const
{
	return members;
}

bool	Channel::hasOperator(int fd) const
{
	return operators.count(fd) != 0;
}

void	Channel::addOperator(int fd)
{
	operators.insert(fd);
}

void	Channel::removeOperator(int fd)
{
	operators.erase(fd);
}

void	Channel::ensureOperator()
{
	if (!operators.empty() || members.empty())
		return;
	operators.insert(*members.begin());
}

bool	Channel::isInvited(int fd) const
{
	return invitedUsers.count(fd) != 0;
}

void	Channel::addInvite(int fd)
{
	invitedUsers.insert(fd);
}

void	Channel::removeInvite(int fd)
{
	invitedUsers.erase(fd);
}

bool	Channel::empty() const
{
	return members.empty();
}
