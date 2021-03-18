//
// Created by Manuel on 29.01.2021.
//

#include "start_game_request.h"

#ifdef LAMA_SERVER
#include "../../server/game_instance_manager.h"
#include "../../server/game_instance.h"
#endif

// Public constructor
start_game_request::start_game_request(std::string game_id, std::string player_id)
        : client_request( client_request::create_base_class_properties(RequestType::start_game, uuid_generator::generate_uuid_v4(), player_id, game_id) )
{ }

// private constructor for deserialization
start_game_request::start_game_request(client_request::base_class_properties props) :
        client_request(props)
{ }

start_game_request* start_game_request::from_json(const rapidjson::Value& json) {
    return new start_game_request(client_request::extract_base_class_properties(json));
}

void start_game_request::write_into_json(rapidjson::Value &json,
                                         rapidjson::MemoryPoolAllocator<rapidjson::CrtAllocator> &allocator) const {
    client_request::write_into_json(json, allocator);
}

#ifdef LAMA_SERVER
request_response* start_game_request::execute() {
    std::string err;
    player* player;
    game_instance* game_instance_ptr;
    if (game_instance_manager::try_get_player_and_game_instance(_player_id, player, game_instance_ptr, err)) {
        if (game_instance_ptr->start_game(player, err)) {
            return new request_response(game_instance_ptr->get_id(), _req_id, true, game_instance_ptr->get_game_state()->to_json(), err);
        }
    }
    return new request_response("", _req_id, false, nullptr, err);
}
#endif