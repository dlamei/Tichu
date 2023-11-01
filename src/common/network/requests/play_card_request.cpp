//
// Created by Manuel on 28.01.2021.
//

#include "client_request.h"
#include "play_card_request.h"

#include <utility>

// Public constructor
play_card_request::play_card_request(UUID card_id)
        : _card_id(std::move(card_id))
{ }

play_card_request play_card_request::from_json(const rapidjson::Value& json) {
    if (json.HasMember("card_id")) {
        return play_card_request {UUID(json["card_id"].GetString())};
    } else {
        throw TichuException("Could not find 'card_id' or 'value' in play_card_request");
    }
}

void play_card_request::write_into_json(rapidjson::Value &json,
                                        rapidjson::MemoryPoolAllocator<rapidjson::CrtAllocator> &allocator) const {
    //client_request::write_into_json(json, allocator);
    rapidjson::Value card_id_val(_card_id.c_str(), allocator);
    json.AddMember("card_id", card_id_val,allocator);
}
