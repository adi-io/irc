### TO RUN:
./ircserv 8080 mypassword



reference at https://modern.ircdocs.horse/#privmsg-message

#### todo implement 6 commands:
- KICK
- JOIN
- TOPIC
- MODE
- PART
- PRIVMSG

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

### Sending Commands:
To send the `KICK` command to the IRC server, you would typically use the IRC client interface. Here is an example of how you might send the command programmatically:

```cpp
std::string channel = "#exampleChannel";
std::string user = "JohnDoe";
std::string comment = "Spamming is not allowed";
std::string command = "KICK " + channel + " " + user + " :" + comment + "\r\n";
send(socket, command.c_str(), command.length(), 0);
```

This code constructs the `KICK` command string and sends it to the IRC server using a socket.



# JOIN
The `JOIN` command is used to enter a channel. The command requires the channel name and an optional key if the channel is password protected.

### Usage:
```
/JOIN <channel> [<key>]
```

### Example:
```
/JOIN #exampleChannel mypassword
```

In this example, the user joins the channel `#exampleChannel` with the key `mypassword`.

### Sending Commands:
To send the `JOIN` command to the IRC server, you would typically use the IRC client interface. Here is an example of how you might send the command programmatically:

```cpp
std::string channel = "#exampleChannel";
std::string key = "mypassword";
std::string command = "JOIN " + channel + " " + key + "\r\n";
send(socket, command.c_str(), command.length(), 0);
```

This code constructs the `JOIN` command string and sends it to the IRC server using a socket.


# PRIVMSG
The `PRIVMSG` command is used to send private messages to a user or a channel. The command requires the target (either a user or a channel) and the message to be sent.

### Usage:
```
/PRIVMSG <target> <message>
```

### Example:
```
/PRIVMSG #exampleChannel Hello, everyone!
```

In this example, the message `Hello, everyone!` is sent to the channel `#exampleChannel`.

### Sending Commands:
To send the `PRIVMSG` command to the IRC server, you would typically use the IRC client interface. Here is an example of how you might send the command programmatically:

```cpp
std::string target = "#exampleChannel";
std::string message = "Hello, everyone!";
std::string command = "PRIVMSG " + target + " :" + message + "\r\n";
send(socket, command.c_str(), command.length(), 0);
```

This code constructs the `PRIVMSG` command string and sends it to the IRC server using a socket.

# Sending to Multiple Users or Channels
To send a command to multiple users or channels, you can specify multiple targets separated by commas.

### Usage:
```
/COMMAND <target1,target2,...> <message>
```

### Example:
```
/PRIVMSG #channel1,#channel2 Hello, everyone!
```

In this example, the message `Hello, everyone!` is sent to both `#channel1` and `#channel2`.

### Sending Commands:
To send the `PRIVMSG` command to multiple targets programmatically, you can construct the command string with multiple targets separated by commas:

```cpp
std::string targets = "#channel1,#channel2";
std::string message = "Hello, everyone!";
std::string command = "PRIVMSG " + targets + " :" + message + "\r\n";
send(socket, command.c_str(), command.length(), 0);
```

This code constructs the `PRIVMSG` command string with multiple targets and sends it to the IRC server using a socket.


