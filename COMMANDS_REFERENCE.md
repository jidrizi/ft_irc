## Registration Commands

### `CAP LS`

Usage:
```text
CAP LS
```

Needs:
- connection open (not yet fully registered is fine)

Success reply:
- `RPL_CAP`

---

### `PASS`

Usage:
```text
PASS <password>
```

Needs:
- should be after `CAP LS` in current flow

Success reply:
- `RPL_PASS`

Errors:
- `ERR_NOTREGISTERED` (if sent too early in state flow)
- `ERR_NEEDMOREPARAMS` (missing password)
- `ERR_PASSWDMISMATCH` (wrong password)
- `ERR_ALREADYREGISTERED` (already passed registration phase)

---

### `NICK`

Usage:
```text
NICK <nickname>
```

Needs:
- password accepted (`PASS` done)

Success replies:
- `RPL_NICK` (first set)
- `RPL_NICKCHANGE` (when changing nick later)

Errors:
- `ERR_NOTREGISTERED`
- `ERR_NONICKNAMEGIVEN`
- `ERR_ERRONEUSNICKNAME`
- `ERR_NICKNAMEINUSE`

---

### `USER`

Usage:
```text
USER <username> 0 * :<realname>
```

Needs:
- password accepted (`PASS` done)

Success reply:
- `RPL_USER`

Errors:
- `ERR_NOTREGISTERED`
- `ERR_ALREADYREGISTERED`
- `ERR_NEEDMOREPARAMS`
- `ERR_INPUTTOOLONG`

---

### Registration completion

When both `NICK` and `USER` are valid after `PASS`, server sends:
- `RPL_WELCOME` (`001`)

---

## Messaging / Channel Commands

## `JOIN`

Usage:
```text
JOIN <#channel>[,<#channel>...]
JOIN <#channel> <key>
```

Needs:
- fully registered user
- valid channel mask (must start with `#`)

Success replies:
- `RPL_JOIN`
- `RPL_NAMERPLY`
- `RPL_ENDOFNAMES`

Errors:
- `ERR_NOTREGISTERED`
- `ERR_NEEDMOREPARAMS`
- `ERR_BADCHANMASK`
- `ERR_INVITEONLYCHAN` (invite-only channel, not invited)
- `ERR_BADCHANNELKEY` (key mismatch)
- `ERR_CHANNELISFULL` (channel at limit)

Notes:
- first member becomes operator
- channel is created automatically on first valid join

---

### `PART`

Usage:
```text
PART <#channel>[,<#channel>...] [:reason]
```

Needs:
- fully registered user
- user must be in channel(s)

Success reply:
- `RPL_PART`

Errors:
- `ERR_NOTREGISTERED`
- `ERR_NEEDMOREPARAMS`
- `ERR_NOSUCHCHANNEL`
- `ERR_NOTONCHANNEL`

---

### `PRIVMSG`

Usage:
```text
PRIVMSG <nick>|<#channel>[,<target>...] :<message>
```

Needs:
- fully registered user
- recipient target exists
- message text present

Success behavior:
- direct message routed to target nick
- channel message broadcast to channel members (except sender)

Errors:
- `ERR_NOTREGISTERED`
- `ERR_NORECIPIENT`
- `ERR_NOTEXTTOSEND`
- `ERR_NOSUCHNICK`
- `ERR_NOSUCHCHANNEL`
- `ERR_CANNOTSENDTOCHAN` (sender not in channel)

---

### `INVITE`

Usage:
```text
INVITE <nick> <#channel>
```

Needs:
- fully registered user
- inviter must be on channel
- target user must exist and not already be in channel
- if channel is invite-only, inviter must be operator

Success replies:
- `RPL_INVITING` (to inviter)
- `RPL_INVITE` (to invited user)

Errors:
- `ERR_NOTREGISTERED`
- `ERR_NEEDMOREPARAMS`
- `ERR_NOSUCHCHANNEL`
- `ERR_NOTONCHANNEL`
- `ERR_NOSUCHNICK`
- `ERR_USERONCHANNEL`
- `ERR_CHANOPRIVSNEEDED` (invite-only op requirement)

---

### `KICK`

Usage:
```text
KICK <#channel> <nick>[,<nick>...] [:reason]
```

Needs:
- fully registered user
- channel exists
- kicker is in channel and is operator

Success reply:
- `RPL_KICK` (broadcast to channel)

Errors:
- `ERR_NOTREGISTERED`
- `ERR_NEEDMOREPARAMS`
- `ERR_NOSUCHCHANNEL`
- `ERR_NOTONCHANNEL`
- `ERR_CHANOPRIVSNEEDED`
- `ERR_NOSUCHNICK`
- `ERR_USERNOTINCHANNEL`

---

## Other

### `PING`

Usage:
```text
PING <token>
```

Success:
- server replies with `PONG`

---

## Unknown commands

If command is not implemented:
- before registration: `ERR_NOTREGISTERED`
- after registration: `ERR_UNKNOWNCOMMAND`

