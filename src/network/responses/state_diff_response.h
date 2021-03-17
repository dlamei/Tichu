//
// Created by Manuel on 15.02.2021.
//

#ifndef LAMA_STATE_DIFF_RESPONSE_H
#define LAMA_STATE_DIFF_RESPONSE_H

#include "server_response.h"
#include "../../reactive_state/diffs/diff.h"

class state_diff_response : public server_response {
private:
    rapidjson::Value* _diff_json;

    /*
     * Private constructor for deserialization
     */
    state_diff_response(base_class_properties props, rapidjson::Value* diff_json);

public:

    state_diff_response(const std::string game_id, const diff& diff);
    ~state_diff_response();

    rapidjson::Value* get_state_diff_json() const;

    virtual void write_into_json(rapidjson::Value& json, rapidjson::Document::AllocatorType& allocator) const override;
    static state_diff_response* from_json(const rapidjson::Value& json);

#ifdef LAMA_CLIENT
    virtual void Process() const override;
#endif
};


#endif //LAMA_STATE_DIFF_RESPONSE_H
