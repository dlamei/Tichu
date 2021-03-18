//
// Created by Manuel on 25.01.2021.
//

#ifndef LAMA_CARD_H
#define LAMA_CARD_H

#include <string>
#include "../../reactive_state/reactive_objects/reactive_object.h"
#include "../../reactive_state/reactive_objects/reactive_value.h"
#include "../../../rapidjson/include/rapidjson/document.h"

class card : public reactive_object {
private:
    reactive_value<int>* _value;

    // from_diff constructor
    card(base_params params);
    // deserialization constructor
    card(base_params params, reactive_value<int>* val);
public:
    card(int val);
    ~card();

// accessors
    int get_value() const noexcept;

// card functions
    bool can_be_played_on(const card* const other) const noexcept;

// serializable interface
    void write_into_json(rapidjson::Value& json, rapidjson::Document::AllocatorType& allocator) const override;
    static card* from_json(const rapidjson::Value& json);
};


#endif //LAMA_CARD_H
