# Tichu - Team SALT´D

##  How to
### 1. Compile Code
1. In the tichu-project file, `mkdir build` create your build folder 
2. Go into your build folder `cd build`
3. Use cmake to build the make file `cmake ..`
4. Build everything you need with `make`

### 2. Run the Game
1. go into your build folder `cd build`
2. Run server `./Tichu-server`
3. Run `./Tichu-client` in 4 new consoles



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






## 1. Compile instructions
This project only works on UNIX systems (Linux / MacOS). We recommend using [Ubuntu](https://ubuntu.com/#download), as it offers the easiest way to setup wxWidgets. Therefore, we explain installation only for Ubuntu systems. The following was tested on a Ubuntu 20.4 system, but should also work for earlier versions of Ubuntu.

**Note:** If you create a virtual machine, we recommend to give the virtual machine **at least 12GB** of (dynamic) harddrive space (CLion and wxWidgets need quite a lot of space).

### 1.1 Prepare OS Environment

#### Ubuntu 20.4
The OS should already have git installed. If not, you can use: 
`sudo apt-get install git`

Then use  `git clone` to fetch this repository.

Execute the following commands in a console:
1. `sudo apt-get update`
2. `sudo apt-get install build-essential` followed by `sudo reboot`
3. if on virtual machine : install guest-additions (https://askubuntu.com/questions/22743/how-do-i-install-guest-additions-in-a-virtualbox-vm) and then `sudo reboot`
4. `sudo snap install clion --classic` this installs the latest stable CLion version
5. `sudo apt-get install libwxgtk3.0-gtk3-dev` this installs wxWidgets (GUI library used in this project)


### 1.2 Compile Code
1. Open Clion
2. Click `File > Open...` and there select the **/sockpp** folder of this project
3. Click `Build > Build all in 'Debug'`
4. Wait until sockpp is compiled (from now on you never have to touch sockpp again ;))
5. Click `File > Open...` select the **/cse-tichu-example-project** folder
6. Click `Build > Build all in 'Debug'`
7. Wait until Tichu-server, Tichu-client and Tichu-tests are compiled

## 2. Run the Game
1. Open a console in the project folder, navigate into "cmake-build-debug" `cd cmake-build-debug`
2. Run server `./Tichu-server`
3. Run `./Tichu-client` in 4 new consoles for the 4 players needed for a game

## 3. Run the Unit Tests
1. CLion should automatically create a Google Test configuration Tichu-tests which will run all tests. See [Google Test run/debug configuration﻿](https://www.jetbrains.com/help/clion/creating-google-test-run-debug-configuration-for-test.html#gtest-config) for more information.
2. From the list on the main toolbar, select the configuration Tichu-tests.
3. Click ![run](https://resources.jetbrains.com/help/img/idea/2021.1/artwork.studio.icons.shell.toolbar.run.svg) or press `Shift+F10`.
   
You can run individual tests or test suites by opening the corresponding file in the **/unit-tests** folder and clicking ![run](https://resources.jetbrains.com/help/img/idea/2021.1/artwork.studio.icons.shell.toolbar.run.svg) next to the test method or class. For more information on testing in CLion read the [documentation](https://www.jetbrains.com/help/clion/performing-tests.html).

## 4. Code Documentation

For documentation we use doxygen ... TODO
