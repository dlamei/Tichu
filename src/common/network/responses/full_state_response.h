//
// Created by Manuel on 15.02.2021.
//

#ifndef TICHU_FULL_STATE_RESPONSE_H
#define TICHU_FULL_STATE_RESPONSE_H

#include "../../game_state/game_state.h"
#include "../../src/common/serialization/serializable.h"

class full_state_response {
private:
    std::shared_ptr<rapidjson::Value> _state_json;

    /*
     * Private constructor for deserialization
     */

public:

    explicit full_state_response(std::shared_ptr<rapidjson::Value> state_json);
    explicit full_state_response(const game_state& state);

    [[nodiscard]] const rapidjson::Value &get_state_json() const;

    virtual void write_into_json(rapidjson::Value& json, rapidjson::Document::AllocatorType& allocator) const;
    static full_state_response from_json(const rapidjson::Value& json);

#ifdef TICHU_CLIENT
    //TODO: move to game_state
    void Process() const;
#endif
};


#endif //TICHU_FULL_STATE_RESPONSE_H
