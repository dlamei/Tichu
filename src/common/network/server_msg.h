// Base class for all messages sent from the server to the client.
// It offers a function to deserialize a server_msg subclass from a valid json.

#ifndef TICHU_SERVER_MSG_H
#define TICHU_SERVER_MSG_H

#include <string>
#include <unordered_map>
#include <variant>

#include "../serialization/serializable.h"

struct request_response {
    bool success;
    std::optional<json_value_ptr> state_json;
    std::string err;
};

struct full_state_response {
    json_value_ptr state_json;
};

using server_msg_variant = std::variant<request_response, full_state_response>;

// Identifier for the different response types.
// The ServerMsgType is sent with every server_msg to identify the type of server_msg
// during deserialization on the client side.
enum class ServerMsgType: int {
    req_response = 0,
    full_state_response
};


class server_msg : public serializable {
protected:

    server_msg_variant _response;
    UUID _game_id;

public:
    explicit server_msg(UUID game_id, server_msg_variant); // base constructor

    [[nodiscard]] ServerMsgType get_type() const;
    [[nodiscard]] const UUID &get_game_id() const { return _game_id; }

    template<typename T>
    T get_msg_data() const {
        return std::get<T>(_response);
    }

    // Tries to create the specific server_msg from the provided json.
    // Throws exception if parsing fails -> Use only inside "try{ }catch()" block
    static server_msg from_json(const rapidjson::Value &json);

    // Serializes the server_msg into a json object that can be sent over the network
    void write_into_json(rapidjson::Value &json, rapidjson::Document::AllocatorType &allocator) const override;
};


#endif //TICHU_SERVER_MSG_H
