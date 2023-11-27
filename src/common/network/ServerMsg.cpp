#include "ServerMsg.h"

#include <utility>

#include "../exceptions/TichuException.h"

// helper type for the visitor
// visit [https://en.cppreference.com/w/cpp/utility/variant/visit] for more info
template<class... Ts>
struct overloaded : Ts ... {
    using Ts::operator()...;
};
// explicit deduction guide (not needed as of C++20)
template<class... Ts>
overloaded(Ts...) -> overloaded<Ts...>;


ServerMsgType response_to_response_type(const server_msg_variant &var) {
    return
            std::visit(overloaded{
                    [](const full_state_response &) { return ServerMsgType::full_state_response; },
                    [](const request_response &) { return ServerMsgType::req_response; },
                    [](auto) { throw TichuException("ServerMsg could not be turned into ClientMsgType"); },
            }, var);
}

server_msg_variant variant_from_json(ServerMsgType type, const rapidjson::Value &json) {
    switch (type) {
        case ServerMsgType::req_response: {
            auto err = string_from_json("error_msg", json);
            auto success = bool_from_json("success", json);
            std::optional<GameState> state = {};
            if (json.HasMember("state")) {
                state = GameState::from_json(json["state"]);
            }

            if (!(err && success)) {
                throw TichuException("Could not parse request_response from json. err or success is missing.");
            }
            return request_response{
                    success.value(),
                    state,
                    err.value(),
            };
        }

        case ServerMsgType::full_state_response: {
            if (!json.HasMember("state")) throw TichuException("full_state_response did not contain a state object");
            return full_state_response{ GameState::from_json(json["state"]) };
        }

        default:
            throw TichuException("Encountered unknown ServerMsg type " + std::to_string((int)type));
    }
}

ServerMsg ServerMsg::from_json(const rapidjson::Value &json) {

    auto type_opt = int_from_json("type", json);
    auto game_id_opt = string_from_json("game_id", json);

    if (!(type_opt && game_id_opt)) {
        throw TichuException("Could not determine type of ServerMsg");
    }

    auto type = ServerMsgType(type_opt.value());
    UUID game_id = UUID(game_id_opt.value());

    auto msg_data = variant_from_json(type, json);

    return ServerMsg(game_id, msg_data);
}

void ServerMsg::write_into_json(rapidjson::Value &json,
                                rapidjson::MemoryPoolAllocator<rapidjson::CrtAllocator> &alloc) const {

    int_into_json("type", (int)get_type(), json, alloc);
    string_into_json("game_id", _game_id.string(), json, alloc);

    switch (get_type()) {

        case ServerMsgType::req_response: {
            auto &resp = std::get<request_response>(_response);
            bool_into_json("success", resp.success, json, alloc);
            string_into_json("error_msg", resp.err, json, alloc);
            if (resp.state.has_value()) {
                resp.state->write_into_json_obj("state", json, alloc);
            }
            break;
        }
        case ServerMsgType::full_state_response: {
            const auto &resp = std::get<full_state_response>(_response);
            resp.state.write_into_json_obj("state", json, alloc);
            break;
        }
    }
}

ServerMsg::ServerMsg(UUID game_id, server_msg_variant var)
        : _game_id(std::move(game_id)), _response(std::move(var)) {
}

ServerMsgType ServerMsg::get_type() const {
    return response_to_response_type(_response);
}