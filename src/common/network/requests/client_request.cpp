//
// Created by Manuel on 28.01.2021.
//

#include "client_request.h"
#include "play_card_request.h"
#include "draw_card_request.h"
#include "fold_request.h"
#include "join_game_request.h"
#include "start_game_request.h"

#include <iostream>
#include <utility>

// helper type for the visitor
// visit [https://en.cppreference.com/w/cpp/utility/variant/visit] for more info
template<class... Ts>
struct overloaded : Ts... { using Ts::operator()...; };
// explicit deduction guide (not needed as of C++20)
template<class... Ts>
overloaded(Ts...) -> overloaded<Ts...>;


RequestType request_to_request_type(const request_variant &var) {
    return
    std::visit(overloaded {
            [](const join_game_request&) { return RequestType::join_game; },
            [](const start_game_request&) { return RequestType::start_game; },
            [](const play_card_request&) { return RequestType::play_card; },
            [](const draw_card_request&) { return RequestType::draw_card; },
            [](const fold_request&) { return RequestType::fold; },
            [] (auto) { throw TichuException("request_variant could not be turned into RequestType"); },
    }, var);
}

// for deserialization
//TODO: send ints instead of string?
const std::unordered_map<std::string, RequestType> client_request::_string_to_request_type = {
        {"join_game", RequestType::join_game },
        {"start_game", RequestType::start_game},
        {"play_card", RequestType::play_card},
        {"draw_card", RequestType::draw_card},
        {"fold", RequestType::fold}
};
// for serialization
const std::unordered_map<RequestType, std::string> client_request::_request_type_to_string = {
        { RequestType::join_game, "join_game" },
        { RequestType::start_game, "start_game"},
        { RequestType::play_card, "play_card"},
        { RequestType::draw_card, "draw_card"},
        {RequestType::fold, "fold"}
};

// protected constructor. only used by subclasses
client_request::client_request(client_request::base_class_properties props, request_variant request) :
        _props(std::move(props)),
        _request(std::move(request))
{ }


// used by subclasses to retrieve information from the json stored by this superclass
client_request::base_class_properties client_request::extract_base_class_properties(const rapidjson::Value& json) {
    if (json.HasMember("player_id") && json.HasMember("game_id") && json.HasMember("req_id")) {
        UUID player_id = UUID(json["player_id"].GetString());
        UUID game_id = UUID(json["game_id"].GetString());
        UUID req_id = UUID(json["req_id"].GetString());
        return base_class_properties {
                client_request::_string_to_request_type.at(json["type"].GetString()),
                req_id,
                player_id,
                game_id
        };
    }
    else
    {
        throw TichuException("Client Request did not contain player_id or game_id");
    }
}

void client_request::write_into_json(rapidjson::Value &json,
                                     rapidjson::MemoryPoolAllocator<rapidjson::CrtAllocator> &allocator) const {
    rapidjson::Value type_val(_request_type_to_string.at(this->_props.type).c_str(), allocator);
    json.AddMember("type", type_val, allocator);

    rapidjson::Value player_id_val(_props.player_id.c_str(), allocator);
    json.AddMember("player_id", player_id_val, allocator);

    rapidjson::Value game_id_val(_props.game_id.c_str(), allocator);
    json.AddMember("game_id", game_id_val, allocator);

    rapidjson::Value req_id_val(_props.req_id.c_str(), allocator);
    json.AddMember("req_id", req_id_val, allocator);

    std::visit(overloaded {
        [&](auto req) { req.write_into_json(json, allocator); }
    }, _request);
}

client_request client_request::from_json(const rapidjson::Value &json) {
    if (json.HasMember("type") && json["type"].IsString()) {
        const std::string type = json["type"].GetString();
        const RequestType request_type = client_request::_string_to_request_type.at(type);

        auto props = extract_base_class_properties(json);
        // Check which type of request it is and call the respective from_json constructor
        if (request_type == RequestType::play_card) {
            return client_request(props, play_card_request::from_json(json));
        }
        else if (request_type == RequestType::draw_card) {
            return client_request(props, draw_card_request::from_json(json));
        }
        else if (request_type == RequestType::fold) {
            return client_request(props, fold_request::from_json(json));
        }
        else if (request_type == RequestType::join_game) {
            return client_request(props, join_game_request::from_json(json));
        }
        else if (request_type == RequestType::start_game) {
            return client_request(props, start_game_request::from_json(json));
        } else {
            throw TichuException("Encountered unknown ClientRequest type " + type);
        }
    }
    throw TichuException("Could not determine type of ClientRequest. JSON was:\n" + json_utils::to_string(json));
}


std::string client_request::to_string() const {
    return "client_request of type " + client_request::_request_type_to_string.at(_props.type) + " for playerId " + _props.player_id.string() + " and gameId " + _props.game_id.string();
}

client_request::client_request(UUID player_id, UUID game_id, const request_variant& var)
: _request(var), _props(base_class_properties{request_to_request_type(var), UUID::create(), std::move(player_id), std::move(game_id)})
{
}






