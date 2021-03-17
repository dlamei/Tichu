//
// Created by Manuel on 29.01.2021.
//

#ifndef LAMA_LEAVE_GAME_REQUEST_H
#define LAMA_LEAVE_GAME_REQUEST_H


#include <string>
#include "client_request.h"
#include "../../../rapidjson/include/rapidjson/document.h"

class leave_game_request : public client_request{

private:

    /*
     * Private constructor for deserialization
     */
    explicit leave_game_request(base_class_properties);

public:
    leave_game_request(std::string game_id, std::string player_id);
    virtual void write_into_json(rapidjson::Value& json, rapidjson::Document::AllocatorType& allocator) const override;
    static leave_game_request* from_json(const rapidjson::Value& json);

#ifdef LAMA_SERVER
    virtual server_response* execute() override;
#endif
};


#endif //LAMA_LEAVE_GAME_REQUEST_H
