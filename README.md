# tpp-mod

Improvements and fixes for Metal Gear Solid V: The Phantom Pain

discord server
https://discord.gg/hYfW9MEEGF

## Installation

tpp-mod only supports original steam releases (latest).  
Any kind of modified or pirated version is **NOT** supported.   

For older builds go here:
- [23 March 2021](https://github.com/alicealys/tpp-mod/tree/day1820)
- [6 June 2026](https://github.com/alicealys/tpp-mod/tree/day3800)

| version | sha1 | support |
| --- | --- | --- |
| mgsvtpp.exe (eng, 1.0.15.4) | 4F643576C6F6ECF6C67A9AEF4D4F02285ADA43E3 | ✔️ |
| mgsvmgo.exe (eng, 1.1.2.9) | 1582B87C25CC989FE5457B9F1FD90ACACA9BCD89 | ✔️ |
| mgsvtpp.exe (jpn, 1.0.15.4) | 72F63B2E3364A36C2FA0BE360AF17A787FCC14B0 | ❌ |
| mgsvmgo.exe (jpn, 1.1.2.9) | 4F65C0DB3A136325D5C358D01680C4D7DE61E566 | ❌ |

- Download [dinput8.dll](https://github.alicent.cat/tpp-mod/latest/dinput8.dll) and copy it to the game folder.  

Configuration files are stored in `%localappdata%\tpp-mod`

## Features

| <img src="assets/github/1.png?raw=true" /> |
|:-:|
| in game text chat & ping/fps counter |

| <img src="assets/github/2.png?raw=true" /> |
|:-:|
| in game console |

### Security

Various crash/DoS exploits and other vulnerabilities have been patched. 

### Performance

The performance and FPS drop issues (notably the avatar edit menu, but also extends to the rest of the game) that have been introduced in the June 6 2026 update have been fixed.

### Misc
- discord rich presence
- unlock fps and custom fps capping
- fps and ping counter
- mgo text chat
- in game console
- high fps wake up time patch
- custom security challenge list
- kick players from your mgo lobby
- DSX integration
- lua script overriding and custom script loading
- custom backend server support (see: tpp-server-emulator)
- mgo dedicated server support (experimental)
- custom key bindings
- key remapping
- fov scaling
- start mgo directly
- allow multiple instance of the game
- scale mouse sensitivity
- disable intro splashscreens

### Command list
Can be executed from the console or in game console 

- `bind <key> <command>`: bind a key to a command
- `remap <key from> <key to>`: remap a key to another
- `unbind <key>`: remove custom bind from key
- `unbindall`: remove all custom binds
- `exec <cfg name>`: execute a config file 
- `alias <name> <command>`: create a command alias 
- `clear`: clear the ingame console
- `toggleconsole`: toggle the ingame console
- `equip <slot type> <index>`: equip a slot (slot types: 'primary1', 'primary2', 'secondary', 'support', 'item', 'stole', hand')
- `equiptoggle <slot type> <index 1> <index 2>`: toggle between 2 equip slots
- `equipnext <slot type>`: equip the next slot
- `equipprev <slot type>`: equip the previous slot
- `set <var name> <value>`: set a var
- `reset <var name>`: reset a var
- `var_list`: show the list of vars
- `quit`: quit the game
- `startsound <id>`: start playing a sound
- `stopsound <id>`: stop playing the sound
- `wait <frame count>`: wait before executing the next command
- `startmgo`: start mgo
- `starttpp`: start tpp
- `script_var <varname>`: view lua script var
- `script_exec <code>`: execute lua code
- `script_load <path>`: load a lua script
- `disconnect`: return to acc/leave the mgo lobby
- `status`: return list of players in session
- `rtt`: connection rtt in milliseconds
- `session_create`: create a session
- `session_start`: start the session
- `session_accept`: accepts new session connections
- `session_close`: close the session
- `session_connect <steamid>`: connect to a session
- `clearkicks`: clear the kicked players list
- `connect_lobby <lobbyid>`: connect to a lobby
- `reconnect`: disconnect and reconnects to the current lobby
- `matchstart`: start a match
- `matchrotate`: rotate the match to the next slot
- `matchset <name> <value>`: set a match setting
- `matchsetrule <name> <value>`: set a match rule
- `matchsetslot <slot> <name> <value>`: set a match' slot rule
- `kick <name|steamid|index>`: kick a player from the lobby
- `clearchat`: clear the chat
- `chatall`: open the all chat
- `chatteam`: open the team chat
- `say <message>`: send a message to all chat
- `say_team <message>`: send a message to team chat
- `mute <name|steamid|index>`: mute a player
- `unmute <name|steamid|index>`: unmute a player
- `mutelist`: show mutes players
- `clearmutes`: unmute all players

### Variable list
similar to cod dvars, can be set through the console or through the config files `%localappdata%/tpp-mod/config/`
(type their name in the console for a description)

- `net_custom_server`
- `net_channel`
- `discord_enable`
- `con_input_box_color`
- `con_input_hint_box_color`
- `con_output_bar_color`
- `con_output_slider_color`
- `con_output_window_color`
- `con_input_dvar_match_color`
- `con_input_dvar_value_color`
- `con_input_dvar_inactive_value_color`
- `con_input_cmd_match_color`
- `match_enable_tweaks`
- `match_min_players`
- `match_max_players`
- `match_briefing_time`
- `ui_draw_fps`
- `ui_draw_ping`
- `com_worker_count`
- `com_unlock_fps`
- `com_max_fps`
- `sensitivity`
- `camera_fov_scale`
- `camera_first_person_fov_scale`
- `name`
- `dsx_enable`
- `lua_logging`
- `lua_dump`
- `net_server_logging`
- `net_server_heartbeat`
- `chat_enable`
- `chat_time`
- `chat_input_bg`
- `chat_output_bg`
- `chat_slider_color`
- `chat_input_pulse`
- `chat_offset`
- `chat_height`
- `chat_width`
- `chat_scale`
- `chat_direction`
- `chat_muted_players`
- `fob_security_challenge_mode`
- `net_udp`
- `net_port`
- `player_ramble_speed_scale`
- `player_ramble_speed_patch`
- `ui_skip_intro`
- `staff_cheat`

## Launch parameters

- `-mode <mode>`: overriding version detection (modes: 'tppeng', 'mgoeng', 'tppjpn', 'mgojpn')
- `-dedicated`: start mgo headless as dedicated server
- `-var-cheat`: allow to modify cheat only vars
- `-dump-hashes`: stores computed hashes, can be dumped with `dumpstrings` command
