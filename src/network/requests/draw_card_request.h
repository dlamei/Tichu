//
// Created by Manuel on 29.01.2021.
//

#ifndef LAMA_DRAW_CARD_REQUEST_H
#define LAMA_DRAW_CARD_REQUEST_H

#include <string>
#include "client_request.h"
#include "../../../rapidjson/include/rapidjson/document.h"

class draw_card_request : public client_request {

private:
    int _nof_cards;

    /*
     * Private constructor for deserialization
     */
    draw_card_request(base_class_properties, int val);

public:
    draw_card_request(std::string game_id, std::string player_id, int nof_cards = 1);
    virtual void write_into_json(rapidjson::Value& json, rapidjson::Document::AllocatorType& allocator) const override;
    static draw_card_request* from_json(const rapidjson::Value& json);

#ifdef LAMA_SERVER
    virtual server_response* execute() override;
#endif
};


#endif //LAMA_DRAW_CARD_REQUEST_H
