//
// Created by Manuel on 29.01.2021.
//

#ifndef TICHU_JOIN_GAME_REQUEST_H
#define TICHU_JOIN_GAME_REQUEST_H


#include <string>
//#include "client_request.h"
#include "../../../../rapidjson/include/rapidjson/document.h"

class join_game_request {
        //: public client_request{

private:
    std::string _player_name;

    /*
     * Private constructor for deserialization
     */

public:
    explicit join_game_request(const std::string &name);

    [[nodiscard]] std::string get_player_name() const { return this->_player_name; }
    /*
     * Constructor to join any game
     */
    //join_game_request(std::string player_id, std::string name);

    /*
     * Constructor to join a specific game
     */
    //join_game_request(std::string game_id, std::string player_id, std::string name);

    void write_into_json(rapidjson::Value& json, rapidjson::Document::AllocatorType& allocator) const;
    static join_game_request from_json(const rapidjson::Value& json);
};


#endif //TICHU_JOIN_GAME_REQUEST_H
