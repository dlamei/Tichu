//
// Created by Manuel on 15.02.2021.
//

#include "request_response.h"

#include <utility>
#include "../../serialization/json_utils.h"
#include "../../exceptions/TichuException.h"
#include "../../game_state/game_state.h"

#ifdef TICHU_CLIENT
#include "../../../client/GameController.h"
#endif


request_response::request_response(UUID req_id, bool success, std::optional<json_value_ptr> state_json, const std::string &err) :
    _req_id(std::move(req_id)),
    _state_json(std::move(state_json)),
    _success(success),
    _err(err)
{ }

void request_response::write_into_json(rapidjson::Value &json,
                                       rapidjson::MemoryPoolAllocator<rapidjson::CrtAllocator> &allocator) const {
    //server_response::write_into_json(json, allocator);

    rapidjson::Value err_val(_err.c_str(), allocator);
    json.AddMember("err", err_val, allocator);

    rapidjson::Value req_id_val(_req_id.c_str(), allocator);
    json.AddMember("req_id", req_id_val, allocator);

    json.AddMember("success", _success, allocator);

    if (_state_json) {
        json.AddMember("state_json", *_state_json.value(), allocator);
    }
}


request_response request_response::from_json(const rapidjson::Value& json) {
    if (json.HasMember("err") && json.HasMember("success")) {
        std::string err = json["err"].GetString();

        std::optional<json_document_ptr> state_json = {};
        if (json.HasMember("state_json")) {
            state_json = json_utils::clone_value(json["state_json"].GetObject());
        }

        return request_response{
                UUID(json["req_id"].GetString()),
                json["success"].GetBool(),
                state_json,
                err};
    } else {
        throw TichuException("Could not parse request_response from json. err or success is missing.");
    }
}

#ifdef TICHU_CLIENT

void request_response::Process() const {
    if (_success) {
        if (_state_json) {
            game_state state = game_state::from_json(*_state_json.value());
            GameController::updateGameState(state);

        } else {
            GameController::showError("Network error", "Expected a state as JSON inside the request_response. But there was none.");
        }
    } else {
        GameController::showError("Not possible", _err);
    }

}

#endif
