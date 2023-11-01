//
// Created by Manuel on 15.02.2021.
//

#ifndef TICHU_REQUEST_RESPONSE_H
#define TICHU_REQUEST_RESPONSE_H

#include <string>
#include <optional>
#include "../../src/common/serialization/serializable.h"

class request_response {
private:
    bool _success;
    std::string _err;
    UUID _req_id;
    std::optional<json_value_ptr> _state_json;


public:

    request_response(UUID req_id, bool success, std::optional<json_value_ptr> state_json, const std::string &err);

    void write_into_json(rapidjson::Value& json, rapidjson::Document::AllocatorType& allocator) const;
    static request_response from_json(const rapidjson::Value& json);

#ifdef TICHU_CLIENT
    void Process() const;
#endif
};


#endif //TICHU_REQUEST_RESPONSE_H
