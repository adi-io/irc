## Instructions to Run the IRC Server

### Steps to Run:
1. Compile the server using `make`.
2. Start the server with the command: `./ircserv 6667 mypassword`.
3. Open another terminal and connect using: `nc localhost 6667`.
4. Authenticate with the server:
    ```
    PASS mypassword
    ```
5. Set your nickname:
    ```
    NICK <display_name>
    ```
6. Register your user:
    ```
    USER <display_name> 0 * :<first_name last_name>
    ```

For more details, refer to the [IRC documentation](https://modern.ircdocs.horse/#privmsg-message).

### TODO: Implement the Following Commands
- [KICK](#kick)
- [JOIN](#join)
- [TOPIC](#topic)
- [MODE](#mode)
- [PART](#part)
- [PRIVMSG](#privmsg)

### Additional Commands Implemented
- [QUIT](#quit)
- [INVITE](#invite)

(Note: `LIST` command will work without being registered for testing and evaluation purposes.)

---

## Command Details

### KICK
The `KICK` command removes a user from a channel.

**Usage:**
```
/KICK <channel> <user> [<comment>]
```

**Example:**
```
/KICK #exampleChannel JohnDoe "Spamming is not allowed"
```
This removes `JohnDoe` from `#exampleChannel` with the comment "Spamming is not allowed".

### JOIN
The `JOIN` command allows you to join a channel.

**Usage:**
```
/JOIN <channel>
```

**Example:**
```
/JOIN #exampleChannel
```
This joins the channel `#exampleChannel`.

### TOPIC
The `TOPIC` command sets or views the topic of a channel.

**Usage:**
```
/TOPIC <channel> [<topic>]
```

**Example:**
```
/TOPIC #exampleChannel "New topic for the channel"
```
This sets the topic of `#exampleChannel` to "New topic for the channel".

### MODE
The `MODE` command changes the mode of a user or a channel.

**Usage:**
```
/MODE <target> <mode>
```

**Example:**
```
/MODE #exampleChannel +o JohnDoe
```
This gives `JohnDoe` operator status in `#exampleChannel`.

### PART
The `PART` command allows you to leave a channel.

**Usage:**
```
/PART <channel>
```

**Example:**
```
/PART #exampleChannel
```
This leaves the channel `#exampleChannel`.

### PRIVMSG
The `PRIVMSG` command sends a private message to a user or a channel.

**Usage:**
```
/PRIVMSG <target> <message>
```

**Example:**
```
/PRIVMSG JohnDoe "Hello, how are you?"
```
This sends the message "Hello, how are you?" to `JohnDoe`.

### QUIT
The `QUIT` command disconnects you from the server.

**Usage:**
```
/QUIT [<comment>]
```

**Example:**
```
/QUIT "Goodbye!"
```
This disconnects you from the server with the comment "Goodbye!".

### INVITE
The `INVITE` command invites a user to a channel.

**Usage:**
```
/INVITE <user> <channel>
```

**Example:**
```
/INVITE JohnDoe #exampleChannel
```
This invites `JohnDoe` to join `#exampleChannel`.
