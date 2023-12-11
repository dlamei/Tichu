#ifndef TICHU_MESSAGES_H
#define TICHU_MESSAGES_H

#include <string>
#include <unordered_map>
#include <utility>
#include <variant>
#include "utils.h"
#include "Event.h"
#include "game_state/GameState.h"
#include "game_state/cards/card.h"
#include "game_state/cards/CardCombination.h"


#define VARIANT_ENUM(self_t, enum_t, value) inline enum_t get_variant_enum(const self_t &) { return enum_t::value; }

//     CLIENT -> SERVER

// Identifier for the different request types.
// The ClientMsgType is sent with every ClientMsg to identify the type of ClientMsg
// during deserialization on the server side.
enum class ClientMsgType: int {
    join_game = 0,
    start_game,
    call_grand_tichu,
    play_combi,
    fold,
    ping,
};

// Every client message can have a struct that holds data for this type of message
struct start_game_req {};
VARIANT_ENUM(start_game_req, ClientMsgType, start_game)
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_EMPTY(start_game_req)

struct fold_req {};
VARIANT_ENUM(fold_req, ClientMsgType, fold)
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_EMPTY(fold_req)

struct join_game_req { 
    std::string player_name; 
    int team;
    };
VARIANT_ENUM(join_game_req, ClientMsgType, join_game)
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(join_game_req, player_name, team)

struct grand_tichu_req { Tichu grand_tichu_call; };
VARIANT_ENUM(grand_tichu_req, ClientMsgType, call_grand_tichu)
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(grand_tichu_req, grand_tichu_call)

struct play_combi_req { CardCombination played_combi; };
VARIANT_ENUM(play_combi_req, ClientMsgType, play_combi)
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(play_combi_req, played_combi)

struct ping {};
VARIANT_ENUM(ping, ClientMsgType, ping)
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_EMPTY(ping)

// this type can hold only one of these structs at any given time
using client_msg_variant = std::variant<join_game_req, start_game_req, grand_tichu_req, play_combi_req, fold_req, ping>;

// a ClientMsg has 3 fields, the game_id, player_id and the data specific for that type of message
class ClientMsg {
private:

    // additional data depending on the msg type
    client_msg_variant _data{};
    UUID _player_id;

public:

    explicit ClientMsg(UUID player_id, client_msg_variant var)
    : _data(std::move(var)), _player_id(std::move(player_id)) {}
    ClientMsg() = default;

    // gets the type of the current message. the function request_to_request_type is used
    [[nodiscard]] ClientMsgType get_type() const { return std::visit([&](auto&& var) { return get_variant_enum(var); }, _data); }

    //[[nodiscard]] const UUID &get_game_id() const { return this->_game_id; }

    [[nodiscard]] const UUID &get_player_id() const { return this->_player_id; }

    template<typename T>
    T get_msg_data() const {
        const int i1 = (int)get_variant_enum(T{});
        const int i2 = (int)get_type();
        ASSERT(i1 == i2, "called get_msg_data for variant {}, but found {}", i1, i2);
        return std::get<T>(_data);
    }

    NLOHMANN_DEFINE_TYPE_INTRUSIVE(ClientMsg, _data, _player_id);
};


// SERVER -> CLIENT

// Identifier for the different response types.
// The ServerMsgType is sent with every ServerMsg to identify the type of ServerMsg
// during deserialization on the client side.
enum class ServerMsgType: int {
    req_response = 0,
    event,
    full_state,
    pong,
};

enum class MessageType: int {
    Info,
    Warn,
    Error,
};

struct server_message {
    MessageType type{};
    std::string msg{};
};
VARIANT_ENUM(server_message, ServerMsgType, req_response)
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(server_message, msg, type)

struct full_state_response { GameState state; };
VARIANT_ENUM(full_state_response, ServerMsgType, full_state)
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(full_state_response, state)

struct event_message { Event event; };
VARIANT_ENUM(event_message, ServerMsgType, event)
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(event_message, event)

struct pong {};
VARIANT_ENUM(pong, ServerMsgType, pong)
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_EMPTY(pong)

using server_msg_variant = std::variant<server_message, event_message, full_state_response, pong>;

class ServerMsg {
private:

    // additional data depending on the type of message
    server_msg_variant _data;

public:
    explicit ServerMsg(server_msg_variant var)
    : _data(std::move(var)) {}
    ServerMsg() = default;

    [[nodiscard]] ServerMsgType get_type() const { return std::visit(overloaded{[](const auto &r) { return get_variant_enum(r); }}, _data); }

    template<typename T>
    T get_msg_data() const {
        const int i1 = (int)get_variant_enum(T{});
        const int i2 = (int)get_type();
        ASSERT(i1 == i2, "called get_msg_data for variant {}, but found {}", i1, i2);
        return std::get<T>(_data);
    }

    NLOHMANN_DEFINE_TYPE_INTRUSIVE(ServerMsg, _data);
};

#endif //TICHU_MESSAGES_H
