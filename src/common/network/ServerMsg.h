// Base class for all messages sent from the server to the client.
// It offers a function to deserialize a ServerMsg subclass from a valid json.

#ifndef TICHU_SERVERMSG_H
#define TICHU_SERVERMSG_H

#include <string>
#include <unordered_map>
#include <variant>

#include "../serialization/serializable.h"
#include "../game_state/GameState.h"

struct request_response {
    bool success;
    std::optional<GameState> state;
    std::string err;
};

struct full_state_response {
    GameState state;
};

using server_msg_variant = std::variant<request_response, full_state_response>;

// Identifier for the different response types.
// The ServerMsgType is sent with every ServerMsg to identify the type of ServerMsg
// during deserialization on the client side.
enum class ServerMsgType: int {
    req_response = 0,
    full_state_response
};


class ServerMsg : public serializable {
protected:

    server_msg_variant _response;
    UUID _game_id;

public:
    explicit ServerMsg(UUID game_id, server_msg_variant); // base constructor

    [[nodiscard]] ServerMsgType get_type() const;
    [[nodiscard]] const UUID &get_game_id() const { return _game_id; }

    template<typename T>
    T get_msg_data() const {
        return std::get<T>(_response);
    }

    // Tries to create the specific ServerMsg from the provided json.
    // Throws exception if parsing fails -> Use only inside "try{ }catch()" block
    static ServerMsg from_json(const rapidjson::Value &json);

    // Serializes the ServerMsg into a json object that can be sent over the network
    void write_into_json(rapidjson::Value &json, rapidjson::Document::AllocatorType &allocator) const override;
};


#endif //TICHU_SERVERMSG_H
