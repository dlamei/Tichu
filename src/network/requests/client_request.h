//
// Created by Manuel on 28.01.2021.
//

#ifndef LAMA_CLIENT_REQUEST_H
#define LAMA_CLIENT_REQUEST_H

#include <string>
#include <unordered_map>
#include "../../../rapidjson/include/rapidjson/document.h"
#include "../../reactive_state/serializable.h"
#include "../../common/utils/LamaException.h"
#include "../../common/utils/uuid_generator.h"
#include "../../common/utils/json_utils.h"

#ifdef LAMA_SERVER
#include "../responses/server_response.h"
#include "../responses/request_response.h"
#endif

// Identifier for the different request types.
// The RequestType is sent with every request to identify the type of request on the other end.
enum RequestType {
    join_game,
    start_game,
    play_card,
    draw_card,
    fold,
};

class client_request : public serializable {
protected:

    struct base_class_properties {
        RequestType _type;
        std::string _req_id;
        std::string _player_id;
        std::string _game_id;
    };

    RequestType _type;
    std::string _req_id;
    std::string _player_id;
    std::string _game_id;

    explicit client_request(base_class_properties); // base constructor
    static base_class_properties create_base_class_properties(RequestType type, std::string req_id, std::string& player_id, std::string& game_id);
    static base_class_properties extract_base_class_properties(const rapidjson::Value& json);

private:

    // for deserialization
    static const std::unordered_map<std::string, RequestType> _string_to_request_type;
    // for serialization
    static const std::unordered_map<RequestType, std::string> _request_type_to_string;

public:
    virtual ~client_request() {}

    std::string get_player_id() const;
    std::string get_game_id() const;

    /*
     * Tries to create the specific client_request from the provided json.
     * Throws exception if parsing fails -> Use only in "try{ }catch()" block
     */
    static client_request* from_json(const rapidjson::Value& json);

    /*
     * Serializes the client_request into a json object that can be sent over the network
     */
    void write_into_json(rapidjson::Value& json, rapidjson::Document::AllocatorType& allocator) const override;

    virtual std::string to_string() const;

    // Code that should only exist on the server side
#ifdef LAMA_SERVER
    // Execute this request on the server side
    virtual server_response* execute() = 0;
#endif
};


#endif //LAMA_CLIENT_REQUEST_H
