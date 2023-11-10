#ifndef TICHU_CLIENT_MSG_H
#define TICHU_CLIENT_MSG_H

#include <string>
#include <unordered_map>
#include <variant>
#include "../../../rapidjson/include/rapidjson/document.h"
#include "../serialization/serializable.h"
#include "../exceptions/TichuException.h"
#include "../game_state/cards/card.h"

// helper type for the visitor
// visit [https://en.cppreference.com/w/cpp/utility/variant/visit] for more info
template<class... Ts>
struct overloaded : Ts... { using Ts::operator()...; };
// explicit deduction guide (not needed as of C++20)
template<class... Ts>
overloaded(Ts...) -> overloaded<Ts...>;

// Identifier for the different request types.
// The ClientMsgType is sent with every client_msg to identify the type of client_msg
// during deserialization on the server side.
enum ClientMsgType {
    join_game,
    start_game,
    play_card,
    fold,
};

// Every client message can have a struct that holds data for this type of message
struct start_game_req {};
struct fold_req {};

struct join_game_req {
    std::string player_name;
};

struct play_card_req {
    card played_card;
};




// this type can hold only one of these structs at any given time
using client_msg_variant = std::variant<join_game_req, start_game_req, play_card_req, fold_req>;


// maps the given msg_variant to the corresponding enum
static ClientMsgType request_to_request_type(const client_msg_variant &var) {
    return
            std::visit(overloaded {
                    [](const join_game_req&) { return ClientMsgType::join_game; },
                    [](const start_game_req&) { return ClientMsgType::start_game; },
                    [](const play_card_req&) { return ClientMsgType::play_card; },
                    [](const fold_req&) { return ClientMsgType::fold; },
                    [] (auto) { throw TichuException("client_msg_variant could not be turned into ClientMsgType"); },
            }, var);
}

// maps the enum to it's string
const std::vector<std::pair<ClientMsgType, const char *>>  msg_type_to_string_map {
        {ClientMsgType::join_game,  "join_game"},
        {ClientMsgType::start_game, "start_game"},
        {ClientMsgType::play_card,  "play_card"},
        {ClientMsgType::fold,       "fold"},
};

// a client_msg has 3 fields, the game_id, player_id and the data specific for that type of message
class client_msg : public serializable {
protected:

    client_msg_variant _request;
    UUID _player_id;
    UUID _game_id;

public:

    explicit client_msg(UUID player_id, UUID game_id, client_msg_variant ); // base constructor

    static std::string msg_type_to_string(ClientMsgType type);

    // gets the type of the current message. the function request_to_request_type is used
    [[nodiscard]] ClientMsgType get_type() const;
    [[nodiscard]] const UUID &get_game_id() const { return this->_game_id; }
    [[nodiscard]] const UUID &get_player_id() const { return this->_player_id; }

    template <typename T>
    T get_msg_data() const {
        return std::get<T>(_request);
    }

    // Tries to create the specific client_msg from the provided json.
    // Throws exception if parsing fails -> Use only in "try{ }catch()" block
    static client_msg from_json(const rapidjson::Value& json);

    // Serializes the client_msg into a json object that can be sent over the network
    void write_into_json(rapidjson::Value& json, rapidjson::Document::AllocatorType& alloc) const override;

    [[nodiscard]] virtual std::string to_string() const;
};


#endif //TICHU_CLIENT_MSG_H
