//
// Created by Manuel on 15.02.2021.
//

#include "full_state_response.h"

#include <utility>

#include "../../exceptions/TichuException.h"
#include "../../serialization/json_utils.h"

#ifdef TICHU_CLIENT
#include "../../../client/GameController.h"
#endif

full_state_response::full_state_response(std::shared_ptr<rapidjson::Value> state_json)
        : _state_json(std::move(state_json))
{ }

full_state_response::full_state_response(const game_state &state)
: _state_json(state.to_json())
{ }


void full_state_response::write_into_json(rapidjson::Value &json,
                                       rapidjson::MemoryPoolAllocator<rapidjson::CrtAllocator> &allocator) const {
    //server_response::write_into_json(json, allocator);
    json.AddMember("state_json", *_state_json, allocator);
}

full_state_response full_state_response::from_json(const rapidjson::Value& json) {
    if (json.HasMember("state_json")) {
        auto state =  json_utils::clone_value(json["state_json"].GetObject());
        auto shared_state = json_value_ptr(state);
        return full_state_response {shared_state };
    } else {
        throw TichuException("Could not parse full_state_response from json. state is missing.");
    }
}

const rapidjson::Value &full_state_response::get_state_json() const {
    return *_state_json;
}

#ifdef TICHU_CLIENT

void full_state_response::Process() const {
    try {
        game_state state = game_state::from_json(*_state_json);
        GameController::updateGameState(state);

    } catch(std::exception& e) {
        std::cerr << "Failed to extract game_state from full_state_response" << std::endl
                  << e.what() << std::endl;
    }
}

#endif
