//
// Created by Manuel on 15.02.2021.
//

#include "state_diff_response.h"

#include <iostream>

#include "../../common/utils/LamaException.h"
#include "../../common/utils/json_utils.h"
#include "../../reactive_state/diffs/object_diff.h"

state_diff_response::state_diff_response(server_response::base_class_properties props, rapidjson::Value* diff_json) :
        server_response(props),
        _diff_json(diff_json)
{ }

state_diff_response::state_diff_response(const std::string game_id, const diff& state_diff) :
server_response(server_response::create_base_class_properties(ResponseType::state_diff_msg, game_id))
{
    this->_diff_json = state_diff.to_json();
}


void state_diff_response::write_into_json(rapidjson::Value &json,
                                          rapidjson::MemoryPoolAllocator<rapidjson::CrtAllocator> &allocator) const {
    server_response::write_into_json(json, allocator);

    rapidjson::Value state_val(json_utils::to_string(_diff_json).c_str(), allocator);
    json.AddMember("diff_json", *_diff_json, allocator);
}

state_diff_response *state_diff_response::from_json(const rapidjson::Value& json) {
    if (json.HasMember("diff_json")) {
        return new state_diff_response(
                server_response::extract_base_class_properties(json),
                json_utils::create_pointer_to_clone(json["diff_json"].GetObject()));
    } else {
        throw LamaException("Could not parse full_state_response from json. state is missing.");
    }
}

state_diff_response::~state_diff_response() {
    if (_diff_json != nullptr) {
        delete _diff_json;
        _diff_json = nullptr;
    }
}

rapidjson::Value* state_diff_response::get_state_diff_json() const {
    return this->_diff_json;
}


#ifdef LAMA_CLIENT

void state_diff_response::Process() const {
    try {
        object_diff* game_state_diff = object_diff::from_json(*_diff_json);

        // TODO update GUI with this game_state_diff
    } catch(std::exception& e) {
        std::cerr << "Failed to extract game_state from full_state_response" << std::endl
                  << e.what() << std::endl;
    }
}

#endif
