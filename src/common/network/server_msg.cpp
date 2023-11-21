//
// Created by Manuel on 15.02.2021.
//

#include "server_msg.h"

#include <utility>

#include "../exceptions/TichuException.h"

// helper type for the visitor
// visit [https://en.cppreference.com/w/cpp/utility/variant/visit] for more info
template<class... Ts>
struct overloaded : Ts... { using Ts::operator()...; };
// explicit deduction guide (not needed as of C++20)
template<class... Ts>
overloaded(Ts...) -> overloaded<Ts...>;


ServerMsgType response_to_response_type(const server_msg_variant &var) {
    return
            std::visit(overloaded {
                    [](const full_state_response&) { return ServerMsgType::full_state_response; },
                    [](const request_response&) { return ServerMsgType::req_response; },
                    [] (auto) { throw TichuException("server_msg could not be turned into ClientMsgType"); },
            }, var);
}

const std::vector<std::pair<ServerMsgType, const char *>> msg_type_to_string_map {
        {ServerMsgType::req_response,  "req_response"},
        {ServerMsgType::full_state_response, "full_state_response"},
};

std::string server_msg::msg_type_to_string(ServerMsgType msg_type) {
    for (auto [typ, str] : msg_type_to_string_map) {
        if (typ == msg_type) {
            return str;
        }
    }

    return "UnknownMsgType";
}

ServerMsgType server_msg::string_to_msg_type(const std::string &msg_str) {
    for (auto [typ, str] : msg_type_to_string_map) {
        if (str == msg_str) {
            return typ;
        }
    }

    throw TichuException("Unknown ClientMsgType: " + msg_str);
}

server_msg_variant variant_from_json(ServerMsgType type, const rapidjson::Value &json) {
    switch (type) {
        case ServerMsgType::req_response: {
            auto err = string_from_json("error_msg", json);
            auto success = bool_from_json("success", json);
            std::optional<json_document_ptr> state_json = {};
            state_json = json_utils::clone_value(json["state_json"].GetObject());
            if (!(err && success)) {
                throw TichuException("Could not parse request_response from json. err or success is missing.");
            }
            return request_response{
                    success.value(),
                    state_json,
                    err.value(),
            };
        }

        case ServerMsgType::full_state_response: {
            std::optional<json_document_ptr> state_json = {};
            state_json = json_utils::clone_value(json["state_json"].GetObject());
            if (state_json) {
                return full_state_response { state_json.value() };
            } else {
                throw TichuException("Could not parse full_state_response from json");
            }
        }

        default:
            throw TichuException("Encountered unknown ServerMsg type " + server_msg::msg_type_to_string(type));
    }
}

server_msg server_msg::from_json(const rapidjson::Value& json) {

    auto type_opt = string_from_json("type", json);
    auto game_id_opt = string_from_json("game_id", json);

    if (!(type_opt && game_id_opt)) {
        throw TichuException("Could not determine type of ClientRequest");
    }

    ServerMsgType type = string_to_msg_type(type_opt.value());
    UUID game_id = UUID(game_id_opt.value());

    auto msg_data = variant_from_json(type, json);

    return server_msg(game_id, msg_data);
}

void server_msg::write_into_json(rapidjson::Value &json,
                                 rapidjson::MemoryPoolAllocator<rapidjson::CrtAllocator> &alloc) const {

    string_into_json("type", msg_type_to_string(get_type()), json, alloc);
    string_into_json("game_id", _game_id.string(), json, alloc);

    std::visit(overloaded {
        [&] (const full_state_response& resp) {
            json.AddMember("state_json", *resp.state_json, alloc);
        },

        [&] (const request_response& resp) {
            bool_into_json("success", resp.success, json, alloc);
            string_into_json("error_msg", resp.err, json, alloc);
            if (resp.state_json) {
                json.AddMember("state_json", *resp.state_json.value(), alloc);
            }
        },
        [&] (auto) {},
    }, _response);
}

server_msg::server_msg(UUID game_id, server_msg_variant  var)
: _game_id(std::move(game_id)), _response(std::move(var))
{
}

ServerMsgType server_msg::get_type() const {
    return response_to_response_type(_response);
}

#ifdef TICHU_CLIENT
/*
#include "../game_state/game_state.h"
#include "../../client/GameController.h"

void process_response(const request_response &resp) {
    if (resp.success) {
        if (resp.state_json) {
            try {
            game_state state = game_state::from_json(*resp.state_json.value());
            GameController::updateGameState(state);
            } catch (std::exception e){}

        } else {
            GameController::showError("Network error", "Expected a state as JSON inside the request_response. But there was none.");
        }
    } else {
        GameController::showError("Not possible", resp.err);
    }

}

void process_response(const full_state_response &resp) {
    try {
        game_state state = game_state::from_json(*resp.state_json);
        GameController::updateGameState(state);

    } catch(std::exception& e) {
        std::cerr << "Failed to extract game_state from full_state_response" << std::endl
                  << e.what() << std::endl;
    }
}


void server_msg::Process() const {
    std::visit(overloaded {
            [](const request_response &resp) { process_response(resp); },
            [](const full_state_response &resp) { process_response(resp); },
            [](auto resp) {},
    }, _response);
}

 */
#endif