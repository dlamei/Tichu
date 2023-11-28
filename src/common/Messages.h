#ifndef TICHU_MESSAGES_H
#define TICHU_MESSAGES_H

#include <string>
#include <unordered_map>
#include <utility>
#include <variant>
#include "utils.h"
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
    play_combi,
    fold,
};

// Every client message can have a struct that holds data for this type of message
struct start_game_req {};
struct fold_req {};
struct join_game_req { std::string player_name; };
struct play_combi_req { CardCombination played_combi; };

// this type can hold only one of these structs at any given time
using client_msg_variant = std::variant<join_game_req, start_game_req, play_combi_req, fold_req>;

VARIANT_ENUM(play_combi_req, ClientMsgType, play_combi)
VARIANT_ENUM(join_game_req, ClientMsgType, join_game)
VARIANT_ENUM(fold_req, ClientMsgType, fold)
VARIANT_ENUM(start_game_req, ClientMsgType, start_game)

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(join_game_req, player_name)
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(play_combi_req, played_combi)
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_EMPTY(start_game_req)
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_EMPTY(fold_req)

// a ClientMsg has 3 fields, the game_id, player_id and the data specific for that type of message
class ClientMsg {
private:

    // additional data depending on the msg type
    client_msg_variant _data{};
    UUID _player_id;
    UUID _game_id;

public:

    explicit ClientMsg(UUID player_id, UUID game_id, client_msg_variant var)
    : _data(std::move(var)), _player_id(std::move(player_id)), _game_id(std::move(game_id)) {}
    ClientMsg() = default;

    // gets the type of the current message. the function request_to_request_type is used
    [[nodiscard]] ClientMsgType get_type() const { return std::visit([&](auto&& var) { return get_variant_enum(var); }, _data); }

    [[nodiscard]] const UUID &get_game_id() const { return this->_game_id; }

    [[nodiscard]] const UUID &get_player_id() const { return this->_player_id; }

    template<typename T>
    T get_msg_data() const {
        return std::get<T>(_data);
    }

    NLOHMANN_DEFINE_TYPE_INTRUSIVE(ClientMsg, _data, _player_id, _game_id);
};


// SERVER -> CLIENT

// Identifier for the different response types.
// The ServerMsgType is sent with every ServerMsg to identify the type of ServerMsg
// during deserialization on the client side.
enum class ServerMsgType: int {
    req_response = 0,
    full_state
};

struct request_response {
    bool success{};
    std::optional<GameState> state;
    std::string err;
};
struct full_state_response { GameState state; };
using server_msg_variant = std::variant<request_response, full_state_response>;

VARIANT_ENUM(request_response, ServerMsgType, req_response)
VARIANT_ENUM(full_state_response, ServerMsgType, full_state)

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(request_response, success, state, err)
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(full_state_response, state)


class ServerMsg {
private:

    // additional data depending on the type of message
    server_msg_variant _data;
    UUID _game_id;

public:
    explicit ServerMsg(UUID  game_id, server_msg_variant var)
    : _game_id(std::move(game_id)), _data(std::move(var)) {}
    ServerMsg() = default;

    [[nodiscard]] ServerMsgType get_type() const { return std::visit(overloaded{[](const auto &r) { return get_variant_enum(r); }}, _data); }
    [[nodiscard]] const UUID &get_game_id() const { return _game_id; }

    template<typename T>
    T get_msg_data() const {
        return std::get<T>(_data);
    }

    NLOHMANN_DEFINE_TYPE_INTRUSIVE(ServerMsg, _data, _game_id);
};

#endif //TICHU_MESSAGES_H
