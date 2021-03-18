//
// Created by Manuel on 29.01.2021.
//

#include "join_game_request.h"

#ifdef LAMA_SERVER
#include <string>
#include "../../../server/game_instance_manager.h"
#include "../../../server/player_manager.h"
#include "../../../server/game_instance.h"
#endif

std::string join_game_request::undefined_game_id {""};

// Public constructor
join_game_request::join_game_request(std::string player_id, std::string name)
        : client_request( client_request::create_base_class_properties(RequestType::join_game, uuid_generator::generate_uuid_v4(), player_id, join_game_request::undefined_game_id) ),
          _player_name(name)
{ }

join_game_request::join_game_request(std::string game_id, std::string player_id, std::string name)
        : client_request( client_request::create_base_class_properties(RequestType::join_game, uuid_generator::generate_uuid_v4(), player_id, game_id) ),
          _player_name(name)
{ }

// private constructor for deserialization
join_game_request::join_game_request(client_request::base_class_properties props, std::string player_name) :
        client_request(props),
        _player_name(player_name)
{ }

void join_game_request::write_into_json(rapidjson::Value &json,
                                        rapidjson::MemoryPoolAllocator<rapidjson::CrtAllocator> &allocator) const {
    client_request::write_into_json(json, allocator);
    rapidjson::Value name_val(_player_name.c_str(), allocator);
    json.AddMember("player_name", name_val, allocator);
}

join_game_request* join_game_request::from_json(const rapidjson::Value& json) {
    if (json.HasMember("player_name")) {
        return new join_game_request(client_request::extract_base_class_properties(json), json["player_name"].GetString());
    } else {
        throw LamaException("Could not parse join_game_request from json. player_name is missing.");
    }
}

#ifdef LAMA_SERVER

request_response* join_game_request::execute() {
    player* player;
    player_manager::add_or_get_player(_player_name, _player_id, player);

    std::string err;
    game_instance* game_instance_ptr = nullptr;
    if (_game_id.empty()) {
        // join any game
        if (game_instance_manager::try_add_player_to_any_game(player, game_instance_ptr, err)) {
            // game_instance_ptr got updated to the joined game

            // return response with full game_state attached
            return new request_response(game_instance_ptr->get_id(), _req_id, true,
                                        game_instance_ptr->get_game_state()->to_json(), err);
        } else {
            return new request_response("", _req_id, false, nullptr, err);
        }
    }
    else {
        if (game_instance_manager::try_get_game_instance(_game_id, game_instance_ptr)) {
            if (game_instance_manager::try_add_player(player, game_instance_ptr, err)) {
                // return response with full game_state attached
                return new request_response(_game_id, _req_id, true, game_instance_ptr->get_game_state()->to_json(), err);
            } else {
                return new request_response("", _req_id, false, nullptr, err);
            }
        } else {
            return new request_response("", _req_id, false, nullptr, "Requested game could not be found.");
        }
    }
}

#endif


