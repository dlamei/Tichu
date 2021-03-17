# Lama

## Compile instructions
This project only works on UNIX systems (Linux / MacOS). We recommend using Ubuntu 20.4.

### Prepare OS Environment
#### Ubuntu 20.4
Execute the following commands in a console
1. >sudo apt-get update
2. >sudo apt-get install build-essentials
>sudo restart
3. if on virtual machine : install guest-additions (https://askubuntu.com/questions/22743/how-do-i-install-guest-additions-in-a-virtualbox-vm) and then restart
4. >sudo snap install clion --classic
5. >sudo apt-get install libwxgtk3.0-gtk3-dev

### Compile Code
1. Open Clion
2. Click "File > Open..." and there select the /sockpp folder
3. When the project is open click "Build > Build all in Debug"
4. Wait until sockpp is compiled
5. Click "File > Open..." select the /cse-lama-example-project folder
6. Click "Build > Build all in Debug"
7. Wait until Lama-server and Lama-client are compiled
