### TO RUN:
1. compile with make
2. ./ircserv 6667 mypassword
3. in another terminal: nc localhost 6667
4. PASS mypassword
5. NICK <display_name>
6. USER <display_name> 0 * :<first_name last_name>



reference at https://modern.ircdocs.horse/#privmsg-message

#### todo implement 6 commands:
- [KICK](#kick)
- [JOIN](#join)
- [TOPIC](#topic)
- [MODE](#mode)
- [PART](#part)
- [PRIVMSG](#privmsg)

#### also added:
- [QUIT](#quit)
- [INVITE](#invite)

(LIST will work without being registered -- testing and eval. purposes)

# KICK
The `KICK` command is used to remove a user from a channel. The command requires the channel name and the nickname of the user to be removed. An optional comment can also be included to specify the reason for the kick.

### Usage:
```
/KICK <channel> <user> [<comment>]
```

### Example:
```
/KICK #exampleChannel JohnDoe "Spamming is not allowed"
```

In this example, the user `JohnDoe` is removed from the channel `#exampleChannel` with the comment "Spamming is not allowed".

# JOIN
The `JOIN` command is used to join a channel. The command requires the channel name.

### Usage:
```
/JOIN <channel>
```

### Example:
```
/JOIN #exampleChannel
```

In this example, the user joins the channel `#exampleChannel`.

# TOPIC
The `TOPIC` command is used to set or view the topic of a channel. The command requires the channel name and optionally the new topic.

### Usage:
```
/TOPIC <channel> [<topic>]
```

### Example:
```
/TOPIC #exampleChannel "New topic for the channel"
```

In this example, the topic of the channel `#exampleChannel` is set to "New topic for the channel".

# MODE
The `MODE` command is used to change the mode of a user or a channel. The command requires the target (user or channel) and the mode to be set.

### Usage:
```
/MODE <target> <mode>
```

### Example:
```
/MODE #exampleChannel +o JohnDoe
```

In this example, the user `JohnDoe` is given operator status in the channel `#exampleChannel`.

# PART
The `PART` command is used to leave a channel. The command requires the channel name.

### Usage:
```
/PART <channel>
```

### Example:
```
/PART #exampleChannel
```

In this example, the user leaves the channel `#exampleChannel`.

# PRIVMSG
The `PRIVMSG` command is used to send a private message to a user or a channel. The command requires the target (user or channel) and the message.

### Usage:
```
/PRIVMSG <target> <message>
```

### Example:
```
/PRIVMSG JohnDoe "Hello, how are you?"
```

In this example, a private message "Hello, how are you?" is sent to the user `JohnDoe`.

# QUIT
The `QUIT` command is used to disconnect from the server. An optional comment can be included to specify the reason for quitting.

### Usage:
```
/QUIT [<comment>]
```

### Example:
```
/QUIT "Goodbye!"
```

In this example, the user disconnects from the server with the comment "Goodbye!".

# INVITE
The `INVITE` command is used to invite a user to a channel. The command requires the nickname of the user to be invited and the channel name.

### Usage:
```
/INVITE <user> <channel>
```

### Example:
```
/INVITE JohnDoe #exampleChannel
```

In this example, the user `JohnDoe` is invited to join the channel `#exampleChannel`.
