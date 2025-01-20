## Bugfix Checklist

- [ ] **PRIVMSG**: Ensure that private messages are sent and received correctly.
- [ ] **QUIT**: Verify that the quit command properly disconnects the user from the server.
- [ ] **PART**: Check that the part command allows users to leave channels as expected.
- [ ] **INVITE**: Confirm with Adi that the invite command works correctly and users can invite others to channels.
- [ ] **TOPIC**: Ensure the topic command works with and without the `#` prefix and updates the channel topic correctly.
- [ ] **Channel Commands**: Verify that all commands referencing a channel (e.g., JOIN, PART, TOPIC) behave as intended with and without the `#` prefix.
- [ ] **USER**: Fix the issue where entering an incorrect username allows connection but throws an error message. Ensure proper validation and error handling for usernames.


# IRC Client Setup (No Sudo Required)

This guide shows how to set up a lightweight IRC client without system-level installation.

## Step 1: Get a Terminal-based Client

Download `weechat'

## Step 2: Launch the Client
```bash
weechat
```

## Step 3: Configure

Basic configuration commands:
```bash
/server del IRC_42  //optional command
/server add IRC_42 localhost -password=asd -username=mark -nicks=mark -realname="mark hawk" -tls=off
/connect IRC_42
```
> importantly make sure to add -tls=off! SSL connection is *not* feasible due to external library limitations.

## Additional Resources

- [WeeChat Quick Start](https://weechat.org/files/doc/stable/weechat_quickstart.en.html)
- [IRC Help](https://www.irchelp.org/)

Note: If weechat-portable isn't available, alternative web-based clients like [KiwiIRC](https://kiwiirc.com/) or [IRCCloud](https://www.irccloud.com/) work in any browser.
