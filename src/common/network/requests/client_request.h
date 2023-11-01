//
// Created by Manuel on 28.01.2021.
//
// client_request is the base-class for all requests that are being sent from the client to the server.
// It offers a function to deserialize a client_request subclass from a valid json.

#ifndef TICHU_CLIENT_REQUEST_H
#define TICHU_CLIENT_REQUEST_H

#include <string>
#include <unordered_map>
#include <variant>
#include "../../../../rapidjson/include/rapidjson/document.h"
#include "../../serialization/serializable.h"
#include "../../exceptions/TichuException.h"
#include "../../serialization/uuid_generator.h"
#include "../../serialization/json_utils.h"

#include "join_game_request.h"
#include "start_game_request.h"
#include "play_card_request.h"
#include "draw_card_request.h"
#include "fold_request.h"

// Identifier for the different request types.
// The RequestType is sent with every client_request to identify the type of client_request
// during deserialization on the server side.
//TODO: make field of every request
enum RequestType {
    join_game,
    start_game,
    play_card,
    draw_card,
    fold,
};
using request_variant = std::variant<join_game_request, start_game_request, play_card_request, draw_card_request, fold_request>;


class client_request : public serializable {
protected:

    struct base_class_properties {
        RequestType type;
        UUID req_id;
        UUID player_id;
        UUID game_id;
    };


    request_variant _request;
    base_class_properties _props;

    explicit client_request(base_class_properties, request_variant); // base constructor
    static base_class_properties extract_base_class_properties(const rapidjson::Value& json);
    virtual const base_class_properties &get_properties() { return _props; }

private:

    // for deserialization
    static const std::unordered_map<std::string, RequestType> _string_to_request_type;
    // for serialization
    static const std::unordered_map<RequestType, std::string> _request_type_to_string;

public:

    explicit client_request(UUID player_id, UUID game_id, const request_variant&); // base constructor

    [[nodiscard]] RequestType get_type() const { return this->_props.type; }
    [[nodiscard]] const UUID &get_req_id() const { return this->_props.req_id; }
    [[nodiscard]] const UUID &get_game_id() const { return this->_props.game_id; }
    [[nodiscard]] const UUID &get_player_id() const { return this->_props.player_id; }

    template <typename T>
    T get_request() const {
        return std::get<T>(_request);
    }

    // Tries to create the specific client_request from the provided json.
    // Throws exception if parsing fails -> Use only in "try{ }catch()" block
    static client_request from_json(const rapidjson::Value& json);

    // Serializes the client_request into a json object that can be sent over the network
    void write_into_json(rapidjson::Value& json, rapidjson::Document::AllocatorType& allocator) const override;

    [[nodiscard]] virtual std::string to_string() const;
};


#endif //TICHU_CLIENT_REQUEST_H
