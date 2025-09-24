# tpp-mod

Improvements and fixes for Metal Gear Solid V: The Phantom Pain

discord server
https://discord.gg/hYfW9MEEGF

## Installation

Download [dinput8.dll](https://github.alicent.cat/tpp-mod/dinput8.dll) and copy it to the game folder

## Features

- Allows overriding of existing lua scripts (working directory is `[mgsv game folder]/tpp-mod`)
- **variables**:  
  similar to cod dvars (can be set through the console or through the config file `localappdata/tpp-mod/config.json`):  
  - `com_unlock_fps`: unlocks fps past 60  
  - `com_worker_count`: changes maximum number of worker threads for rendering  
  - `net_udp`: makes sessions use udp sockets instead of steam networking (only works on tpp with a custom server backend)  
  - `net_port`: udp socket port  
  - `ui_skip_intro`: disables the splashscreens on tpp  
  - `net_server_logging`: logs and dumps server requests/responses to the disk  
  - `net_custom_server_tpp`: custom backend server url for tpp  
  - `net_custom_server_mgo`: custom backend server url for mgo  
    - When using the custom server flag the game save is written/read from this folder `tpp-mod/steam_storage/server-{server hash}` instead of `steamuserdata`
  - `lua_logging`: enables lua logging (1: files, 2: debug prints)
  - `lua_dump`: enables lua script dumping
  - `net_channel`: sets the steam networking channel to use (mgo only)
  - `player_ramble_speed_scale`: scales the player ramble speed
  - `player_ramble_speed_patch`: enables the player ramble speed patch for unlocked fps
  - `chat_time`: chat message duration
  - `chat_key`: key to open the chat
  - `chat_enable`: enable chat
  - `discord_enable`: enable discord rpc
  
- **commands**:
  - mgo only:
    - `status`: prints list of players present in the current session (mgo)  
    - `kick <name|steam_id|index>`: kicks a player from your session (mgo)  
    - `clearkicks`: clears the kicked player list (mgo)
    - `connect_lobby <lobby_id>`: connects to a lobby
    - `chatall`: opens text chat
    - `mute <name|steam_id|index>`: locally mutes a player from text chat
    - `unmute <name|steam_id|index>`: umutes a player from text chat
    - `clearmutes`: unmutes all players
    - `clearchat`: clears the text chat
    - `matchrotate`: rotates the match to the next slot
    - `matchstart`: starts a match
    - `matchprint`: prints match settings in the console
    - `matchset <name> <value>`: sets a match setting
    - `matchsetrule <name> <value>`: sets a match rule
    - `matchsetslot <slot index> <name> <value>`: sets a match slot rule
    - `disconnect`: disconnects from a match
  - `var_list`: returns list of registered variables
  - `startsound <id>`: starts playing the specified sound
  - `stopsound <id>`: stops playing the specified sound
  - `set <var name> <var value>`: creates or sets a var

- Metal Gear Online:
  - Text chat ([video](https://www.youtube.com/watch?v=wlzHqPy7XYA))
  - Lobby host commands/tools
  - Dedicated server support (experimental)

- Discord rich presence (displays current mission/location or match)  
