#include "client_msg.h"

#include <iostream>
#include <utility>



std::string client_msg::msg_type_to_string(ClientMsgType msg_type) {
    for (auto [typ, str] : msg_type_to_string_map) {
        if (typ == msg_type) {
            return str;
        }
    }

    return "UnknownMsgType";
}

ClientMsgType string_to_msg_type(const std::string &msg_str) {
    for (auto [typ, str] : msg_type_to_string_map) {
        if (str == msg_str) {
            return typ;
        }
    }

    throw TichuException("Unknown ClientMsgType: " + msg_str);
}

void client_msg::write_into_json(rapidjson::Value &json,
                                 rapidjson::MemoryPoolAllocator<rapidjson::CrtAllocator> &alloc) const {

    string_into_json("type", msg_type_to_string(get_type()), json, alloc);
    string_into_json("player_id", _player_id.string(), json, alloc);
    string_into_json("game_id", _game_id.string(), json, alloc);

    // handles the special cases for the different message types
    std::visit(overloaded {
            [&] (const join_game_req& data) {
                string_into_json("player_name", data.player_name, json, alloc);
            },

            [&] (const play_card_req& data) {
                data.played_card.write_into_json(json, alloc);
            },

            [&] (const draw_card_req& data) {
                int_into_json("nof_cards", data.nof_cards, json, alloc);
            },

            [&] (const start_game_req& data) {},
            [&] (const fold_req& data) {},

            [&] (auto) {},
    }, _request);
}

// helper function that tries to parse the given message type from json
client_msg_variant msg_variant_from_json(ClientMsgType type, const rapidjson::Value &json) {
    switch (type) {
        case start_game:
            return start_game_req {};
        case fold:
            return fold_req {};

        case join_game: {
            std::string player_name = string_from_json("player_name", json).value();
            return join_game_req { player_name };
        }

        case play_card: {
            card card = card::from_json(json);
            return play_card_req { card };
        }

        case draw_card: {
            int nof_cards = int_from_json("nof_cards", json).value();
            return draw_card_req { nof_cards };
        }

        default:
            throw TichuException("Encountered unknown ClientMsg type " + client_msg::msg_type_to_string(type));
    }

}

client_msg client_msg::from_json(const rapidjson::Value &json) {

    auto type_opt = string_from_json("type", json);
    auto player_id_opt = string_from_json("player_id", json);
    auto game_id_opt = string_from_json("game_id", json);

    if (!(type_opt && player_id_opt && game_id_opt)) {
        throw TichuException("Could not determine type of ClientRequest. JSON was:\n" + json_utils::to_string(json));
    }

    /// these 3 json fileds are the same for all client messages
    ClientMsgType type = string_to_msg_type(type_opt.value());
    UUID player_id = UUID(player_id_opt.value());
    UUID game_id = UUID(game_id_opt.value());

    return client_msg(player_id, game_id, msg_variant_from_json(type, json));
}


std::string client_msg::to_string() const {
    return "client_msg of type " + msg_type_to_string(get_type()) + " for playerId " + _player_id.string() + " and gameId " + _game_id.string();
}

ClientMsgType client_msg::get_type() const {
    return request_to_request_type(_request);
}

client_msg::client_msg(UUID player_id, UUID game_id, client_msg_variant  var)
        : _request(std::move(var)), _player_id(std::move(player_id)), _game_id(std::move(game_id))
{ }
