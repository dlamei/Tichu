//
// Created by Manuel on 29.01.2021.
//

#include "leave_game_request.h"

#ifdef LAMA_SERVER
#include "../../server/game_instance_manager.h"
#include "../../server/player_manager.h"
#include "../../server/game_instance.h"
#endif

// Public constructor
leave_game_request::leave_game_request(std::string game_id, std::string player_id)
        : client_request( client_request::create_base_class_properties(RequestType::leave_game, uuid_generator::generate_uuid_v4(), player_id, game_id) )
{ }

// private constructor for deserialization
leave_game_request::leave_game_request(client_request::base_class_properties props) :
        client_request(props)
{ }

leave_game_request* leave_game_request::from_json(const rapidjson::Value& json) {
    return new leave_game_request(client_request::extract_base_class_properties((json)));
}

void leave_game_request::write_into_json(rapidjson::Value &json,
                                         rapidjson::MemoryPoolAllocator<rapidjson::CrtAllocator> &allocator) const {
    client_request::write_into_json(json, allocator);
}

#ifdef LAMA_SERVER
server_response* leave_game_request::execute() {
    player* player;
    std::string err;
    if (player_manager::try_get_player(_player_id, player)) {

#ifndef USE_DIFFS // not USE_DIFFS
        if (game_instance_manager::try_remove_player(player, _game_id, err)) {
            game_instance* game_instance_ptr = nullptr;
            game_instance_manager::try_get_game_instance(_game_id, game_instance_ptr);
            // return full game_state
            return new request_response(_game_id, _req_id, true, game_instance_ptr->get_game_state()->to_json(), err);
        }
#else   // USE_DIFFS
        object_diff game_state_diff(player->get_game_id(), "game_state");
        if (game_instance_manager::try_remove_player(player, _game_id, game_state_diff, err)) {
            // return diff
            return new request_response(_game_id, _req_id, true, game_state_diff.to_json(), err);
        }
#endif

    } else {
        err = "Requested player " + _player_id + " could not be found in the database.";
    }
    return new request_response("", _req_id, false, nullptr, err);
}
#endif