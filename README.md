# Tichu - Team SALT´D

# Tichu

This is a simple C++ implementation of the game "Tichu" by AMIGO. You can read the game's rules [here](https://www.amigo.games/content/ap/rule/19420--031-2019-Tichu_Manual_002_LAYOUT[1].pdf). The implementation features a client/server architecture for multiplayer scenarios.
It uses [Dear ImGui](https://github.com/ocornut/imgui) for the GUI, [sockpp](https://github.com/fpagliughi/sockpp) for the network interface, [nlohmann/json](https://github.com/nlohmann/json) for object serialization, and [googletest](https://github.com/google/googletest) for the unit tests. 

<div align="center">

<img src="./assets/tichu_logo.png" alt="Tichu Logo" width="200px"/>

</div>

## Quick guide of the GUI

### 1. Login Screen

After starting the client, the first screen is going to be the login screen. Here every player enters:
- the **server address**
- the **server port**
- their **player name**
- their **preferred team** (Team A , Team B or a random selection)

When all details are entered, the player clicks on the *CONNECT*-button. If the player belongs to the first 3 players joining, they will be forwarded to the game lobby.

![Login-Screen](./assets/login_screen.jpeg?raw=true)






## 2. Compile instructions
This project should compile on Linux, Windows and MacOS. It was mainly tested on Ubuntu  20/22 and Windows 10/11.

## 2.1 Requirements:
- C++20
- OpenGL 3.2+
- CMake 3.15+

## 2.2 Build
```
mkdir build
cd build
cmake ..
make
```

## 3. Code Documentation

For documentation we use doxygen ... TODO
