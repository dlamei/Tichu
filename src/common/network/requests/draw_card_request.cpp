//
// Created by Manuel on 29.01.2021.
//

#include "draw_card_request.h"

#ifdef LAMA_SERVER
#include "../../../server/game_instance_manager.h"
#include "../../../server/game_instance.h"
#endif

// Public constructor
draw_card_request::draw_card_request(std::string game_id, std::string player_id, int nof_cards)
        : client_request( client_request::create_base_class_properties(RequestType::draw_card, uuid_generator::generate_uuid_v4(), player_id, game_id) )
{
    _nof_cards = nof_cards;
}

// private constructor for deserialization
draw_card_request::draw_card_request(client_request::base_class_properties props, int nof_cards) :
        client_request(props),
        _nof_cards(nof_cards)
{ }

draw_card_request* draw_card_request::from_json(const rapidjson::Value &json) {
    base_class_properties props = client_request::extract_base_class_properties(json);
    if (json.HasMember("nof_cards") ) {
        return new draw_card_request(props, json["nof_cards"].GetInt());
    } else {
        throw LamaException("Could not find 'nof_cards' in draw_card_request");
    }
}

void draw_card_request::write_into_json(rapidjson::Value &json,
                                        rapidjson::MemoryPoolAllocator<rapidjson::CrtAllocator> &allocator) const {
    client_request::write_into_json(json, allocator);
    json.AddMember("nof_cards", rapidjson::Value(this->_nof_cards),allocator);
}

#ifdef LAMA_SERVER
request_response* draw_card_request::execute() {
    std::string err;
    player* player;
    game_instance* game_instance_ptr;
    if (game_instance_manager::try_get_player_and_game_instance(_player_id, player, game_instance_ptr, err)) {
        card* drawn_card;
        if (game_instance_ptr->draw_card(player, drawn_card, err)) {
            return new request_response(game_instance_ptr->get_id(), _req_id, true, game_instance_ptr->get_game_state()->to_json(), err);
        }
    }
    return new request_response("", _req_id, false, nullptr, err);
}
#endif