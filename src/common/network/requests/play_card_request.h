//
// Created by Manuel on 28.01.2021.
//

#ifndef TICHU_PLAY_CARD_REQUEST_H
#define TICHU_PLAY_CARD_REQUEST_H


//#include "client_request.h"
#include <string>
#include "../../../../rapidjson/include/rapidjson/document.h"

class play_card_request {

private:

    UUID _card_id;

    //Private constructor for deserialization
    //play_card_request(base_properties, std::string card_id);

public:
    [[nodiscard]] UUID get_card_id() const { return _card_id; }

    //play_card_request(std::string game_id, std::string player_id, std::string card_id);
    explicit play_card_request(UUID card_id);
    void write_into_json(rapidjson::Value& json, rapidjson::Document::AllocatorType& allocator) const;
    static play_card_request from_json(const rapidjson::Value& json);
};


#endif //TICHU_PLAY_CARD_REQUEST_H
