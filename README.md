# tpp-mod

## Download

[dinput8.dll](https://github.alicent.cat/tpp-mod/dinput8.dll)  
place it in the game folder

## Features

- Allows overriding of existing lua scripts
- **variables**:  
  similar to cod dvars (can be set through the console or through the config file `localappdata/tpp-mod/config.json`:  
  - `com_unlock_fps`: unlocks fps past 60  
  - `com_worker_count`: changes maximum number of worker threads for rendering  
  - `net_udp`: makes sessions use udp sockets instead of steam networking (only works on tpp with a custom server backend)  
  - `net_port`: udp socket port  
  - `ui_skip_intro`: disables the splashscreens on tpp  
  - `net_server_logging`: logs and dumps server requests/responses to the disk  
  - `net_custom_server_tpp`: custom backend server url for tpp  
  - `net_custom_server_mgo`: custom backend server url for mgo  
    - When using the custom server flag the game save is written/read from this folder `tpp-mod/steam_storage/server-{server hash}` instead of `steamuserdata`
  
- **commands**:  
  - `status`: prints list of players present in the current session (mgo)  
  - `kick <client index>`: kicks a player from your session (mgo)  
  - `clearkicks`: clears the kicked player list (mgo)
  - `var_list`: returns list of registered variables

working directory is `[mgsv game folder]/tpp-mod`
