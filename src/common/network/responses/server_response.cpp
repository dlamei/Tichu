//
// Created by Manuel on 15.02.2021.
//

#include "server_response.h"

#include <utility>
#include "request_response.h"
#include "full_state_response.h"

#include "../../exceptions/TichuException.h"

// helper type for the visitor
// visit [https://en.cppreference.com/w/cpp/utility/variant/visit] for more info
template<class... Ts>
struct overloaded : Ts... { using Ts::operator()...; };
// explicit deduction guide (not needed as of C++20)
template<class... Ts>
overloaded(Ts...) -> overloaded<Ts...>;


ResponseType response_to_response_type(const response_variant &var) {
    return
            std::visit(overloaded {
                    [](const full_state_response&) { return ResponseType::full_state_msg; },
                    [](const request_response&) { return ResponseType::req_response; },
                    [] (auto) { throw TichuException("server_response could not be turned into RequestType"); },
            }, var);
}

// for deserialization
const std::unordered_map<std::string, ResponseType> server_response::_string_to_response_type = {
        {"req_response", ResponseType::req_response },
        {"full_state_msg", ResponseType::full_state_msg}
};

// for serialization
const std::unordered_map<ResponseType, std::string> server_response::_response_type_to_string = {
        { ResponseType::req_response,   "req_response" },
        { ResponseType::full_state_msg, "full_state_msg"}
};

//server_response::base_properties
//server_response::create_base_class_properties(ResponseType type, UUID game_id) {
//    return { .game_id = std::move(game_id), .type = type};
//}

server_response::base_properties server_response::extract_base_class_properties(const rapidjson::Value& json) {
    if (json.HasMember("type") && json.HasMember("game_id")) {
        UUID game_id = UUID(json["game_id"].GetString());
        return server_response::base_properties {
                .game_id = game_id,
                .type = server_response::_string_to_response_type.at(json["type"].GetString()),
        };
    }
    else
    {
        throw TichuException("Server Response did not contain game_id");
    }
}


server_response server_response::from_json(const rapidjson::Value& json) {

    if (json.HasMember("type") && json["type"].IsString()) {
        std::string type = json["type"].GetString();
        ResponseType response_type = server_response::_string_to_response_type.at(type);

        auto props = extract_base_class_properties(json);

        if (response_type == ResponseType::req_response) {
            auto req = request_response::from_json(json);
            return server_response(props.game_id, req);
        }
        else if (response_type == ResponseType::full_state_msg) {
            auto req = full_state_response::from_json(json);
            return server_response(props.game_id, req);
        } else {
            throw TichuException("Encountered unknown ServerResponse type " + _response_type_to_string.at(response_type));
        }
    }
    throw TichuException("Could not determine type of ClientRequest");
}

void server_response::write_into_json(rapidjson::Value &json,
                                      rapidjson::MemoryPoolAllocator<rapidjson::CrtAllocator> &allocator) const {
    rapidjson::Value type_val(_response_type_to_string.at(_type).c_str(), allocator);
    json.AddMember("type", type_val, allocator);

    rapidjson::Value game_id_val(_game_id.c_str(), allocator);
    json.AddMember("game_id", game_id_val, allocator);
}

server_response::server_response(UUID game_id, const response_variant& var)
: _game_id(std::move(game_id)), _type(response_to_response_type(var)), _response(var)
{
}

#ifdef TICHU_CLIENT
void server_response::Process() const {
    std::visit(overloaded {
        [](auto resp) { resp.Process(); }
    }, _response);
}
#endif