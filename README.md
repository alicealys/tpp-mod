# tpp-mod

## Download

[dinput8.dll](https://github.alicent.cat/tpp-mod/dinput8.dll)

## Features

- Allows overriding of existing lua scripts
- Flags:
  - `-custom-server-tpp {url}`: modifies konami server url for tpp
  - `-custom-server-mgo {url}`: modifies konami server url for mgo
  - `-server-logging`: enables server response logging to disk
  - `-lua-debug`: enables lua debug print
- When using the custom server flag the game save is written/read from this folder `tpp-mod/steam_storage/server-{server hash}` instead of `steamuserdata`

working directory is `[mgsv game folder]/tpp-mod`
