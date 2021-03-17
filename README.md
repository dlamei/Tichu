# Lama

## 1. Compile instructions
This project only works on UNIX systems (Linux / MacOS). We recommend using Ubuntu 20.4.

**Note:** If you create a virtual machine, we recommend to give the virtual machine at least 15GB of (dynamic) harddrive space (CLion and wxWidgets need quite a lot of space).

### 1.1 Prepare OS Environment
#### Ubuntu 20.4
Execute the following commands in a console
1. `sudo apt-get update`
2. `sudo apt-get install build-essentials` followed by `sudo restart`
3. if on virtual machine : install guest-additions (https://askubuntu.com/questions/22743/how-do-i-install-guest-additions-in-a-virtualbox-vm) and then `sudo restart`
4. `sudo snap install clion --classic` this installs the latest stable CLion version
5. `sudo apt-get install libwxgtk3.0-gtk3-dev` this installs wxWidgets (GUI library used in this project)

#### MacOS
TODO

### 1.2 Compile Code
1. Open Clion
2. Click `File > Open...` and there select the /sockpp folder of this project
3. Click `Build > Build all in 'Debug'`
4. Wait until sockpp is compiled
5. Click `File > Open...` select the /cse-lama-example-project folder
6. Click `Build > Build all in 'Debug'`
7. Wait until Lama-server and Lama-client are compiled

## 2. Run the Game
1. Open console in the project folder, navigate into "cmake-build-debug" `cd cmake-build-debug`
2. Run server `./Lama-server`
3. Run as many clients as you want players `./Lama-client`

## 3. Code Documentation
The code can be found in /src, where it is separated into different folders:
- **/client** contains only code that is used on the client side (e.g. UI, sending messages)
- **/common** contains helper functions that are used on the client and server side. You don't need to change anything in here.
- **/game_state** contains the game state that is synchronized between client and server. We use the pre-compile directive LAMA_SERVER to enable certain parts of the code only on the server side. Namely these are the state update functions, as this should only happen on the server. The client then simply reflects the current game state. 
- **/network** contains all the messages that are being passed between client and server. We use the pre-compile directive LAMA_SERVER to enable execution of a `client_request` on the server side (through the function `execute()`). Similarly, we use the LAMA_CLIENT pre-compile directive to make `server_repsonses` only executable on the client side (through the function `process()`) .
- **/reactive_state** contains base classes for the game_state objects. You don't need to change anything in here.
- **/server** contains only code that is relevant fo the server (e.g. player management, receiving messages)

### 3.1 Network Interface
Everything that is passed between client and server are objects of type `client_request` and `server_response`. Since the underlying network protocol works with TCP, these `client_request` and `server_response` objects are transformed into a JSON string, which can then be sent byte by byte over the network. The receiving end reads the JSON string and constructs an object of type `client_request` resp. `server_response` that reflects the exact parameters that were specified in the JSON string. This process is known as **serialization** (object to string) and **deserialization** (string to object).

#### 3.1.1 Serialization & Deserialization of messages
Therefore, both, the `client_request` and `server_response` base classes, implement the abstract class `serializable` with its `write_into_json(...)` function. Additionally, they have a static function from_json(...)

```cpp
class client_request : public serializable {
protected:
    RequestType _type;   // stores the type of request, such that the receiving end knows how to deserialize it
    std::string _req_id; // unique id of this request
    std::string _player_id; // id of the player sending the request
    std::string _game_id;   // id of the game this request is for

    ...
public:
    // DESERIALIZATION: Attempts to create the specific client_request from the provided json.
    // Throws exception if parsing fails -> Use only in "try{ }catch()" block
    static client_request* from_json(const rapidjson::Value& json);

    // SERIALIZATION: Serializes the client_request into a json object that can be sent over the network
    virtual void write_into_json(rapidjson::Value& json, rapidjson::Document::AllocatorType& allocator) const override;

// Code that should only exist on the server side
#ifdef LAMA_SERVER
    // Executes the client_request (only on server)
    // This function is automatically invoked by the server_network_manager when a valid client_request arrived.
    virtual server_response* execute() = 0;
#endif
};
```

When you implement your own specializations of `client_request` and `server_response` you will have to implement the `write_into_json(...)` functions yourself. The subclass always has to call the `write_into_json(...)` function of its base class, such that the parameters of the base class are also written into the JSON document: 

```cpp
// Implementation in the base class client_request
void client_request::write_into_json(rapidjson::Value &json,
                                     rapidjson::MemoryPoolAllocator<rapidjson::CrtAllocator> &allocator) const {
    // Look up string value of this client_request's RequestType and store it in the json document
    rapidjson::Value type_val(_request_type_to_string.at(this->_type).c_str(), allocator);
    json.AddMember("type", type_val, allocator);

    // Save player_id in the JSON document
    rapidjson::Value player_id_val(_player_id.c_str(), allocator);
    json.AddMember("player_id", player_id_val, allocator);

    // Save game_id in the JSON document
    rapidjson::Value game_id_val(_game_id.c_str(), allocator);
    json.AddMember("game_id", game_id_val, allocator);
    ...
}

// Implementation in the subclass play_card_request 
void play_card_request::write_into_json(rapidjson::Value &json,
                                        rapidjson::MemoryPoolAllocator<rapidjson::CrtAllocator> &allocator) const {
    // call base class, such that the parameters of the base class are written into the JSON document
    client_request::write_into_json(json, allocator);

    // Add parameters to the JSON that are unique to the play_card_request
    rapidjson::Value card_id_val(_card_id.c_str(), allocator);
    json.AddMember("card_id", card_id_val,allocator);
}
```

There are plenty of examples of subclasses in the network/requests folder, where you can see how the serialization/deserialization scheme works.

#### 3.1.2 Sending messages
#### Client -> Server:
All you have to do is create an object of type `ClientNetworkThread` on the client side and then invoke its `sendRequest(const client_request& request)` with the client_request that you want to send. The response will arrive as an object of type `request_response` and the `ClientNetworkThread` will invoke the `Process()` function of that `request_response` object automatically.

#### Server -> Client:
If you look at the signature of the `Execute()` function of the `client_request`, you can see that it returns a pointer to an object of type `server_response`. When implementing the `Execute()` function of your `client_request` subclass, you only have to return an object of type `request_response`, which will then automatically be sent over the network to the requesting client. 

If the `client_request` caused an update of the game_state you should also update all other players of that game about the game_state change. This happens in the game_instance class, here examplified at the case where a start_game_request is executed on the server side:

```cpp
bool game_instance::start_game(player* player, std::string &err) {
    modification_lock.lock();   // make sure only one request can modify the game_state at a time

    // Try to start the game
    if (_game_state->start_game(err)) { 
        // create a full_state_response (subclass of server_response) with the full game_state inside
        full_state_response state_update_msg = full_state_response(this->get_id(), *_game_state);
        // send new game_state to all other players
        server_network_manager::broadcast_message(state_update_msg, _game_state->get_players(), player);

        modification_lock.unlock(); // allow other threads to modify the game_state
        return true;
    }
    modification_lock.unlock();
    return false;
}
```

TODO
