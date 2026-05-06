#ifndef CHANNEL_HPP
# define CHANNEL_HPP

# include <set>
# include <string>

class Channel
{
	private:
		std::string		name;
		std::string		topic;
		std::string		key;
		std::size_t		userLimit;
		bool			inviteOnly;
		bool			topicRestricted;
		bool			keyEnabled;
		bool			userLimitEnabled;
		std::set<int>	members;
		std::set<int>	operators;
		std::set<int>	invitedUsers;

	public:
		Channel(const std::string& channelName);
		~Channel();

		const std::string&	getName() const;
		const std::string&	getTopic() const;
		void				setTopic(const std::string& value);

		bool				isInviteOnly() const;
		void				setInviteOnly(bool value);
		bool				isTopicRestricted() const;
		void				setTopicRestricted(bool value);

		bool				hasKey() const;
		const std::string&	getKey() const;
		void				setKey(const std::string& value);
		void				clearKey();
		bool				keyMatches(const std::string& value) const;

		bool				hasUserLimit() const;
		std::size_t			getUserLimit() const;
		void				setUserLimit(std::size_t value);
		void				clearUserLimit();
		bool				isFull() const;

		bool				hasMember(int fd) const;
		void				addMember(int fd);
		void				removeMember(int fd);
		const std::set<int>& getMembers() const;

		bool				hasOperator(int fd) const;
		void				addOperator(int fd);
		void				removeOperator(int fd);
		void				ensureOperator();

		bool				isInvited(int fd) const;
		void				addInvite(int fd);
		void				removeInvite(int fd);

		bool				empty() const;
};

#endif
