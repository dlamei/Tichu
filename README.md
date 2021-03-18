# Lama

This is a simple C++ implementation of the game "Lama" by AMIGO. The implementation features a client/server architecture for multiplayer scenarios.
It uses [wxWidgets](https://www.wxwidgets.org/) for the GUI, [sockpp](https://github.com/fpagliughi/sockpp) for the network interface and [rapidjson](https://rapidjson.org/md_doc_tutorial.html) for object serialization.

This is a template project for the students of the course Software Engineering. We encourage the students to read through this documentation and get familiar with [wxWidgets](https://www.wxwidgets.org/) and the [rapidjson library](https://rapidjson.org/md_doc_tutorial.html) if they want to implement their game based on the architecture of this template. They can reuse as much of the code as they want and we encourage to at least reuse the `server_network_manager` and `client_network_manager`, which handles TCP communication between client and server.

## 1. Compile instructions
This project only works on UNIX systems (Linux / MacOS). We recommend using [Ubuntu](https://ubuntu.com/#download), as it offers the easiest way to setup wxWidgets. Therefore, we explain installation only for Ubuntu systems. The following was tested on a Ubuntu 20.4 system, but should also work for earlier versions of Ubuntu.

**Note:** If you create a virtual machine, we recommend to give the virtual machine at least 12GB of (dynamic) harddrive space (CLion and wxWidgets need quite a lot of space).

### 1.1 Prepare OS Environment

#### Ubuntu 20.4
The OS should alredy have git installed in order to download this repository. If not, you can use: 
`sudo apt-get install git`

Execute the following commands in a console:
1. `sudo apt-get update`
2. `sudo apt-get install build-essentials` followed by `sudo reboot`
3. if on virtual machine : install guest-additions (https://askubuntu.com/questions/22743/how-do-i-install-guest-additions-in-a-virtualbox-vm) and then `sudo reboot`
4. `sudo snap install clion --classic` this installs the latest stable CLion version
5. `sudo apt-get install libwxgtk3.0-gtk3-dev` this installs wxWidgets (GUI library used in this project)


### 1.2 Compile Code
1. Open Clion
2. Click `File > Open...` and there select the /sockpp folder of this project
3. Click `Build > Build all in 'Debug'`
4. Wait until sockpp is compiled
5. Click `File > Open...` select the /cse-lama-example-project folder
6. Click `Build > Build all in 'Debug'`
7. Wait until Lama-server and Lama-client are compiled

## 2. Run the Game
1. Open a console in the project folder, navigate into "cmake-build-debug" `cd cmake-build-debug`
2. Run server `./Lama-server`
3. In new consoles run as many clients as you want players `./Lama-client`

## 3. Code Documentation
The code can be found in **/src**, where it is separated into different folders:
- **/client** contains only code that is used on the client side (e.g. UI, sending messages)
- **/common** contains helper functions that are used on the client and server side. You don't need to change anything in here (unless you want to rename the LamaException class ;)).
- **/game_state** contains the game state that is synchronized between client and server. We use the pre-compile directive LAMA_SERVER to enable certain parts of the code only on the server side. Namely, these are the state update functions, as they should only happen on the server. The client  simply reflects the current game state as sent by the server without modifying it directly. 
- **/network** contains all the messages that are being passed between client and server. We use the pre-compile directive LAMA_SERVER to enable execution of a `client_request` on the server side (through the function `execute()`). Similarly, we use the LAMA_CLIENT pre-compile directive to make `server_repsonses` only executable on the client side (through the function `Process()`) .
- **/reactive_state** contains base classes for the `game_state` objects. You don't need to change anything in here.
- **/server** contains only code that is relevant for the server (e.g. player management, game instance management, receiving messages)

### 3.1 Network Interface
Everything that is passed between client and server are objects of type `client_request` and `server_response`. Since the underlying network protocol works with TCP, these `client_request` and `server_response` objects are transformed into a **[JSON](https://wiki.selfhtml.org/wiki/JSON) string**, which can then be sent byte by byte over the network. The receiving end reads the JSON string and constructs an object of type `client_request` resp. `server_response` that reflects the exact parameters that are specified in the JSON string. This process is known as **serialization** (object to string) and **deserialization** (string to object).

![client-server-diagram](./docs/img/client-server-diagram.png?raw=true)

#### 3.1.1 Serialization & Deserialization of messages
Therefore, both, the `client_request` and `server_response` base classes, implement the abstract class `serializable` with its `write_into_json(...)` function. Additionally, they have a static function `from_json(...)`, which allows creating an instance of an object from JSON.

```cpp
enum RequestType {
    join_game,
    start_game,
    play_card,
    draw_card,
    fold,
};

class client_request : public serializable {
protected:
    RequestType _type;   // stores the type of request, such that the receiving end knows how to deserialize it
    std::string _req_id; // unique id of this request
    std::string _player_id; // id of the player sending the request
    std::string _game_id;   // id of the game this request is for

    ...
private:
    // for deserialization
    static const std::unordered_map<std::string, RequestType> _string_to_request_type;
    // for serialization
    static const std::unordered_map<RequestType, std::string> _request_type_to_string;

public:
    // DESERIALIZATION: Attempts to create the specific client_request from the provided json.
    static client_request* from_json(const rapidjson::Value& json);

    // SERIALIZATION: Serializes the client_request into a json object that can be sent over the network
    virtual void write_into_json(rapidjson::Value& json, rapidjson::Document::AllocatorType& allocator) const override;

// Code that should only exist on the server side
#ifdef LAMA_SERVER
    // Executes the client_request (only on server)
    // This function is automatically invoked by the server_network_manager when a valid client_request (this) arrives.
    virtual server_response* execute() = 0;
#endif
};
```

**Serialization**

When you implement your own specializations of `client_request` (and `server_response` if necessary) you will have to implement the `write_into_json(...)` functions yourself. Your subclass always has to call the `write_into_json(...)` function of its base class, such that the parameters of the base class are  written into the JSON document: 

Here is the **base-class** implementation:
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
```
And here is the **subclass** implementation, where an additional field `_card_id` is serialized.
```cpp
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

**Deserialization**

The deserialization of `client_request` JSONs always goes through the `from_json(...)` function of the `client_request` class. In this function, the "type" field, stored in the JSON, is inspected to determine, which subclass should be called to perform the deserialization: 

```cpp
if (json.HasMember("type") && json["type"].IsString()) {
        // Get the RequestType stored as a string in the JSON
        const std::string type = json["type"].GetString();
        // Lookup the actual RequestType per string from a pre-defined unordered_map
        const RequestType request_type = client_request::_string_to_request_type.at(type);

        // Call the correct from_json() specialization
        if (request_type == RequestType::play_card) {
            return play_card_request::from_json(json);
        }
        else if (request_type == RequestType::draw_card) {
            return draw_card_request::from_json(json);
        }
        else if (...) {
            ...
        } else {
            throw LamaException("Encountered unknown ClientRequest type " + type);
        }
    }
    throw LamaException("Could not determine type of ClientRequest. JSON was:\n" + json_utils::to_string(&json));
```

Therefore, when you implement your own `client_request` subclasses, remember to add a new element into the `RequestType` enum to define your new request type. You will also have to add an entry for this new RequestType in the two unordered_maps `_string_to_request_type`, resp. `_request_type_to_string` in the `client_request` base-class. Once this is done, you can add a check for your new `RequestType` element in the `from_json(...)` function of the `client_request` base-class and call the specialized `from_json(...)` function of your subclass from there. 

Also, don't forget to set the correct `RequestType` in the public constructor of your new `client_request` subclass:

```cpp
// Public constructor
play_card_request::play_card_request(std::string game_id, std::string player_id, std::string card_id)
        : client_request(client_request::create_base_class_properties(  // call base-class constructor
                                                    RequestType::play_card, // IMPORTANT: set the RequestType of your subclass
                                                    uuid_generator::generate_uuid_v4(), 
                                                    player_id, 
                                                    game_id) ),
        _card_id(card_id)   // set subclass specific parameters
{ }
```

The deserialization in your subclass will look something like this:

```cpp
// private constructor for deserialization
play_card_request::play_card_request(client_request::base_class_properties props, std::string card_id) :
        client_request(props),  // call base-class constructor
        _card_id(card_id)   // set subclass specific parameters
{ }

// Deserialization
play_card_request* play_card_request::from_json(const rapidjson::Value& json) {
    // extract base-class properties from the json
    base_class_properties props = client_request::extract_base_class_properties(json);

    // get subclass specific properties
    if (json.HasMember("card_id")) {
        // invoke deserialization constructor
        return new play_card_request(props, json["card_id"].GetString());
    } else {
        throw LamaException("Could not find 'card_id' in play_card_request");
    }
}
```

There are plenty of examples of subclasses in the network/requests folder, where you can see how the serialization/deserialization scheme works.

#### 3.1.2 Sending messages
#### Client -> Server:
All you have to do is create an object of type `ClientNetworkThread` on the client side and then invoke its `sendRequest(const client_request& request)` function with the client_request that you want to send. The response will arrive as an object of type `request_response` and the `ClientNetworkThread` will invoke the `Process()` function of that `request_response` object automatically.

#### Server -> Client:
If you look at the signature of the `execute()` function of the `client_request`, you can see that it returns a pointer to an object of type `request_response` (a subclass of `server_response`). When implementing the `execute()` function of your `client_request` subclass, you have to return an object of type `request_response` with all parameters you want to send. This return value will then automatically be sent over the network to the requesting client. 

If the `client_request` caused an update of the game_state you should also update all other players of that game about the game_state change. This happens in the `game_instance` class, here examplified at the case where a `start_game_request` calls the `start_game(...)` function on the respective `game_instance` on the server side:

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

### 3.2 Game State

The `game_state` class stores all parameters that are required to represent the game on the client (resp. server) side. In order to synchronize this `game_state` among all players, the `game_state` can also be **serialized** and **deserialized**. If a `client_request` was successfully executed on the server, then the `request_response` that is sent to back to the client contains a serialized version of the updated `game_state`. All other players receive the updated `game_state` at the same time through a `full_state_response`.

To serialize the `game_state`, the same `write_into_json(...)` function is used as for the `client_request`. 

```cpp
class game_state : public reactive_object {
private:
    // Properties
    std::vector<player*> _players;
    draw_pile* _draw_pile;
    discard_pile* _discard_pile;
    reactive_value<bool>* _is_started;
    reactive_value<bool>* _is_finished;
    reactive_value<int>* _round_number;
    reactive_value<int>* _current_player_idx;
    reactive_value<int>* _play_direction;  // 1 or -1 depending on which direction is played in
    reactive_value<int>* _starting_player_idx;

    // deserialization constructor
    game_state(
            base_params params,
            draw_pile* draw_pile,
            discard_pile* discard_pile,
            std::vector<player*>& players,
            reactive_value<bool>* is_started,
            reactive_value<bool>* is_finished,
            reactive_value<int>* current_player_idx,
            reactive_value<int>* play_direction,
            reactive_value<int>* round_number,
            reactive_value<int>* starting_player_idx);

public:
    game_state();

    ...
    // SERIALIZATION
    virtual void write_into_json(rapidjson::Value& json, rapidjson::Document::AllocatorType& allocator) const override;
    // DESERIALIZATION
    static game_state* from_json(const rapidjson::Value& json);
};
```

The `game_state` inherits from `reactive_object`, which essentially requires the `write_into_json()` function and adds a unique `id` to the object, such that it can be uniquely identified. Similarly, each parameter nested inside the `game_state` (e.g. players, draw_pile, etc.) also inherit from reactive_object and therefore have their own `id` and serialization, resp. deserialization functions.

On the client side, the new `game_state` is then passed to the `updateGameState(game_state*)` function of the `GameController` class, which performs a redraw of the GUI.

Since you will have to add your own properties to the `game_state` class (and probably create other classes that inherit from `reactive_object` to add to your game_state), we want to shortly elaborate how the serialization and deserialization works for subclasses of `reactive_object`. It's very similar to the `client_request` class discussed earlier. Here is how the `write_into_json(...)` function is implemented in the `game_state` class of Lama:

```cpp
void game_state::write_into_json(rapidjson::Value &json,
                                 rapidjson::MemoryPoolAllocator<rapidjson::CrtAllocator> &allocator) const {
    // call base-class to write id and object name into the json
    reactive_object::write_into_json(json, allocator);

    // write all properties of this game_state instance into the JSON
    rapidjson::Value is_finished_val(rapidjson::kObjectType);
    _is_finished->write_into_json(is_finished_val, allocator);
    json.AddMember("is_finished", is_finished_val, allocator);

    rapidjson::Value is_started_val(rapidjson::kObjectType);
    _is_started->write_into_json(is_started_val, allocator);
    json.AddMember("is_started", is_started_val, allocator);

    rapidjson::Value current_player_idx_val(rapidjson::kObjectType);
    _current_player_idx->write_into_json(current_player_idx_val, allocator);
    json.AddMember("current_player_idx", current_player_idx_val, allocator);

    rapidjson::Value play_direction_val(rapidjson::kObjectType);
    _play_direction->write_into_json(play_direction_val, allocator);
    json.AddMember("play_direction", play_direction_val, allocator);

    rapidjson::Value starting_player_idx_val(rapidjson::kObjectType);
    _starting_player_idx->write_into_json(starting_player_idx_val, allocator);
    json.AddMember("starting_player_idx", starting_player_idx_val, allocator);

    rapidjson::Value round_number_val(rapidjson::kObjectType);
    _round_number->write_into_json(round_number_val, allocator);
    json.AddMember("round_number", round_number_val, allocator);

    rapidjson::Value draw_pile_val(rapidjson::kObjectType);
    _draw_pile->write_into_json(draw_pile_val, allocator);
    json.AddMember("draw_pile", draw_pile_val, allocator);

    rapidjson::Value discard_pile_val(rapidjson::kObjectType);
    _discard_pile->write_into_json(discard_pile_val, allocator);
    json.AddMember("discard_pile", discard_pile_val, allocator);

    // Special helper function to serialize vector of pointers
    // The pointers inside the vector have to inherit from serializable (or reactive_object)
    json.AddMember("players", vector_utils::serialize_vector(_players, allocator), allocator);
}
```

For **deserialization**, the `from_json(...)` function is used, which is implemented as follows:

```cpp
// DESERIALIZATION COSNTRUCTOR receives pointers for all its properties and stores them
game_state::game_state(base_params params, draw_pile *draw_pile, discard_pile *discard_pile,
                       std::vector<player *> &players, reactive_value<bool> *is_started,
                       reactive_value<bool> *is_finished, reactive_value<int> *current_player_idx,
                       reactive_value<int> *play_direction, reactive_value<int>* round_number, reactive_value<int> *starting_player_idx)
        : reactive_object(params),  // initialize the reactive_object base-class
          _draw_pile(draw_pile),
          _discard_pile(discard_pile),
          _players(players),
          _is_started(is_started),
          _is_finished(is_finished),
          _current_player_idx(current_player_idx),
          _play_direction(play_direction),
          _round_number(round_number),
          _starting_player_idx(starting_player_idx)
{ }

// DESERIALIZATION 
// returns a pointer to the new game_state
game_state* game_state::from_json(const rapidjson::Value &json) {
    // Make sure the json contains all required information
    if (json.HasMember("is_finished")
        && json.HasMember("is_started")
        && json.HasMember("current_player_idx")
        && json.HasMember("play_direction")
        && json.HasMember("round_number")
        && json.HasMember("starting_player_idx")
        && json.HasMember("players")
        && json.HasMember("draw_pile")
        && json.HasMember("discard_pile"))
    {
        // deserialize all players
        std::vector<player*> deserialized_players;
        for (auto &serialized_player : json["players"].GetArray()) {
            deserialized_players.push_back(player::from_json(serialized_player.GetObject()));
        }
        // Invoke deserialization constructor
        return new game_state(reactive_object::extract_base_params(json),   // extract base_params from JSON
                              draw_pile::from_json(json["draw_pile"].GetObject()),
                              discard_pile::from_json(json["discard_pile"].GetObject()),
                              deserialized_players,
                              reactive_value<bool>::from_json(json["is_started"].GetObject()),
                              reactive_value<bool>::from_json(json["is_finished"].GetObject()),
                              reactive_value<int>::from_json(json["current_player_idx"].GetObject()),
                              reactive_value<int>::from_json(json["play_direction"].GetObject()),
                              reactive_value<int>::from_json(json["round_number"].GetObject()),
                              reactive_value<int>::from_json(json["starting_player_idx"].GetObject()));
    } else {
        throw LamaException("Failed to deserialize game_state. Required entries were missing.");
    }
}
```

A similar scheme is applied in all other objects that inherit from `reactive_object`. Namely, these are:
- `player`
- `hand`
- `card`
- `draw_pile`
- `discard_pile`
- `reactive_value`
